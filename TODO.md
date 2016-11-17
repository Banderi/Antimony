# TODO list for Antimony OpenEngine

### Todo:
- [ ] Core
    - [ ] Implement core subsystem management
    - [ ] Implement 3D graphics
        - [x] Basic 3D rendering, vertex/index buffering, vector/matrix management
        - [x] Basic 3D geometry
        - [ ] Basic texture rendering
        - [ ] 3D lights
        - [ ] Cel-shading HLSL shader
        - [ ] Advanced techniques and shaders
            - [ ] Normal mapping
            - [ ] Displacement mapping
            - [ ] God rays
            - [ ] Dynamic reflection
            - [ ] Ambient occlusion
    - [ ] Implement 2D/HUD graphics
        - [x] Basic 2D geometry
        - [ ] Font/text printing
            - [x] Basic 2D font
            - [ ] Advanced/2.5D fonts & effects
        - [ ] 2D/3D textures rendering
    - [ ] Implement menu/GUI/HUD system
        - [ ] Core GUI/buttons system
        - [ ] Basic Main Menu
            - [ ] Settings
        - [ ] Basic Pause Menu
    - [ ] Implement core asset file system
        - [ ] FBX models
            - [ ] Mesh container management/loading
            - [ ] 3D Animation
        - [ ] 2D assets
        - [ ] World/region and triggers files
        - [ ] Sound files
        - [ ] Main asset archive/compression system
    - [ ] Implement launcher/external settings editor
        - [ ] Version build management/updater
        - [ ] GUI and text editor for config.ini
        - [ ] Compatibility and requirements check
    - [X] Implement controller input
    - [ ] Implement sound system
    - [ ] Implement core AI structure
    - [ ] Implement core triggers mechanics
    - [ ] Implement core physics
    - [ ] Implement core player controller mechanics
    - [ ] Implement core world mechanics
    - [ ] Implement core save/load system
    - [ ] Implement core cutscene/FMV system
- [ ] SubSystem: 2D
- [ ] SubSystem: Tiled 2D
- [ ] SubSystem: 2.5D
- [ ] SubSystem: 3D FPS/TPS
- [ ] SubSystem: 3D RTS
- [ ] SubSystem: 2D/3D Sidescroller

### Bugs:
- [X] Fix keyboard input setting to '1' repeatedly
- [X] Fix camera interpolation
- [x] Fix high framerate usage by the debug renderer -- compiling with `release` optimization gives expected results for the moment

### Roadmap:
- [ ] Get basic gameplay engine working, test some assets against it
- [ ] Develop testing build
- [ ] Polish github/create dedicated website
- [ ] Develop prototype working game
- [ ] Sell on Steam
- [ ] Develop multiple gameplays
- [ ] Implement Steam Workshop
- [ ] ???
- [ ] Profit

### Possible future plans:
- Implement multiplatform support
- Look into Vulkan support
- Look into mobile/legacy systems
- Handshake with Jon Stewart
