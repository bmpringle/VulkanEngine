#!/usr/bin/env bash

#can't take inputs in $2 or $3 with a | in them, since that's the sed delimiter
function replace() { 
    sed -i '' 's|'$2'|'$3'|' $1
}

APP_NAME=TEST_APP
EXECUTABLE_NAME=tstGame
ICD_FILE_PATH=deploy/$APP_NAME.app/Contents/Resources/vulkan/icd.d/MoltenVK_icd.json
PLIST_FILE_PATH=deploy/$APP_NAME.app/Contents/Info.plist

#construct app bundle

mkdir -p deploy
rm -rf deploy/$APP_NAME.app
mkdir -p deploy/$APP_NAME.app
mkdir -p deploy/$APP_NAME.app/Contents
mkdir -p deploy/$APP_NAME.app/Contents/MacOS
mkdir -p deploy/$APP_NAME.app/Contents/Resources
mkdir -p deploy/$APP_NAME.app/Contents/Resources/vulkan
mkdir -p deploy/$APP_NAME.app/Contents/Frameworks

cp artifacts/Info.plist deploy/$APP_NAME.app/Contents/Info.plist
cp artifacts/vk_engine.icns deploy/$APP_NAME.app/Contents/Resources/vk_engine.icns
cp -r assets/ deploy/$APP_NAME.app/Contents/Resources/assets
cp -r shaders/ deploy/$APP_NAME.app/Contents/Resources/shaders
cp -r VulkanSDKMacOS/macOS/share/vulkan/icd.d deploy/$APP_NAME.app/Contents/Resources/vulkan/icd.d
cp bin/rel/$EXECUTABLE_NAME deploy/$APP_NAME.app/Contents/MacOS/$EXECUTABLE_NAME
cp VulkanSDKMacOS/macOS/lib/libMoltenVK.dylib deploy/$APP_NAME.app/Contents/Frameworks/libMoltenVK.dylib
cp VulkanSDKMacOS/macOS/lib/libvulkan.1.3.243.dylib deploy/$APP_NAME.app/Contents/Frameworks/libvulkan.1.dylib
cp bin/rel/libVulkanEngineLib.dylib deploy/$APP_NAME.app/Contents/Frameworks/libVulkanEngineLib.dylib

#change rpath and dylib locations
install_name_tool -add_rpath @loader_path/../Frameworks deploy/$APP_NAME.app/Contents/MacOS/$EXECUTABLE_NAME
install_name_tool -add_rpath @loader_path/../Frameworks deploy/$APP_NAME.app/Contents/Frameworks/libVulkanEngineLib.dylib
install_name_tool -id libVulkanEngineLib.dylib deploy/$APP_NAME.app/Contents/Frameworks/libVulkanEngineLib.dylib
install_name_tool -change bin/rel/libVulkanEngineLib.dylib @rpath/libVulkanEngineLib.dylib deploy/$APP_NAME.app/Contents/MacOS/$EXECUTABLE_NAME

replace $PLIST_FILE_PATH INSERT_EXECUTABLE_NAME $EXECUTABLE_NAME
replace $PLIST_FILE_PATH INSERT_DISPLAY_NAME $APP_NAME
replace $PLIST_FILE_PATH INSERT_BUNDLE_NAME $APP_NAME
replace $ICD_FILE_PATH "../../../lib/libMoltenVK.dylib" "../../../Frameworks/libMoltenVK.dylib"