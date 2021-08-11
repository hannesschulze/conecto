project(
  'conecto',
  ['c', 'cpp'],
  version: '0.1.0',
  meson_version: '>= 0.46.0',
)

libconecto_deps = [
  dependency('jsoncpp', version: '>=1.7.4'),
  dependency('sigc++-2.0', version: '>=2.10.0'),
  dependency('giomm-2.4', version: '>=2.56.0'),
  dependency('gnutls', version: '>=3.5.18'),
  dependency('libnotify', version: '>=0.7.7'),
  dependency('atspi-2', version: '>=2.28.0'),
  dependency('gdkmm-3.0', version: '>=3.22.2'),
  dependency('sqlite3', version: '>=3.22.0'),
  dependency('folks', version: '>=0.11.4'),
]

pkgconfig = import('pkgconfig')
gnome = import('gnome')
i18n = import('i18n')

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

if get_option('docs')
  subdir('docs')
endif

subdir('tests')
subdir('data')
subdir('po')
subdir('src')

conecto_deps = [
  libconecto_dep,
  dependency('gtkmm-3.0'),
  dependency('granite'),
]

if not get_option('disable_plank_support')
  conecto_deps += dependency('plank')
endif

conecto_sources += gnome.compile_resources(
  'conecto-resources',
  'data/com.github.hannesschulze.conecto.gresources.xml',
  source_dir: 'data',
  c_name: 'conecto'
)

executable(
  'com.github.hannesschulze.conecto',
  conecto_sources,
  dependencies: conecto_deps,
  install: true
)

if not get_option('skip_postinstall')
  meson.add_install_script('build-aux/meson/postinstall.py')
endif