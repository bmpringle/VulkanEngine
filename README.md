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
        - Make sure to install the Vulkan SDK to your system in the installer, or to run the install_vulkan.py script at your chosen install location afterwards.
    3. Run the compileShaders.sh shell script
    4. Run scons -j8, command line options are:
        - DBG -- turns off optimizations and places the executable at ./bin/dbg/tstGame, otherwise the executable is at ./bin/rel/tstGame
        - ARM -- no-op (only compile for arm right now)
        - WARN -- turns of -Werror
        - TEST - no-op
        - VALIDATION_LAYERS - turns on Vulkan's Validation Layers
        - SANITIZE_MEM - turns on the address sanitizer
        - RELEASE - sets certain environemnt variables at runtime that need to be set in order to allow executables built with this engine to run on their own without the Vulkan SDK.
        
### Windows
___
-todo

<br/>  
<br/>
<br/>

## Packaging
___
### MacOS
___
### Place the following items in one folder:
    - the vulkan engine dynamic library (in RELEASE mode)
    - your executable
    - the Vulkan dynamic library (should be at /usr/local/lib/libvulkan.1.dylib)
    - the MoltenVK dynamic library (should be at /usr/local/lib/libMoltenVK.dylib)
    - the icd file (should be at /usr/local/share/vulkan/icd.d/MoltenVK_icd.json)
        -edit the icd file's path to point to your MoltenVK dylib
    - the assets folder
    - the shaders folder
### Next, change where your executable and libVulkanEngine look at runtime for dependencies to be the directory they are already in. An example of this is as follows:
    install_name_tool -change bin/rel/libVulkanEngineLib.dylib @rpath/libVulkanEngineLib.dylib tstGame
### This should be enough to run on a macos platform even without the SDK. If you want to use an App Bundle, then just place these resources into the proper structure. See the Apple documentation here: https://developer.apple.com/library/archive/documentation/CoreFoundation/Conceptual/CFBundles/Introduction/Introduction.html