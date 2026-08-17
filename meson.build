project('MathViz', ['c', 'cpp'],
  version : '0.2.0',
  default_options : ['cpp_std=c++23', 'warning_level=3']
)

glfw_dep = dependency('glfw3')
gl_dep = dependency('gl')
avformat_dep = dependency('libavformat')
avcodec_dep = dependency('libavcodec')
swscale_dep = dependency('libswscale')
avutil_dep = dependency('libavutil')
freetype_dep = dependency('freetype2')
harfbuzz_dep = dependency('harfbuzz')

glad_lib = static_library('glad',
  'glad/src/gl.c',
  include_directories : include_directories('glad/include'),
  override_options : ['warning_level=0']
)

glslang = find_program('glslangValidator', required: true)

# Simply list the shader source files:
shader_files = [
  'res/shaders/fullscreen_quad.vert',
  'res/shaders/mandelbrot.frag',
  'res/shaders/newton_frac.frag',
  'res/shaders/julia.frag',
  'res/shaders/generic.frag',
  'res/shaders/standing_wave.frag',
  'res/shaders/geometry.frag',
  'res/shaders/atom.vert',
  'res/shaders/atom.frag',
  'res/shaders/newton_general.frag',
  'res/shaders/plot2d.frag',
  'res/shaders/plot3d.frag',
  'res/shaders/grid2d.frag',
  'res/shaders/plots/polynomial.frag',
  'res/shaders/plots/waves.frag',
  'res/shaders/plots/fourier.frag',
  'res/shaders/plots/soliton.frag',
  'res/shaders/plots/implicit.frag',
  'res/shaders/plots/polar.frag',
  'res/shaders/plots/surface_ripple.frag',
  'res/shaders/plots/surface_membrane.frag',
  'res/shaders/line_plot.vert',
  'res/shaders/line_plot.frag'
]

# Central list of all shared helper/include files.
# Append any new custom #include files here:
common_glsl_includes = files(
  'res/shaders/colormap.glsl',
  'res/shaders/common_uniforms.glsl',
  'res/shaders/math_functions.glsl'
)

shader_headers = []

foreach f : shader_files
  # Parse path to deduce targets and C++ variable names (e.g. mandelbrot.frag -> mandelbrot_frag_spirv)
  parts = f.split('/')
  filename = parts[parts.length() - 1]
  file_parts = filename.split('.')

  base = file_parts[0] + '_' + file_parts[1]
  var_name = base + '_spirv'

  target = custom_target(base + '_h',
    input: f,
    output: base + '.h',
    command: [glslang, '-G', '--vn', var_name, '@INPUT@', '-o', '@OUTPUT@'],
    depend_files: common_glsl_includes
  )
  shader_headers += target
endforeach

executable('mathviz',
  'src/main.cpp',
  'src/utils/all.cpp',
  'src/utils/math.cpp',
  'src/writer/video.cpp',
  'src/writer/ppm.cpp',
  'src/ogl/app.cpp',
  'src/scene/scene.cpp',
  'src/scene/mandelbrot.cpp',
  'src/scene/julia.cpp',
  'src/scene/generic.cpp',
  'src/scene/geometry.cpp',
  'src/scene/atom.cpp',
  'src/scene/newton_fractal.cpp',
  'src/scene/plot.cpp',
  'src/scene/plot2d.cpp',
  'src/scene/plot3d.cpp',
  'src/scene/line_plot2d.cpp',
  'src/text/font_atlas.cpp',
  'src/text/text_renderer.cpp',
  shader_headers,
  include_directories : [include_directories('include', 'glad/include')],
  link_with : glad_lib,
  dependencies : [glfw_dep, gl_dep, avformat_dep, avcodec_dep, swscale_dep, avutil_dep, freetype_dep, harfbuzz_dep],
  cpp_args : ['-pedantic']
)
