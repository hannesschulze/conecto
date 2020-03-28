project(
  'conecto',
  ['cpp'],
  version: '0.1.0',
  meson_version: '>= 0.46.0',
)

libconecto_deps = [
  dependency('jsoncpp'),
  dependency('sigc++-2.0'),
  dependency('giomm-2.4'),
  dependency('gnutls'),
]

pkgconfig = import('pkgconfig')

subdir('libconecto')

libconecto = shared_library(
  meson.project_name(),
  libconecto_sources,
  dependencies: libconecto_deps,
  include_directories: include_directories,
  install: true,
  install_dir: true
)

libconecto_dep = declare_dependency(
  link_with: libconecto,
  include_directories: include_directories,
  dependencies: libconecto_deps
)

install_headers(libconecto_headers, subdir: 'conecto')

libconecto_pc = pkgconfig.generate(
  libconecto,
  name: meson.project_name(),
  requires: libconecto_deps,
  subdirs: ['conecto'],
  description: 'Library that serves as a client for KDE Connect',
  version: meson.project_version(),
  url: 'https://github.com/hannesschulze/conecto'
)

subdir('tests')