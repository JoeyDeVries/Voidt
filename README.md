## C/C++ Low-level game code (ongoing)

### Notes

- Repository is not built around supporting different run-time configurations.
- Built with 64 bit Visual C++ compiler; see misc/build.bat for compilation options.

### Low-level game code

Voidt is the solo effort of aiming to develop a relatively simple, but performant game-engine and game in C with some features of C++ without the use of any libraries, including a graphics API like OpenGL. This involves software-based rendering, custom window management code with back-buffer memory, advanced memory management, SIMD-based mathematics and much more as inspired by the [Handmade Hero](https://handmadehero.org/) series; basically as low-level as one can get without having to write one's own operating system.

The project is still in development; for more information regarding the project: see <a href="http://joeydevries.com/#portfolio" target="_blank">joeydevries.com</a>.

### Features

- Software-based renderer
- Epic graphics
- Local multiplayer
- Manual Win32 window/input management
- Custom memory allocater
- DirectSound circular audio buffer
- SIMD intrinsics