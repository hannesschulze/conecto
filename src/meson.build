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
  'models/notifications-list.cpp',
  'models/sms-storage.cpp',

  'views/main/devices-list.cpp',
  'views/main/active-device-view.cpp',
  'views/main/empty-selection-view.cpp',
  'views/main/available-device-view.cpp',
  'views/main/unavailable-device-view.cpp',
  'views/main/connected-device-view.cpp',

  'views/dock/active-device-view.cpp',
  'views/dock/warning-view.cpp',
  'views/dock/connected-device-view.cpp',

  'views/notifications-view.cpp',
  'views/sms-view.cpp',

  'widgets/header-bar.cpp',
  'widgets/circular-progress-bar.cpp',
  'widgets/popover-window.cpp',
  'widgets/notification-row.cpp',

  'utils/icons.cpp',
  'utils/focus.cpp',
)

if not get_option('disable_plank_support')
  conecto_sources += files('controllers/dock-item-manager.cpp')
endif