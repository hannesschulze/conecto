# Conecto

> Integrate your Android-phone with your PC - a GTK Client for KDEConnect.

## Current State Of The Project

> ⚠️ Unfortunately, this project is currently put on hold. It is still possible to install the C++ rewrite in its current state on elementary OS (tested on elementary OS 6.0). For more information, see [#30 (comment)](https://github.com/hannesschulze/conecto/issues/30).

The goal of this project was originally to update and maintain [EOSConnect](https://github.com/gyan000/EOSConnect) by gyan000. However, that soon turned out to be difficult because the version of mconnect it was based on was too outdated, causing some issues and communication problems. Beacuse of this, and with the aim to provide a new user interface for the application (see [#3](https://github.com/hannesschulze/conecto/issues/3)), I decided to rewrite the application in C++.

The rewrite is currently able to manage device connections and display the battery level. There has been some work to support notifications and the mousepad-feature. However, because the new library is still WIP there are still many bugs, unimplemented features and missing tests.

![data/screenshot.png](Current version of Conecto running on elementary OS 6)

## Installation instructions

### Dependencies
These dependencies must be present before building:
 - `elementary-sdk`
 - `libjsoncpp-dev`
 - `libgtkmm-3.0-dev`
 - `libghc-gnutls-dev`
 - `libnotify-dev`
 - `libsqlite3-dev`
 - `libfolks-dev`

Use the following command to install the dependencies:
```shell
sudo apt install elementary-sdk libjsoncpp-dev libgtkmm-3.0-dev libghc-gnutls-dev libnotify-dev libsqlite3-dev libfolks-dev
```

### Building
```
git clone https://github.com/hannesschulze/conecto.git && cd conecto
meson build --prefix=/usr
cd build
ninja
```

To install, use `ninja install`, then execute with com.github.hannesschulze.conecto:
```shell
sudo ninja install
com.github.hannesschulze.conecto
```

### Generating documentation

Documentation can be generated using doxygen. Once you have cloned the repository and are in the `build` directory, run:

```
meson configure -Ddocs=true
ninja
```

You will find the generated documentation in `build/docs/libconecto`
