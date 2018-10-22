project(
    'com.github.gyan000.eos-connect',
    'vala', 'c',
    version: '0.4.9')

gettext_name = meson.project_name()
gnome = import('gnome')
i18n = import('i18n')

add_project_arguments(
    '-DGETTEXT_PACKAGE="@0@"'.format(gettext_name),
    '-DI_KNOW_THE_PACKAGEKIT_GLIB2_API_IS_SUBJECT_TO_CHANGE',
    language:'c'
)

dep_atspi = dependency('atspi-2')
dep_gdk = dependency('gdk-3.0')
dep_gee = dependency ('gee-0.8')
dep_gio = dependency ('gio-2.0')
dep_gio_unix = dependency('gio-unix-2.0')
dep_glib = dependency('glib-2.0')
dep_granite = dependency ('granite', version: '>=0.4.2')
dep_gtk = dependency ('gtk+-3.0', version: '>=3.18')
dep_gnutls = dependency('gnutls')
dep_gobject = dependency('gobject-2.0')
dep_json_glib = dependency('json-glib-1.0')
dep_libebook = dependency('libebook-1.2')
dep_libedataserver = dependency('libedataserver-1.2')
dep_libnotify = dependency('libnotify')
dep_posix = meson.get_compiler('vala').find_library('posix')
#dep_libsoup = dependency ('libsoup-2.4')
dep_unity = dependency('unity', version: '>=4.0.0')
dep_sqlite = dependency('sqlite3')

add_project_arguments(['--vapidir', join_paths(meson.current_source_dir(), 'src/MConnect/vapi')], language: 'vala')

dependencies = [
    dep_atspi,
    dep_gdk,
    dep_gee,
    dep_gio,
    dep_gio_unix,
    dep_glib,
    dep_gnutls,
    dep_gobject,
    dep_granite,
    dep_gtk,
    dep_json_glib,
    dep_libebook,
    dep_libedataserver,
    dep_libnotify,
    #dep_libsoup,
    dep_posix,
    dep_sqlite,
    dep_unity
]

conf_data = configuration_data()
conf_data.set('PROJECT_NAME', meson.project_name())
#conf_data.set('DESKTOP_ICON', get_option('icon'))
conf_data.set('EXEC_NAME', meson.project_name())
conf_data.set('GETTEXT_PACKAGE', meson.project_name())
#conf_data.set('APP_NAME', get_option('name'))
conf_data.set('VERSION', meson.project_version())
#conf_data.set('CONFIGDIR', config_dir)
conf_data.set('EXEC_PATH', join_paths (get_option('prefix'), get_option('bindir'), meson.project_name()))

meson.add_install_script('meson/post_install.py')

subdir('data')
subdir('src')
subdir('po')
