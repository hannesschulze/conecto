conecto_sources = files(
  'main.cpp',
  'application.cpp',
  'window.cpp',
  'device-popover.cpp',

  'controllers/settings-manager.cpp',
  'controllers/active-device-manager.cpp',

  'models/connected-devices.cpp',
  'models/unavailable-devices.cpp',
  'models/available-devices.cpp',

  'views/devices-list.cpp',
  'views/active-device-view.cpp',
  'views/empty-selection-view.cpp',
  'views/available-device-view.cpp',
  'views/unavailable-device-view.cpp',
  'views/connected-device-view.cpp',

  'widgets/header-bar.cpp',
  'widgets/circular-progress-bar.cpp',
  'widgets/popover-window.cpp',

  'utils/icons.cpp',
  'utils/focus.cpp',
)

if not get_option('disable_plank_support')
  conecto_sources += files('controllers/dock-item-manager.cpp')
endif