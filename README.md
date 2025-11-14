# Martmists's Switch Mods
![GitHub License](https://img.shields.io/github/license/Martmists-GH/switch-mods)
![GitHub Actions Workflow Status](https://img.shields.io/github/actions/workflow/status/martmists-gh/switch-mods/build.yml)

This repository holds all the mods I'm currently actively developing. Some of my older mods might not be present due to the switch to hakkun.

This repository template is free to use under CC0, with the code in `src/` requiring you follow the [LICENSE](LICENSE).

## Mods

Pokemon Scarlet/Violet

| Mod Name                                                     | Description                              |
|--------------------------------------------------------------|------------------------------------------|
| [SV_CrowdControl](src/pokemon_sv/mod_crowd_control)          | Crowd Control for Pokemon Scarlet/Violet |
| [ZA_Debug_Tools](src/pokemon_za/mod_debug_tools)             | Debug Menu for Pokemon Legends ZA        |
| [ZA_Same_Gender_Rival](src/pokemon_za/mod_same_gender_rival) | Same Gender Rival for Pokemon Legends ZA |

## Repository contents

- `build-files` - Contains some files used by the toolchain to build the mod
- `cmake` - Contains the build toolchain and utilities
- `lib` - Contains all external libraries used
- `src` - Contains all the source files for mods
  - `include` - Contains includes for the specific game/mod
  - `lang` - Contains translation files for mods
  - `romfs` - Contains files that will be copied to romfs for the mod
  - `src` - Contains the code used for the custom exefs mod
  - `sym` - Contains files required by [sail](https://github.com/fruityloops1/LibHakkun/tree/main/hakkun/sail) to generate symbols
- `tools` - Contains a variety of useful scripts

## Building

### Prerequisites:

- A Linux-like environment (WSL should work, but is untested)
- Python 3.12+
- The clang compiler
- Cmake
- CLion is recommended but not strictly necessary

#### Optional: Setting up CLion

1. Open the project
2. Go to `File > Settings > Build, Execution, Deployment > CMake`
3. Add `-DCMAKE_TOOLCHAIN_FILE=cmake/toolchain.cmake` to the CMake options field. Optionally add `-DCMAKE_VERBOSE_MAKEFILE=ON`
4. Reload the CMake project

### Building the mods

#### Using CLion

1. Select the target in the top-right dropdown
2. Click the 'Build Project' button next to it (the hammer icon)

#### Using the command line

```bash
$ cmake -S . -B build --toolchain=cmake/toolchain.cmake
$ cmake --build build
```

## License

This project is licensed under the BSD-4-Clause license, and can be found in the [LICENSE](LICENSE) file.

## Credits

A huge thanks to the following people:

- [Fruityloops1](https://github.com/fruityloops1) for developing Hakkun, the code injection layer used, and for sitting through my endless barrage of questions
- [CraftyBoss](https://github.com/CraftyBoss) for implementing the ImGui backend for NVN
- The [open-ead](https://github.com/open-ead) project, for maintaining public headers for the NX SDK
- The [Coding Den Discord](https://discord.com/invite/code) for explaining a lot of weird C++ stuff

## Donations

If you'd like to support me, you can do so on my [Sponsors](https://github.com/sponsors/Martmists-GH) page.

