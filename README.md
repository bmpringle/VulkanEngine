# Vulkan Engine
## Build
#### Dependencies (the ones that need to be manually installed):
    python@3.X
    scons-compiledb 
        - installed using pip, which should be installed automatically with python
        - pip install scons-compiledb should install it
        - on some devices python is python3 and pip is pip3
    wget
    cmake
    make
    git
___
### MacOS and Linux
___
    
#### Build Steps:
    1. Clone this repository
    2. Run the python script compileDeps.py in the project root directory
        - Some systems may need to add the following link to their system to get some dependencies to compile:
            ln -s /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX11.3.sdk
    3. Run the compileShaders.sh shell script
    4. Run scons -j8, command line options are:
        - DBG -- turns off optimizations and places the executable at ./bin/dbg/tstGame, otherwise the executable is at ./bin/rel/tstGame
        - ARM -- no-op (only compile for arm right now)
        - WARN -- turns of -Werror
        - TEST - no-op
        - VALIDATION_LAYERS - turns on Vulkan's Validation Layers
        - SANITIZE_MEM - turns on the address sanitizer
    5. Before running any code built with this engine, run the setupEnvironment script to in order to properly set up ENV variables
        
### Windows
___
-todo