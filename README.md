# SCP - Containment Breach (RBFX Port)

This is a port of the game SCP - Containment Breach into the RBFX framework. This project is an attempt to have more stability and better modding tools.

This game and the source code are licensed under [Creative Commons Attribution-ShareAlike 3.0 License](http://creativecommons.org/licenses/by-sa/3.0/).

## Building the Project and the Framework from source

This option has longer build times but you can easily modify the engine.

1. Checkout [the Framework](https://github.com/rbfx/rbfx) as `rbfx` directory anywhere, if you didn't do that already.
2. Checkout this repository near `rbfx` directory.
3. Configure and build it via CMake. For example:

```
cd path/to/project
cmake . ./cmake-build
cmake --build ./cmake-build -j12
```

## Building the Project with pre-built Framework SDK

This option has shorter build times but you have to use pre-built version of the engine.

1. Download [pre-built SDK of the Framework](https://github.com/rbfx/rbfx/releases/tag/latest) for your platform and compiler anywhere.
2. Checkout this repository anywhere.
3. Configure and build it via CMake. Variable `REBELFORK_SDK` should be set to the folder `SDK` from within the zip archive. Example:

```
cd path/to/project
cmake . ./cmake-build -DBUILD_SHARED_LIBS=ON -DREBELFORK_SDK=path/to/prebuilt/SDK
cmake --build ./cmake-build -j12
```

If pre-built SDKs available on GitHub are not suitable for you, you can build the Framework as SDK on your own.
Check [documentation](https://rbfx.github.io/index.html) for build instructions.

## Running the Sample

Just run `SCP-Containment-Breach.exe` on Windows or `SCP-Containment-Breach` on Linux.

Make sure that `rbfx/bin/CoreData` is visible by the executable.
It is usually copied automatically by CMake, but if it doesn't, you can copy it manually.

Alternatively, you can run `SCP-Containment-Breach` executable with `--pp path/to/bin` command line argument
to specify the path to the directory containing `CoreData`.
