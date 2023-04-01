#ifndef RESOURCE_PATH_RESOLVER_H
#define RESOURCE_PATH_RESOLVER_H

#include <string>

#ifdef __APPLE__

#include <CoreFoundation/CoreFoundation.h>

/*
credits: 
    https://stackoverflow.com/questions/28860033/convert-from-cfurlref-or-cfstringref-to-stdstring
    https://stackoverflow.com/questions/24165681/accessing-files-in-resources-folder-in-mac-osx-app-bundle
*/

std::string resolve_resource_path(std::string path) { 
    CFStringRef path_cf_string = CFStringCreateWithCString(kCFAllocatorDefault, path.data(), kCFStringEncodingUTF8);
    CFURLRef appUrlRef = CFBundleCopyResourceURL(CFBundleGetMainBundle(), path_cf_string, NULL, NULL);
    CFStringRef filePathRef = CFURLCopyPath(appUrlRef);
    CFIndex bufferSize = CFStringGetLength(filePathRef) + 1; // The +1 is for having space for the string to be NUL terminated
    char* buffer = new char[bufferSize];

    // CFStringGetCString is documented to return a false if the buffer is too small 
    // (which shouldn't happen in this example) or if the conversion generally fails    
    if (CFStringGetCString(filePathRef, buffer, bufferSize, kCFStringEncodingUTF8))
    {
        return std::string(buffer);
    }else {
        throw std::runtime_error("failed to resolve resource path");
    }

    CFRelease(path_cf_string);
    CFRelease(appUrlRef);
    CFRelease(filePathRef);
}
#else

std::string resolve_resource_path(std::string path) { 
    return path; //no conversion on Linux/Windows, this is a MacOS problem.
}

#endif

#endif