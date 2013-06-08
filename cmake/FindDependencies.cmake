if(AXR_BUILD_CORE)
    ### Qt ###
    find_package(Qt4 4.8 COMPONENTS QtCore QtGui REQUIRED)
    find_package(Qt4 4.8 COMPONENTS QtMacExtras)
    find_package(Qt4 4.8 COMPONENTS QtWinExtras)
    find_package(Qt4 4.8 COMPONENTS QtScript)
    include(${QT_USE_FILE})
    add_definitions(${QT_DEFINITIONS} -DNOMINMAX)

    ### DPKG ###
    find_program(DPKG NAMES "dpkg-deb" PATHS "/usr/bin")
    if(DPKG)
        get_filename_component(DPKG_PATH ${DPKG} ABSOLUTE)
        message(STATUS "Found dpkg-deb: ${DPKG_PATH}")
        set(DPKG_FOUND TRUE)
    else()
        message(STATUS "Could not find dpkg-deb; will not build Debian packages")
        set(DPKG_FOUND FALSE)
    endif()

    ### RPMBUILD ###
    find_program(RPMBUILD NAMES "rpmbuild" PATHS "/usr/bin")
    if(RPMBUILD)
        get_filename_component(RPMBUILD_PATH ${RPMBUILD} ABSOLUTE)
        message(STATUS "Found rpmbuild: ${RPMBUILD_PATH}")
        set(RPMBUILD_FOUND TRUE)
    else()
        message(STATUS "Could not find rpmbuild; will not build RPM packages")
        set(RPMBUILD_FOUND FALSE)
    endif()

    ### pandoc ###
    find_program(PANDOC NAMES "pandoc.exe" "pandoc" PATHS "C:\\Program Files (x86)\\Pandoc\\bin" "/usr/local/bin" "/usr/bin")
    if(NOT PANDOC)
        message(FATAL_ERROR "Could not find pandoc; this is required for building packages")
    endif()
endif()

if(AXR_BUILD_DOCS)
    find_package(Doxygen REQUIRED)
endif()
