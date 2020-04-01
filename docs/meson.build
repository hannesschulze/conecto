doxygen = find_program('doxygen', required: false)
dot = find_program('dot', required: false)
if not doxygen.found()
  error('Program "doxygen" not found, try building with -Ddocs=false')
endif
if not dot.found()
  error('Program "dot" (graphviz) not found, try building with -Ddocs=false')
endif

config_docs = configuration_data()
config_docs.set('PACKAGE_NAME', meson.project_name())
config_docs.set('PACKAGE_VERSION', meson.project_version())
config_docs.set('OUTDIR', meson.current_build_dir())
config_docs.set('SRCDIR', meson.source_root())

doxyfile = configure_file(
  input: 'Doxyfile.in',
  output: 'Doxyfile',
  configuration: config_docs,
  install: false
)

custom_target(
  'doxygen',
  input: [doxyfile, libconecto_headers, libconecto_sources, join_paths(meson.source_root(), 'README.md')],
  output: '.',
  command: [doxygen, doxyfile],
  install: false,
  build_by_default: true,
)