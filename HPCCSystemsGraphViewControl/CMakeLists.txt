
# Component: npHPCCSystemsGraphViewControl
#####################################################
# Description:
# ------------
#	 Cmake Input File for npHPCCSystemsGraphViewControl
#####################################################

project( HPCCSystemsGraphViewControl )

# *********************************************************************

include_directories (
	gen
	../graphdb
	../graphrender
	${WTL_INCLUDE_DIR}/include
	${Boost_INCLUDE_DIRS}
	${AGGSRC_DIRS}/agg-2.4/include
	${FIREBREATH_INCLUDE_DIRS}
	)

if(WIN32)
elseif(APPLE)
elseif(UNIX)
include_directories (
	${GTK_INCLUDE_DIRS}
	)
endif()

# SRC *****************************************************************

set (	SRCS
	precompiled_headers.h

	Factory.cpp

	HPCCSystemsGraphViewControl.cpp
	HPCCSystemsGraphViewControl.h
	HPCCSystemsGraphViewControlAPI.cpp
	HPCCSystemsGraphViewControlAPI.h

	DotViewCommon.cpp
	DotViewCommon.h

	Version.h
	Version.cpp

	../config.h

	../npHPCCSystemsGraphViewControl.inf
	../install.rdf

	gen/FBControl.htm
#	index.htm
	)

# Win *****************************************************************

set (	SRCS_WIN
	gen/FBControl.rgs
	gen/FireBreathWin.def
	gen/firebreathWin.rc
	gen/FireBreathWin.rgs
	gen/FireBreathWin.idl

	DotViewWin.cpp
	DotViewWin.h
	)
set (	SRCS_WIN_NO_PRECOMPILED_HEADERS
	gen/FireBreathWin_i.c
	)

SOURCE_GROUP(Win FILES ${SRCS_WIN} ${SRCS_WIN_NO_PRECOMPILED_HEADERS})

# OSX *****************************************************************

set (	SRCS_APPLE
	DotViewMac.cpp
	DotViewMac.h
	)

SOURCE_GROUP(OSX FILES ${SRCS_APPLE})

# X11 *****************************************************************

set (	SRCS_UNIX
	DotViewX11.cpp
	DotViewX11.h
	)

SOURCE_GROUP(X11 FILES ${SRCS_UNIX})

# *********************************************************************

set (	SRCS_CMAKE
	../cmake/Modules/Externals.cmake
	../cmake/Modules/Utils.cmake
	../cmake/Modules/FindAGGSRC.cmake
	../cmake/Modules/FindFIREBREATH.cmake
	../cmake/Modules/FindGRAPHVIZSRC.cmake
	../cmake/Modules/FindWTL.cmake
	../README
	)

SOURCE_GROUP(CMake FILES ${SRCS_CMAKE})

# *********************************************************************

if(WIN32)
    set_source_files_properties(${SRCS_APPLE} PROPERTIES HEADER_FILE_ONLY 1)
    set_source_files_properties(${SRCS_UNIX} PROPERTIES HEADER_FILE_ONLY 1)
elseif(APPLE)
    set_source_files_properties(${SRCS_WIN} PROPERTIES HEADER_FILE_ONLY 1)
    set_source_files_properties(${SRCS_WIN_NO_PRECOMPILED_HEADERS} PROPERTIES HEADER_FILE_ONLY 1)
    set_source_files_properties(${SRCS_UNIX} PROPERTIES HEADER_FILE_ONLY 1)
elseif(UNIX)
    set_source_files_properties(${SRCS_WIN} PROPERTIES HEADER_FILE_ONLY 1)
    set_source_files_properties(${SRCS_WIN_NO_PRECOMPILED_HEADERS} PROPERTIES HEADER_FILE_ONLY 1)
    set_source_files_properties(${SRCS_APPLE} PROPERTIES HEADER_FILE_ONLY 1)
endif()

# *********************************************************************

add_definitions(-DUNICODE -D_UNICODE)

if(WIN32)
	add_definitions(-DHPCCSYSTEMSGRAPHVIEWCONTROL_EXPORTS)
elseif(APPLE)
elseif(UNIX)
endif()

set(SOURCES ${SRCS} ${SRCS_WIN} ${SRCS_APPLE} ${SRCS_UNIX} ${SRCS_CMAKE})
ADD_MSVC_PRECOMPILED_HEADER("precompiled_headers.h" "precompiled_headers.cpp" SOURCES)
set(INSOURCES ${SOURCES} ${SRCS_WIN_NO_PRECOMPILED_HEADERS})

# *********************************************************************

if ( APPLE )
	set(PLIST "Mac/bundle_template/Info.plist")
	set(STRINGS "Mac/bundle_template/InfoPlist.strings")
	set(LOCALIZED "Mac/bundle_template/Localized.r")

    message ("Creating Mac Browser Plugin project ${PROJECT_NAME}")
    if (NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/bundle)
        file (MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bundle)
        file (MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bundle/English.lproj)
    endif()

    configure_template(${PLIST} ${CMAKE_CURRENT_BINARY_DIR}/bundle/Info.plist)
    configure_template(${STRINGS} ${CMAKE_CURRENT_BINARY_DIR}/bundle/English.lproj/InfoPlist.strings)
    configure_template(${LOCALIZED} ${CMAKE_CURRENT_BINARY_DIR}/bundle/English.lproj/Localized.r)

    #set(MAC_RESOURCE_FILES ${CMAKE_CURRENT_BINARY_DIR}/bundle/English.lproj/Localized.r)

    set(SOURCES
        ${${INSOURCES}}
        ${CMAKE_CURRENT_BINARY_DIR}/bundle/Info.plist
        ${CMAKE_CURRENT_BINARY_DIR}/bundle/English.lproj/InfoPlist.strings
        ${CMAKE_CURRENT_BINARY_DIR}/bundle/English.lproj/Localized.r
        ${CMAKE_CURRENT_BINARY_DIR}/bundle/English.lproj/Localized.rsrc
    )

    add_definitions(
        -DXP_MACOSX=1
        )

    set (RCFILES ${CMAKE_CURRENT_BINARY_DIR}/bundle/English.lproj/Localized.r)

    foreach(ARCH ${CMAKE_OSX_ARCHITECTURES})
        list(APPEND ARCHS -arch ${ARCH})
    endforeach()

    # Compile the resource file
    find_program(RC_COMPILER Rez NO_DEFAULT_PATHS)
    execute_process(COMMAND
        ${RC_COMPILER} ${RCFILES} -useDF ${ARCHS} -arch x86_64 -o ${CMAKE_CURRENT_BINARY_DIR}/bundle/English.lproj/Localized.rsrc
        )

    set_source_files_properties(
        ${CMAKE_CURRENT_BINARY_DIR}/bundle/English.lproj/Localized.rsrc
        PROPERTIES GENERATED 1
        )
    message("Generating ${CMAKE_CURRENT_BINARY_DIR}/bundle/English.lproj/Localized.rsrc")

    add_library( ${PROJECT_NAME} MODULE
        ${SOURCES} 
        )

    string(REPLACE " " "\ " FB_ESC_ROOT_DIR ${FIREBREATH_SOURCE_DIR})
    set_target_properties(${PROJECT_NAME} PROPERTIES
        BUNDLE 1
        BUNDLE_EXTENSION plugin
        XCODE_ATTRIBUTE_WRAPPER_EXTENSION plugin  #sets the extension to .plugin
        XCODE_ATTRIBUTE_MACH_O_TYPE mh_bundle
        XCODE_ATTRIBUTE_INFOPLIST_FILE ${CMAKE_CURRENT_BINARY_DIR}/bundle/Info.plist
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_BINARY_DIR}/bundle/Info.plist
        LINK_FLAGS "-Wl,-exported_symbols_list,${FB_ESC_ROOT_DIR}/gen_templates/ExportList_plugin.txt")

    set_source_files_properties(
        ${CMAKE_CURRENT_BINARY_DIR}/bundle/English.lproj/InfoPlist.strings
        ${CMAKE_CURRENT_BINARY_DIR}/bundle/English.lproj/Localized.rsrc
        PROPERTIES MACOSX_PACKAGE_LOCATION "Resources/English.lproj")

    #set_source_files_properties(
    #    ${EXECUTABLE_OUTPUT_PATH}/libgraphdb.dylib
    #    ${EXECUTABLE_OUTPUT_PATH}/Debug/libgraphlayout.dylib
    #    ${EXECUTABLE_OUTPUT_PATH}/Debug/libgraphrender.dylib
    #    PROPERTIES MACOSX_PACKAGE_LOCATION "MacOS")
else ( APPLE )
	add_library(HPCCSystemsGraphViewControl SHARED ${SOURCES} ${SRCS_WIN_NO_PRECOMPILED_HEADERS})
endif ( APPLE )

# *********************************************************************

if(WIN32)
elseif(APPLE)
elseif(UNIX)
	set(NPAPI_LINK_FLAGS "-Wl,--discard-all -Wl,-Bsymbolic -Wl,-z,defs -Wl")
endif()
set_target_properties ("HPCCSystemsGraphViewControl" PROPERTIES
       OUTPUT_NAME npHPCCSystemsGraphViewControl
       PROJECT_LABEL "HPCCSystemsGraphViewControl"
       LINK_FLAGS "${NPAPI_LINK_FLAGS}"
       PREFIX ""
	)

add_subdirectory(PluginAuto)
set_property(TARGET PluginAuto PROPERTY FOLDER "3rdParty/FireBreath Generated")

#  Libraries added twice to resolve circular references on linux (when using static linking).
target_link_libraries( HPCCSystemsGraphViewControl
	graphdb
	graphrender
	${FIREBREATH_LIBRARIES}
	PluginAuto
	${FIREBREATH_LIBRARIES}
	PluginAuto
	${Boost_LIBRARIES}
	)
if(WIN32)
	target_link_libraries( HPCCSystemsGraphViewControl
		wininet
		)
elseif(APPLE)
	target_link_libraries( HPCCSystemsGraphViewControl
		${FOUNDATION_FRAMEWORK}
		${CORE_SERVICES}
		${SYS_CONFIG}
	)
elseif(UNIX)
    set(LIBDL_LIBRARIES dl)
	target_link_libraries( HPCCSystemsGraphViewControl
		${GTK_LIBRARIES}
		${LIBDL_LIBRARIES}
		${CMAKE_THREAD_LIBS_INIT}
		)
endif()

# *********************************************************************
SET(plugin_dest_dir bin)
SET(qtconf_dest_dir bin)
SET(APPS "\${CMAKE_INSTALL_PREFIX}/bin/npHPCCSystemsGraphViewControl.so")
IF(APPLE)
  SET(plugin_dest_dir npHPCCSystemsGraphViewControl.plugin/Contents/MacOS)
  SET(qtconf_dest_dir npHPCCSystemsGraphViewControl.plugin/Contents/Resources)
  SET(APPS "\${CMAKE_INSTALL_PREFIX}/npHPCCSystemsGraphViewControl.plugin")
ENDIF(APPLE)
IF(WIN32)
  SET(APPS "\${CMAKE_INSTALL_PREFIX}/bin/npHPCCSystemsGraphViewControl.dll")
ENDIF(WIN32)

# *********************************************************************

INSTALL(TARGETS HPCCSystemsGraphViewControl 
	BUNDLE DESTINATION . 
	RUNTIME DESTINATION bin 
	LIBRARY DESTINATION lib
)

# *********************************************************************
SET(DIRS "/opt/local/lib")

#INSTALL(CODE "
    #file(GLOB_RECURSE QTPLUGINS \"\${CMAKE_INSTALL_PREFIX}/${plugin_dest_dir}/plugins/*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
 #   include(BundleUtilities)
  #  message(\"QTPLUGINS:  ${QTPLUGINS}\")
    #fixup_bundle(\"${APPS}\" \"\${QTPLUGINS}\" \"${DIRS}\")
   # " COMPONENT Runtime)

# *********************************************************************
