# Config files
conf_data = configuration_data()
conf_data.set_quoted('NAME', meson.project_name())
conf_data.set_quoted('VERSION', meson.project_version())
conf_data.set_quoted('APPID', 'com.github.hannesschulze.' + meson.project_name())
conf_data.set('ENABLE_PLANK_SUPPORT', not get_option('disable_plank_support'))
config_header = configure_file(
  input: 'constants.h.in',
  output: 'constants.h',
  configuration: conf_data
)
conf_inc = include_directories('.')

libconecto_sources = files(
  'backend.cpp',
  'device.cpp',
  'discovery.cpp',
  'network-packet.cpp',
  'communication-channel.cpp',
  'crypt.cpp',
  'config-file.cpp',

  'plugins/ping.cpp',
  'plugins/notifications.cpp',
  'plugins/battery.cpp',
  'plugins/mouse.cpp',
)

libconecto_headers = files(
  'conecto.h',
  'backend.h',
  'device.h',
  'discovery.h',
  'exceptions.h',
  'network-packet.h',
  'communication-channel.h',
  'crypt.h',
  'config-file.h',
  'abstract-packet-handler.h',

  'plugins/ping.h',
  'plugins/notifications.h',
  'plugins/battery.h',
  'plugins/mouse.h',
)
libconecto_headers += config_header

include_directories = [
  conf_inc,
  include_directories('.'),
  include_directories('./plugins'),
]
