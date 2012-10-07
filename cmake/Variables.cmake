set(AXR_PACKAGE_NAME "AXR")
set(AXR_PACKAGE_PREFIX "libaxr")

set(AXR_VERSION_MAJOR 0)
set(AXR_VERSION_MINOR 4)
set(AXR_VERSION_PATCH 7)
set(AXR_VERSION_BUILD 0)
set(AXR_VERSION_STRING "${AXR_VERSION_MAJOR}.${AXR_VERSION_MINOR}")

if(AXR_VERSION_BUILD GREATER 0)
    set(AXR_VERSION_STRING "${AXR_VERSION_STRING}.${AXR_VERSION_PATCH}.${AXR_VERSION_BUILD}")
elseif(AXR_VERSION_PATCH GREATER 0)
    set(AXR_VERSION_STRING "${AXR_VERSION_STRING}.${AXR_VERSION_PATCH}")
endif()

git_append_shorttag(AXR_VERSION_STRING)

set(AXR_VENDOR "AXR Project Contributors")
set(AXR_DOMAIN "axr.vg")
set(AXR_CONTACT "info@axr.vg")

set(AXR_README_FILE "${CMAKE_SOURCE_DIR}/README.md")
set(AXR_LICENSE_FILE "${CMAKE_SOURCE_DIR}/COPYING")

set(AXR_README_FILE_TXT "${CMAKE_BINARY_DIR}/README.txt")
set(AXR_LICENSE_FILE_TXT "${CMAKE_BINARY_DIR}/COPYING.txt")

if(WIN32)
    set(AXR_PACKAGE_ICON "${CMAKE_SOURCE_DIR}\\\\share\\\\icons\\\\prototype.ico")
elseif(APPLE)
    set(AXR_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/share/icons/prototype.icns")
else()
    set(AXR_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/share/icons/prototype.png")
endif()
