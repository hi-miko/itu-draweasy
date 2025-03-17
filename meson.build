project('draweasy', 'cpp',
        version: '0.1',
        default_options: ['cpp_std=c++17'])

gtkmm_dep = dependency('gtkmm-4.0', required: true)

sources = [
  './src/main.cpp',
  './src/draw-view.cpp',
  './src/draw-controller.cpp',
  './src/draw-model.cpp'
]

executable(meson.project_name(), sources,
           dependencies: [gtkmm_dep])
