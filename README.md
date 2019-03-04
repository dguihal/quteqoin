# Quteqoin

Quteqoin is a chat application dedicated to free boards

## Building

Requirements : Qt 5.4 or newer

```
git clone https://github.com/dguihal/quteqoin.git
qmake
make
```

On linux ou can specify a base destination directory using:

```
qmake PREFIX=/usr
```

Then build with make and install using :

```
sudo make install
```

Default base directory for install is /usr/local

## Features

Current features includes :

 * Multi tabbed / multi threaded pinni
 * Full norloge support including shorten, with date (year support), indices, ....
 * Feature complete palmi
 * Totoz manager
 * Graphical configuration tool for easily managing your quteqoin
 * Duck hunting
 * Support for X-Post-Id, tsv & xml backends, url prettifer
