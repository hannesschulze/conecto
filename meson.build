project('com.github.hannesschulze.conecto', ['c', 'vala'], version: '0.1.0',
  meson_version: '>= 0.40.0',
)

i18n = import('i18n')

conf = configuration_data()
conf.set_quoted('GETTEXT_PACKAGE', meson.project_name())
configure_file(output: 'config.h', configuration: conf)
config_h_dir = include_directories('.')
c_args = [
  '-include', 'config.h'
]


gnome = import('gnome')

conecto_deps = [
  dependency('atspi-2'),
  dependency('gdk-3.0'),
  dependency('gee-0.8'),
  dependency('gio-2.0'),
  dependency('gio-unix-2.0'),
  dependency('glib-2.0'),
  dependency('granite', version: '>=0.4.2'),
  dependency('gtk+-3.0', version: '>=3.18'),
  dependency('gnutls'),
  dependency('gobject-2.0'),
  dependency('json-glib-1.0'),
  dependency('libebook-1.2'),
  dependency('libedataserver-1.2'),
  dependency('libnotify'),
  dependency('unity', version: '>=4.0.0'),
  dependency('sqlite3'),
  meson.get_compiler('vala').find_library('posix'),
]

add_project_arguments(
  [ '--vapidir', join_paths(meson.current_source_dir(), 'src/MConnect/vapi') ],
  language: 'vala'
)

subdir('src')
subdir('po')
subdir('data')


executable('com.github.hannesschulze.conecto',
  conecto_sources,
  vala_args: '--target-glib=2.50',
  dependencies: conecto_deps,
  install: true,
  c_args: c_args
)

meson.add_install_script('build-aux/meson/postinstall.py')
