<div align="center">
    <h1 align="center">Conecto</h1>
    <h3 align="center">Integrate your Android-phone with your PC - a GTK client for KDEConnect.</h3>
</div>

<br/>

<p align="center">
  <a href="https://github.com/hannesschulze/conecto/blob/master/COPYING">
    <img src="https://img.shields.io/badge/License-GPL--2.0-blue.svg">
  </a>
  <a href="https://github.com/hannesschulze/conecto/releases">
    <img src="https://img.shields.io/badge/Release-v%201.0.0-orange.svg">
  </a>
</p>

<p align="center">
    <a href="https://github.com/hannesschulze/conecto/issues/new">Report a problem!</a>
</p>

## Installation

### Dependencies
These dependencies must be present before building:
 - `meson`
 - `valac`
 - `debhelper`
 - `libgranite-dev`
 - `libgtk-3-dev`
 - `libunity-dev`
 - `libnotify-dev`
 - `libghc-gnutls-dev`
 - `libsqlite3-dev`
 - `libedataserver1.2-dev`
 - `libebook1.2-dev`

Use the following command to install the dependencies:
```shell
sudo apt install elementary-sdk libunity-dev libnotify-dev libghc-gnutls-dev libsqlite3-dev libedataserver1.2-dev libebook1.2-dev
```

### Building
```
git clone https://github.com/hannesschulze/conecto.git && cd conecto
meson build
ninja -C build
glib-compile-schemas ./data/
```

To install, use `ninja install`:
```shell
sudo ninja -C build install
```

## About this project

This project aims to provide a KDEConnect client designed for elementary OS, based on [mconnect](https://github.com/kevinselvaprasanna/mconnect).

Conecto is based on [EOSConnect](https://github.com/gyan000/EOSConnect) by gyan000. Unfortunately he wasn't able to continue development because he didn't own an android device anymore.

This project is a work in progress, not intended for daily use.

Current features include:
 - Connecting to the android device
 - Pinging the device
 - Sharing files through devices
 - Showing the battery level
 - Receiving notifications
 - Sending SMS

## License

This project is licensed under the GPL-2.0 License - see the [COPYING](COPYING) file for details.
