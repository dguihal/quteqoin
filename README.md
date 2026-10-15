# Quteqoin

Quteqoin is a chat application dedicated to free boards

## Building

Requirements: CMake 3.22 or newer, a C++17 compiler, and Qt 6.4 or newer
(Core, Gui, DBus, Multimedia, MultimediaWidgets, Network, Widgets).
Qt Test is also required when `BUILD_TESTING` is enabled, which is the default.

On Ubuntu 24.04, install the build dependencies:

```sh
sudo apt-get install cmake ninja-build g++ qt6-base-dev qt6-multimedia-dev libgl1-mesa-dev
```

Build outside the source directory:

```sh
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=ON
cmake --build build --parallel 2
./build/quteqoin
```

## Tests and CI

```sh
ctest --test-dir build --output-on-failure --no-tests=error
```

The behavior tests use an offscreen Qt platform, a local HTTP server, and
temporary settings. They do not require a display or access to online boards.

GitHub Actions builds the application and runs the tests on Ubuntu 24.04 in
Debug and Release configurations for pull requests, pushes to `main` or
`master`, and tags starting with `v`. It can also be started manually.
JUnit reports and test logs are retained as artifacts for seven days.
This workflow complements CodeQL rather than replacing it.

Actions are pinned to commit hashes; Dependabot checks for updates weekly.
The `Linux (Debug)` and `Linux (Release)` checks can be required in GitHub branch
protection rules. Use `-DBUILD_TESTING=OFF` to build without Qt Test.

## Features

Current features includes :

 * Multi tabbed / multi threaded pinni
 * Full norloge support including shorten, with date (year support), indices, ....
 * Feature complete palmi
 * Totoz manager
 * Graphical configuration tool for easily managing your quteqoin
 * Duck hunting
 * Support for X-Post-Id, tsv & xml backends, url prettifer
