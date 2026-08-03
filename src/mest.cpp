#include <stdexcept>
#include <string>
#include <vector>
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

class VideoWriter {
public:
    VideoWriter(const std::string& filename, int w, int h, int fps_);
    ~VideoWriter();

    void push_frame(const uint8_t* rgb_data);
    void flush();

private:
    int width, height, fps;
    int64_t frame_index;

    AVFormatContext* fmt_ctx;
    AVStream* stream;
    AVCodecContext* codec_ctx;
    SwsContext* sws_ctx;
    AVFrame* frame;
};

VideoWriter::VideoWriter(const std::string& filename, int w, int h, int fps_)
    : width(w), height(h), fps(fps_), frame_index(0),
      fmt_ctx(nullptr), stream(nullptr), codec_ctx(nullptr),
      sws_ctx(nullptr), frame(nullptr)
{
    avformat_alloc_output_context2(&fmt_ctx, nullptr, nullptr, filename.c_str());
    if (!fmt_ctx) throw std::runtime_error("Could not allocate format context");

    const AVCodec* codec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if (!codec) throw std::runtime_error("H.264 codec not found");

    stream = avformat_new_stream(fmt_ctx, codec);
    if (!stream) throw std::runtime_error("Could not create stream");

    codec_ctx = avcodec_alloc_context3(codec);
    codec_ctx->width = width;
    codec_ctx->height = height;
    codec_ctx->time_base = {1, fps};
    codec_ctx->framerate = {fps, 1};
    codec_ctx->pix_fmt = AV_PIX_FMT_YUV420P;
    codec_ctx->gop_size = 12;
    codec_ctx->max_b_frames = 2;
    codec_ctx->bit_rate = 4000000; // optional

    stream->time_base = codec_ctx->time_base;

    if (fmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
        codec_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

    if (avcodec_open2(codec_ctx, codec, nullptr) < 0)
        throw std::runtime_error("Could not open codec");

    avcodec_parameters_from_context(stream->codecpar, codec_ctx);

    if (!(fmt_ctx->oformat->flags & AVFMT_NOFILE)) {
        if (avio_open(&fmt_ctx->pb, filename.c_str(), AVIO_FLAG_WRITE) < 0)
            throw std::runtime_error("Could not open output file");
    }

    if (avformat_write_header(fmt_ctx, nullptr) < 0)
        throw std::runtime_error("Could not write header");

    frame = av_frame_alloc();
    frame->format = codec_ctx->pix_fmt;
    frame->width = codec_ctx->width;
    frame->height = codec_ctx->height;
    if (av_frame_get_buffer(frame, 32) < 0)
        throw std::runtime_error("Could not allocate frame buffer");

    sws_ctx = sws_getContext(width, height, AV_PIX_FMT_RGB24,
                             width, height, AV_PIX_FMT_YUV420P,
                             SWS_BILINEAR, nullptr, nullptr, nullptr);
}

VideoWriter::~VideoWriter() {
    flush();
    av_write_trailer(fmt_ctx);
    avcodec_free_context(&codec_ctx);
    av_frame_free(&frame);
    sws_freeContext(sws_ctx);
    if (!(fmt_ctx->oformat->flags & AVFMT_NOFILE))
        avio_closep(&fmt_ctx->pb);
    avformat_free_context(fmt_ctx);
}

void VideoWriter::push_frame(const uint8_t* rgb_data) {
    const uint8_t* srcSlice[1] = { rgb_data };
    int srcStride[1] = { 3 * width };

    sws_scale(sws_ctx, srcSlice, srcStride, 0, height,
              frame->data, frame->linesize);

    frame->pts = frame_index++; // increment in units of 1/fps

    AVPacket* pkt = av_packet_alloc();
    if (!pkt) throw std::runtime_error("Could not allocate packet");

    if (avcodec_send_frame(codec_ctx, frame) < 0)
        throw std::runtime_error("Error sending frame");

    while (avcodec_receive_packet(codec_ctx, pkt) == 0) {
        av_packet_rescale_ts(pkt, codec_ctx->time_base, stream->time_base);
        pkt->stream_index = stream->index;
        av_interleaved_write_frame(fmt_ctx, pkt);
        av_packet_unref(pkt);
    }
    av_packet_free(&pkt);
}

void VideoWriter::flush() {
    AVPacket* pkt = av_packet_alloc();
    avcodec_send_frame(codec_ctx, nullptr); // signal flush
    while (avcodec_receive_packet(codec_ctx, pkt) == 0) {
        av_packet_rescale_ts(pkt, codec_ctx->time_base, stream->time_base);
        pkt->stream_index = stream->index;
        av_interleaved_write_frame(fmt_ctx, pkt);
        av_packet_unref(pkt);
    }
    av_packet_free(&pkt);
}

int main() {
    const int W = 640, H = 480, FPS = 30;
    VideoWriter writer("test.mp4", W, H, FPS);

    std::vector<uint8_t> buffer(W * H * 3, 128); // gray frame

    for (int i = 0; i < 90; ++i) { // 3 seconds at 30 fps
        writer.push_frame(buffer.data());
    }

    // destructor flushes and finalizes
    return 0;
}

