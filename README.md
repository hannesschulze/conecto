## EOSConnect

It's a KDEConnect client using GTK+ written in Vala, based on mconnect [mconnect](https://github.com/kevinselvaprasanna/mconnect).
The application is designed for elementary OS. (but should works on other distributions, perhaps.)

A note on [mconnect](https://github.com/kevinselvaprasanna/mconnect). First version of EOSConnect used a custom fork of mconnect, extending his DBus API, so used as a backend, started by EOSConnect at startup. But I encoutered lot's of issues with async DBus calls in order to setup mconnect backend correctly, and EOSConnect began to have too much files and abstractions classes - in order to be able to change the backend later. So I decided to integrate directly mconnect code into EOSConnect, removing his DBus API. It removed lot's of code in EOSConnect and I was able to setup and adapt mconnect like I want easily.

### Warning

It's a Work In Progess, not intented for daily use.

### Installation
1 - On Ubuntu or elementary OS, install these dependencies:
```bash
sudo apt install  elementary-sdk libunity-dev libnotify-dev libghc-gnutls-dev libsqlite3-dev libedataserver1.2-dev libebook1.2-dev/ -y
```
2 - Clone the repository.
3 - Inside the project, compile with:
```bash
meson build && clear && ninja -C build && glib-compile-schemas ./data/
```
4.1 - To start it from the build directory:
```bash
GSETTINGS_SCHEMA_DIR=data ./build/src/com.github.gyan000.eos-connect 
```
4.2 - To start it, with complete debug output:
```bash
GSETTINGS_SCHEMA_DIR=data ./build/src/com.github.gyan000.eos-connect -d
```
5 - To install it:
```bash
sudo ninja -C build install
```

### Known issues
* When launching app, sometimes, configured device, not connected on local network, does not appears as "not connected".
* When launching app, sometimes, need to unlock phone/tablet in order to see the device "connected".
* Need to click on quicklist option "Send or view SMS" to display contact list in SMS tab.

### Roadmap
#### v0.1
- [x] Import mconnect, remove useless stuff.
- [x] List available devices.
- [x] Use Tablet or Phone or Computre icons in devices list.
- [x] Pair automatically.
- [x] Plugin configuration by device: plugin list, custom name.
- [x] List configured device in dynamic quicklist.
- [x] Dconf: global setting and by device settings.

#### v0.2 - ping
- [x] Send / receive ping, with notification.
- [x] Ping plugin configurarion. (button to send a ping.)

#### v0.3 - battery
- [x] Receive notification when battery below a threshold.
- [x] Battery plugin configuration. (option to setup charge percentage warning)
- [x] Show battery percentage in dynamic quicklist.

#### v0.4 - notification
- [x] Send / receive notifications.
- [x] Notification plugin configuration. (configuration window to activate/deactivate send/receive notifications globally and by application, with application filter.)
- [] Synchronise notifications on both devices. (when notification closed on a device, it should close on the other device.)

#### v0.4 - share
- [x] Send one or more files, with popup with file advancement.
- [x] Receive files, with advancement using launcher api to display progress bar on the icon in the dock.
- [x] Share plugin configuration. (option to setup default save folder)

#### v0.5 - SMS
- [x] Send / receive SMS, with SMS notification.
- [x] Option to display SMS tab in dynamic quicklist.
- [x] Tab in main window with SMS history by contact. 
- [x] Add an icon on header bar to send SMS to one or more contacts. 
- [x] Use libedataserver & libebook to get available adressbooks and take contacts with mobile phone. (and use contact photo if available)
- [x] Widget to send SMS, with input text and dropdown to select available device, which can send a SMS. (to use in the widget send SMS fom header bar, and into the SMS tab.)
- [x] When entering phone number or contact name in SMS widget from headerbar, display available contacts, with photo.
- [x] Use sqlite3 to store SMS by phone number. (and be able to scroll back in the history)
- [] When user do not want to store SMS with sqlite, use a dummy SMS store.
- [] General option, for all devices, to activate or not the storage of SMS history.
- [] SMS plugin configuration. (to setup how many days of history we want to keep)

#### v0.5.1 - Telephony
- [] Display who's calling, with contact photo if available.
- [] Make sure we display notification when call missed.
- [] Telephony plugin configuration.

#### v0.7 - Commands
- [] Execute commands.
- [] Command plugin configuration. (configuration window with available commands for remove device)

#### v0.8 - Clipboard
- [] Copy / past working. (in both direction ?)
- [] Clipboard plugin configuration. 

#### v0.9 - Multimedia control 
- [] Multimedia control plugin configuration.

#### v0.9.1 - Remote control
- [] Remote control plugin configuration

#### v1
- [] Remove refresh icon at bottom of devices list. 
- [] Unpair device, with icon at bottom of devices list.
- [] Option, when unpairing a device, to delete everything: configuration, SMS history.
- [] Welcome screen, with how-to setup.
- [] Option to setup public visible name of EOSConnect.
- [] Translations: french.
- [] Code cleanup.
