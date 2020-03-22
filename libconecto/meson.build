# Config files
conf_data = configuration_data()
conf_data.set_quoted('NAME', meson.project_name())
conf_data.set_quoted('VERSION', meson.project_version())
config_header = configure_file(
  input: 'config.h.in',
  output: 'config.h',
  configuration: conf_data
)
conf_inc = include_directories('.')

libconecto_sources = files(
  'backend.cpp',
  'device.cpp',
  'discovery.cpp',
)

libconecto_headers = files(
  'conecto.h',
  'backend.h',
  'device.h',
  'discovery.h',
  'exceptions.h',
)
libconecto_headers += config_header

include_directories = [
  conf_inc,
  include_directories('.'),
]