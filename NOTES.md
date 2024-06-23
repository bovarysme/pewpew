# NOTES
## Setting up a Windows development environment
### C++ toolchain

- Install MSYS2 with the default installation path.
- Update the base installation:
```
$ pacman -Suy
```
- Install the clang64 toolchain and CMake:
```
$ pacman -S mingw-w64-clang-x86_64-toolchain mingw-w64-clang-x86_64-cmake
```
- Add `C:\msys64\clang64\bin` to the PATH.

### Git

- Install Git for Windows.

### Text editor

- Install VSCode.
- Install the C/C++, CMake Tools, and Vim extensions.
- Edit `settings.json`:
```
{
    "editor.formatOnSave": true,
    "telemetry.telemetryLevel": "off"
}
```

## Building and running

- Generate a CMake build directory with the Release config:
```
$ cmake -B build -D CMAKE_BUILD_TYPE=Release
```
- Build and run, writing the output to a file as UTF-8:
```
$ cmake --build build; .\build\pewpew.exe | Set-Content image.ppm -encoding String
```

## Questions for Lyse, the C++ and graphics programming goddess

- Is it fine to have so much code in header files?
  - It's fine as long as the functions are simple[^1].
- `Vec3`: array of coords vs. individual coords as fields?
  - I prefer to have them as individual public fields. There are no invariants
    so the lack of encapsulation is fine. There are cases were you might want
    to directly modify individual coords.
- Do you use the `Float` trick everywhere (e.g. even for the camera logic)?
  - Yeah, even though there are spots where it doesn't matter.
- Do you use `auto`?
  - Very rarely, e.g. when iterating[^1].
- What does the `const` qualifier do for member functions?
  - It makes the object immutable.
- Operator overloading: member vs. non-member?
  - Member: assignment overloading. Non-member: everything else.

[^1]: The [Google C++ Style
Guide](https://google.github.io/styleguide/cppguide.html) agrees.