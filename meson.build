project('MandelbrotGL', ['c', 'cpp'],
  version : '0.1.0',
  default_options : ['cpp_std=c++23', 'warning_level=3']
)

glfw_dep = dependency('glfw3')
gl_dep = dependency('gl')
avformat_dep = dependency('libavformat')
avcodec_dep = dependency('libavcodec')
swscale_dep = dependency('libswscale')
avutil_dep = dependency('libavutil')

glad_lib = static_library('glad',
  'glad/src/gl.c',
  include_directories : include_directories('glad/include'),
  override_options : ['warning_level=0']
)

executable('mandelbrot',
  'src/main.cpp',
  'src/video/writer.cpp',
  'src/mandelbrot/mandelbrot.cpp',
  'src/ogl/app.cpp',
  'src/scene/scene.cpp',
  include_directories : [include_directories('include', 'glad/include')],
  link_with : glad_lib,
  dependencies : [glfw_dep, gl_dep, avformat_dep, avcodec_dep, swscale_dep, avutil_dep],
  cpp_args : ['-pedantic']
)
