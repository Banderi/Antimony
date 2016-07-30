# Project-X

The greatness I possess in choosing project titles is without bounds.
This project is an open source, general-purpose 3D-2D game engine built to accomodate many indie game ideas and concepts I had in mind. Feel free to contribute, fork or use at will for your personal or commercial projects, following my [license specifications](LICENSE.md).

## Current situation

As of now, the engine does not support any basic gameplay. It is only capable of rendering basic 3D shapes to the screen.
Read [TODO.md](TODO.md) for further information.

The project is currently Windows only. There are currently no plans for implementing support for multiple platforms.

## Tools and libraries required

You will need the Windows 8.1 SDK to develop DirectX11 applications, and Visual Studio 2015 to open/build the solution. The project is compatible with Windows Vista, 7, 8, 8.1 and 10 (currently only tested on Windows 7, 8.1 and 10); Windows XP support is currently not given.
The C/C++ code is written to work with the official MSVC 14.0 compiler, other toolsets might work, but no official support is given.
The project currently supports static linkage against official Microsoft C Runtime and Microsoft DirectX 11 Runtime libraries, so no inclusion of redistributable is needed for release deployment.
The project currently does not make use of the deprecated D3DX libraries, so no installation and inclusion of the deprecated DirectX SDK is required.

## Contributing

Please read [CONTRIBUTING.md](CONTRIBUTING.md) for details on our code of conduct, and the process for submitting pull requests to us.

## Authors

* **Federico Barlotti** - [Banderi](https://github.com/Banderi) - *Main author*

See also the list of [contributors](https://github.com/Banderi/Project-X/contributors) who participated in this project.

## License

This project is currently licensed under the GPL v3.0 License - see the [LICENSE.md](LICENSE.md) file for details
