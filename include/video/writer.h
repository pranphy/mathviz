#pragma once

#include <string>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

class VideoWriter {
public:
    VideoWriter(const std::string& filename, int width, int height, int fps);
    ~VideoWriter();

    void push_frame(const uint8_t* rgb_data); // expects RGB24 buffer
    void flush();

private:
    int width, height, fps;
    int frame_index;

    AVFormatContext* fmt_ctx;
    AVStream* stream;
    AVCodecContext* codec_ctx;
    SwsContext* sws_ctx;
    AVFrame* frame;

};

