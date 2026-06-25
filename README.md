# Graphics

## Environment Setup

- Windows
  - Install MSYS2 with `winget install MSYS2.MSYS2`
  - Add `C:\msys64\clang64\bin` to PATH
    - You can do that from the *Edit the system environment variables* dialog
    - Or run this command (from the codeblock below) in Powershell
  - Install Git with `winget install Git.Git`
  - Install VSCode with `winget install Microsoft.VisualStudioCode`
```powershell
if ($env:Path -notlike "*C:\msys64\clang64\bin*") { [Environment]::SetEnvironmentVariable("Path", [Environment]::GetEnvironmentVariable("Path", "User") + ";C:\msys64\clang64\bin", "User") }
```

## Dependencies

- On Windows, open the `MSYS2 CLANG64` shell
- On Linux, use "THE BASH"
- Run `./scripts/dependencies.sh`, this installs compilers, build tools etc

> [!NOTE]
> `dependencies.sh` only installs packages for Archlinux and MSYS2. If you
> use some other distribution, feel free to find the equivalent package
> names and add them to the script, thanks!

## Bulid Steps

```bash
# pull in the git submodules
git submodule update --init --recursive

# build with cmake
cmake -B build
cmake --build build

# run the executable(s)
./build/bin/editor
./build/bin/game
```

## Style

- Everything :snake: `snake_case`
- Avoid unnecessary `typedef`ing
- Format as per `.clang-format`

## Error Handling

- Error should be propogated up the function call stack.
- On error, every function should cleanup it's allocations and let the parent handle their allocations
- Error message should be printed/logged at the site it happened and very bottom of the stack, rest all should just cleanup and propogate

## Memory Management

- Never use `malloc`, use `calloc` as that sets the memory to `0` and we can use that to set some default flags to 0, like `initialized`.
- Always call `foo_init` after allocation.