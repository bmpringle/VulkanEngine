#!/usr/bin/env bash

#can't take inputs in $2 or $3 with a | in them, since that's the sed delimiter
function replace() { 
    sed -i '' 's|'$2'|'$3'|' $1
}

APP_NAME="${APP_NAME:-TEST_APP}"
EXECUTABLE_FILE_PATH="${EXECUTABLE_FILE_PATH:-./bin/rel/tstGame}"
EXECUTABLE_NAME="${EXECUTABLE_NAME:-tstGame}"
DEPLOY_FOLDER="${DEPLOY_FOLDER:-./deploy}"

ICD_FILE_PATH=$DEPLOY_FOLDER/$APP_NAME.app/Contents/Resources/vulkan/icd.d/MoltenVK_icd.json
PLIST_FILE_PATH=$DEPLOY_FOLDER/$APP_NAME.app/Contents/Info.plist

#construct app bundle

mkdir -p $DEPLOY_FOLDER
rm -rf $DEPLOY_FOLDER/$APP_NAME.app
mkdir -p $DEPLOY_FOLDER/$APP_NAME.app
mkdir -p $DEPLOY_FOLDER/$APP_NAME.app/Contents
mkdir -p $DEPLOY_FOLDER/$APP_NAME.app/Contents/MacOS
mkdir -p $DEPLOY_FOLDER/$APP_NAME.app/Contents/Resources
mkdir -p $DEPLOY_FOLDER/$APP_NAME.app/Contents/Resources/vulkan
mkdir -p $DEPLOY_FOLDER/$APP_NAME.app/Contents/Frameworks

cp artifacts/Info.plist $DEPLOY_FOLDER/$APP_NAME.app/Contents/Info.plist
cp artifacts/vk_engine.icns $DEPLOY_FOLDER/$APP_NAME.app/Contents/Resources/vk_engine.icns
cp -r assets/ $DEPLOY_FOLDER/$APP_NAME.app/Contents/Resources/assets
cp -r shaders/ $DEPLOY_FOLDER/$APP_NAME.app/Contents/Resources/shaders
cp -r VulkanSDKMacOS/macOS/share/vulkan/icd.d $DEPLOY_FOLDER/$APP_NAME.app/Contents/Resources/vulkan/icd.d
cp $EXECUTABLE_FILE_PATH $DEPLOY_FOLDER/$APP_NAME.app/Contents/MacOS/$EXECUTABLE_NAME
cp VulkanSDKMacOS/macOS/lib/libMoltenVK.dylib $DEPLOY_FOLDER/$APP_NAME.app/Contents/Frameworks/libMoltenVK.dylib
cp VulkanSDKMacOS/macOS/lib/libvulkan.1.3.243.dylib $DEPLOY_FOLDER/$APP_NAME.app/Contents/Frameworks/libvulkan.1.dylib
cp bin/rel/libVulkanEngineLib.dylib $DEPLOY_FOLDER/$APP_NAME.app/Contents/Frameworks/libVulkanEngineLib.dylib

#change rpath and dylib locations
install_name_tool -add_rpath @loader_path/../Frameworks $DEPLOY_FOLDER/$APP_NAME.app/Contents/MacOS/$EXECUTABLE_NAME
install_name_tool -add_rpath @loader_path/../Frameworks $DEPLOY_FOLDER/$APP_NAME.app/Contents/Frameworks/libVulkanEngineLib.dylib
install_name_tool -id libVulkanEngineLib.dylib $DEPLOY_FOLDER/$APP_NAME.app/Contents/Frameworks/libVulkanEngineLib.dylib
install_name_tool -change bin/rel/libVulkanEngineLib.dylib @rpath/libVulkanEngineLib.dylib $DEPLOY_FOLDER/$APP_NAME.app/Contents/MacOS/$EXECUTABLE_NAME

replace $PLIST_FILE_PATH INSERT_EXECUTABLE_NAME $EXECUTABLE_NAME
replace $PLIST_FILE_PATH INSERT_DISPLAY_NAME $APP_NAME
replace $PLIST_FILE_PATH INSERT_BUNDLE_NAME $APP_NAME
replace $ICD_FILE_PATH "../../../lib/libMoltenVK.dylib" "../../../Frameworks/libMoltenVK.dylib"