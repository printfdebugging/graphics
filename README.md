## Environment Setup

- Windows
  - Install MSYS2 with `winget install MSYS2.MSYS2`, add `C:\msys64\clang64\bin` to PATH
  - Install Git with `winget install Git.Git`

## Dependencies

- Windows
  - Run `dependencies.sh` from `MSYS2 CLANG64` shell
  - This installs compilers, build tools etc
- Archlinux
  - Run `dependencies.sh` from "THE BASH"
  - You use Arch BTW, so no spoon feeding for you!

## Libraries

- Different libraries use different build systems, like `meson`, `cmake`, `autotools`
- External libraries that we use are included as submodules in `./subprojects`
- All build systems discover installed libraries through `pkgconf`
- We build the libraries one by one and install them in `./install`
- These libraries generate pkg-config files in `./install/lib/pkgconfig`
- Run `./externals.sh` to build and install these libraries
  - Run in "THE BASH" on Archlinux
  - Run in `MSYS2 CLANG64` shell on Windows
- We use a bash script to compile/install external libraries because other ways are painful

## Bulid Steps

- With CMake
  - This is a CMake project
  - Source files live in `./source`, header files live in `./include`
  - See `build.sh` for the full CMake command
- With Script
  - Windows: Run the `./build.sh` script from `MSYS2 CLANG64` shell (or Git Bash if path has `C:\msys64\clang64\bin`)
  - Archlinux: Run the `./build.sh` script in "THE BASH"
- With VSCode
  - Install `ms-vscode.cpptools-extension-pack` extension pack
  - Press `<Shift+F5>` to build the project and attach a debugger

## Style

- Everything :snake: `snake_case`
- Avoid unnecessary `typedef`
- Format as per `.clang-format`