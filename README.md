![Antimony](/logo.png)

| Continuous Integration    | Last build    |
| ------------- |:-------------:|
| Microsoft VS Team Services          |![Microsoft Team Services CI](https://banderi.visualstudio.com/_apis/public/build/definitions/ed4db6b1-d928-4080-8bdc-edf144c3fcac/1/badge)|

_**Antimony**_ is an open source, general-purpose 3D-2D game engine built to accomodate many indie game ideas and concepts I had in mind. Feel free to contribute, fork or use at will for your personal or commercial projects, following my [license specifications](LICENSE.md).

## Usage

As of now, no available games or applications make use of this engine. Plans for future use on various commercially available games are currently underway.

## Current situation

As of now, the engine does not support any basic gameplay. It is only capable of rendering basic 3D shapes to the screen.
Read [TODO.md](TODO.md) for further information.

The project is currently Windows only. There are currently no plans for implementing support for multiple platforms.

## Tools and libraries required

The Windows SDK 8.1 is required to develop DirectX11 applications, so the project is only compatible with Windows Vista, 7, 8, 8.1 and 10 (currently only tested on Windows 7, 8.1 and 10). Support to the deprecated Windows XP and other versions of Windows is currently not given.

Both 64-bit (x64) and 32-bit (x86) version of the operating systems are supported, though compilation of the code in 64-bit is unconfirmed to be free of issues at the moment.

Visual Studio 2015 is needed to open/build the solution, any other version is unconfirmed to be compatible with the project files. It is recommended to use the free official [Visual Studio 2015 Community](https://www.visualstudio.com/en-us/products/visual-studio-community-vs.aspx) provided by Microsoft.

The C/C++ code is written to work with the official MSVC 14.0 compiler. Other toolsets might work, but no official support is given.

The project currently supports static linkage against official Microsoft C Runtime and Microsoft DirectX 11 Runtime libraries, so no inclusion of redistributable bundles is needed for release deployment.

The project currently does not make use of the deprecated D3DX libraries, instead it uses the included [SimpleMath](https://blogs.msdn.microsoft.com/shawnhar/2013/01/08/simplemath-a-simplified-wrapper-for-directxmath/) library from the official [Microsoft DirectX Tool Kit for DirectX 11](https://github.com/Microsoft/DirectXTK), so no installation and inclusion of the deprecated DirectX SDK is required.

The project currently makes use of and includes parts of the [Autodesk FBX SDK 2017](http://usa.autodesk.com/adsk/servlet/pc/item?siteID=123112&id=25408427), [FW1FontWrapper Library](https://fw1.codeplex.com/) and [Bullet 3](https://github.com/bulletphysics/bullet3), which have been added to the source control for continuous integration. Note that the Autodesk FBX SDK and the FW1FontWrapper library are linked dynamically, due to the following reasons:
- Some (community) sources note that statically linking to the FBX SDK would go against the ToS (likely false)
- The FBX SDK is many times larger than the Antimony engine at the current stage and would benefit more from dynamic linkage
- The distribution of the FW1FontWrapper library offers no static .lib files out of the box

No other third-party library is currently required.

## Contributing

Please read [CONTRIBUTING.md](CONTRIBUTING.md) for details on our code of conduct, and the process for submitting pull requests to us.
Also be sure to take a look at [STANDARD.md](STANDARD.md) for details on the general coding conventions and guidelines.

## Authors

* **Federico Barlotti** - [Banderi](https://github.com/Banderi) - *Main author*

See also the list of [contributors](https://github.com/Banderi/Project-X/contributors) who participated in this project.

## License

This project is currently licensed under a license based on the [BSD](https://choosealicense.com/licenses/bsd-2-clause/) and [MIT](https://choosealicense.com/licenses/mit/) licenses - see the [LICENSE.md](LICENSE.md) file for details
