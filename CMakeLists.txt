#https://cmake.org/cmake/help/latest/manual/cmake-toolchains.7.html#cross-compiling-for-ios-tvos-or-watchos
#rm ./* && cmake .. -G "Xcode" -DCMAKE_SYSTEM_NAME=iOS && open 3d4.xcodeproj/
#
cmake_minimum_required(VERSION 3.17)
PROJECT(fs C CXX)
list(APPEND CMAKE_MODULE_PATH
        "${CMAKE_SOURCE_DIR}/cmake/macros")
include(AutoCollect)
set(CMAKE_CXX_STANDARD 20)

set(USE_GCC ON)
set(SDL 0)
set(GLFW 0)
set(IOS 0)
set(ASSIMP 0)
set(IMGUI 0)
set(GLAD 0)
IF (IOS)
    set(SDL 0)
    set(GLFW 0)

    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -DIOS")
ENDIF ()
add_definitions(-D_AFXDLL)
set(CMAKE_MFC_FLAG 1)

#libeay32
set(LIBS ${LIBS} crypt32 libssh2 ws2_32 advapi32 shell32 user32 gdi32 winmm)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/general/tomlplusplus-2.4.0/include/)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/general/doctest-2.4.6/doctest/)
#add_library(doctest "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/general/doctest-2.4.6/doctest/parts/doctest.cpp")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/general/doctest-2.4.6/)
set(LIBS ${LIBS} doctest::doctest)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/general/GameNetworkingSockets-1.3.0/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/general/GameNetworkingSockets-1.3.0/include/)
set(LIBS ${LIBS} GameNetworkingSockets_s)
#set(WITH_TESTSUITE OFF CACHE BOOL "" FORCE) # disable what you don't use
#
#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/general/corrade/src/Corrade/configure.h.cmake
#        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/general/corrade/src/Corrade/configure.h.in)
#file(GENERATE OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/general/corrade/src/Corrade/configure.h
#        INPUT ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/general/corrade/src/Corrade/configure.h.in)
#
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/general/corrade EXCLUDE_FROM_ALL) # so only things you use are built
#set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/general/corrade/modules" ${CMAKE_MODULE_PATH})
#INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/general/corrade/src/)
#find_package(Corrade REQUIRED Utility) # see below
#
#set(LIBS ${LIBS} Corrade::Utility)

if(VULKAN)
    find_package(Vulkan)
    if (DEFINED ENV{VULKAN_SDK})
        message(STATUS "Vulkan environment variable: $ENV{VULKAN_SDK}")
        set(VULKAN_INCLUDE "$ENV{VULKAN_SDK}/include")
    else ()
        message(STATUS "Vulkan environment variable: undefined")
        set(VULKAN_INCLUDE "")
    endif ()
endif()


IF(ASSIMP)
    set(ASSIMP_STATIC 0)
    IF (0)
        if (WIN32 AND MINGW)
            set(ASSIMP_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/mingw64/assimp)
        ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Linux")
            set(ASSIMP_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/linux/assimp)
        ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Darwin")
            set(ASSIMP_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/bsd/assimp)
        endif ()

        set(ASSIMP_LIBRARY "assimp" "IrrXML" "z")

        #set(ASSIMP_LIBRARY "assimp" "IrrXML" "zlibstatic")
        set(ASSIMP_LIBRARY_DIRS ${ASSIMP_PREFIX}/lib)
        set(ASSIMP_INCLUDE_DIRS ${ASSIMP_PREFIX}/include)
        link_directories(${ASSIMP_LIBRARY_DIRS})
        INCLUDE_DIRECTORIES(${ASSIMP_INCLUDE_DIRS})
        set(LIBS ${LIBS} ${ASSIMP_LIBRARY})


    ELSEIF (NOT IOS)
        #https://github.com/assimp/assimp/blob/master/Build.md
        #BUILD_SHARED_LIBS ( default ON ): Generation of shared libs ( dll for windows, so for Linux ). Set this to OFF to get a static lib.
        #BUILD_FRAMEWORK ( default OFF, MacOnly): Build package as Mac OS X Framework bundle
        #ASSIMP_DOUBLE_PRECISION( default OFF ): All data will be stored as double values.
        #ASSIMP_OPT_BUILD_PACKAGES ( default OFF): Set to ON to generate CPack configuration files and packaging targets
        #ASSIMP_ANDROID_JNIIOSYSTEM ( default OFF ): Android JNI IOSystem support is active
        #ASSIMP_NO_EXPORT ( default OFF ): Disable Assimp's export functionality
        #ASSIMP_BUILD_ZLIB ( default OFF ): Build your own zlib
        #ASSIMP_BUILD_ASSIMP_TOOLS ( default ON ): If the supplementary tools for Assimp are built in addition to the library.
        #ASSIMP_BUILD_SAMPLES ( default OFF ): If the official samples are built as well (needs Glut).
        #ASSIMP_BUILD_TESTS ( default ON ): If the test suite for Assimp is built in addition to the library.
        #ASSIMP_COVERALLS ( default OFF ): Enable this to measure test coverage.
        #ASSIMP_ERROR_MAX( default OFF): Enable all warnings.
        #ASSIMP_WERROR( default OFF ): Treat warnings as errors.
        #ASSIMP_ASAN ( default OFF ): Enable AddressSanitizer.
        #ASSIMP_UBSAN ( default OFF ): Enable Undefined Behavior sanitizer.
        #SYSTEM_IRRXML ( default OFF ): Use system installed Irrlicht/IrrXML library.
        #BUILD_DOCS ( default OFF ): Build documentation using Doxygen.
        #INJECT_DEBUG_POSTFIX( default ON ): Inject debug postfix in .a/.so lib names
        #IGNORE_GIT_HASH ( default OFF ): Don't call git to get the hash.
        #ASSIMP_INSTALL_PDB ( default ON ): Install MSVC debug files.

        #    option(ASSIMP_NO_EXPORT OFF)
        option(BUILD_SHARED_LIBS OFF)
        option(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
        option(ASSIMP_BUILD_SAMPLES OFF)
        option(ASSIMP_BUILD_TESTS OFF)
        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/general/assimp-5.0.1)
        include_directories(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/general/assimp-5.0.1/include)
        set(LIBS ${LIBS} assimp::assimp)
    ENDIF ()
ENDIF()

set(LIBS ${LIBS} uv)
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/general/enet-1.3.16)
#set(LIBS ${LIBS} enet)


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/general/glm2)

# cd build
# cmake .. -G "MinGW Makefiles" -DCMAKE_SH="CMAKE_SH-NOTFOUND" -DCMAKE_CXX_FLAGS="-static"
PickIncludeDirectories(
        ${CMAKE_CURRENT_SOURCE_DIR}/code
        INCLUDEDIRS
        # Exclude
        ${CMAKE_CURRENT_SOURCE_DIR}/build
        ${CMAKE_CURRENT_SOURCE_DIR}/code/GameLogic
        ${CMAKE_CURRENT_SOURCE_DIR}/code/util
        ${CMAKE_CURRENT_SOURCE_DIR}/code/Graphics
)

PickSourceFiles(
        ${CMAKE_CURRENT_SOURCE_DIR}/code
        CODEFILES
        # Exclude
        ${CMAKE_CURRENT_SOURCE_DIR}/build
        ${CMAKE_CURRENT_SOURCE_DIR}/code/GameLogic
        ${CMAKE_CURRENT_SOURCE_DIR}/code/util
        ${CMAKE_CURRENT_SOURCE_DIR}/code/Graphics
)


MakeFilter(
        ${CMAKE_CURRENT_SOURCE_DIR}/code)


#add_library(STB_IMAGE "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/general/stb/stb_image.cpp")

include_directories(${CMAKE_PROJECT_NAME}
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/general/stb/include/)
#set(LIBS ${LIBS} STB_IMAGE)


IF (IOS)
ELSEIF (GLAD)
    set(CODEFILES ${CODEFILES} ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/general/glad/glad.c)
    set(INCLUDEDIRS ${INCLUDEDIRS} ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/general/glad/include/)
ENDIF ()

IF (IMGUI)
    IF (GLFW)
        include_directories(${CMAKE_PROJECT_NAME}
                PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/general/imgui/
                PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/general/imgui/backends/)
        set(CODEFILES ${CODEFILES}
                ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/general/imgui/backends/imgui_impl_opengl3.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/general/imgui/backends/imgui_impl_glfw.cpp
                )
        set(CODEFILES ${CODEFILES}
                ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/general/imgui/imgui.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/general/imgui/imgui_draw.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/general/imgui/imgui_demo.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/general/imgui/imgui_widgets.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/general/imgui/imgui_demo.cpp
                )
    ELSEIF(SDL)
        include_directories(${CMAKE_PROJECT_NAME}
                PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/general/imgui-1.78/
                PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/general/imgui-1.78/examples/)
        set(CODEFILES ${CODEFILES}
                ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/general/imgui-1.78/examples/imgui_impl_opengl3.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/general/imgui-1.78/examples/imgui_impl_sdl.cpp
                )
        set(CODEFILES ${CODEFILES}
                ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/general/imgui-1.78/imgui.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/general/imgui-1.78/imgui_draw.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/general/imgui-1.78/imgui_demo.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/general/imgui-1.78/imgui_widgets.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/general/imgui-1.78/imgui_demo.cpp
                )

        #        include_directories(${CMAKE_PROJECT_NAME}
        #                PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/general/imgui/
        #                PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/general/imgui/backends/)
        #        set(CODEFILES ${CODEFILES}
        #                ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/general/imgui/backends/imgui_impl_opengl3.cpp
        #                ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/general/imgui/backends/imgui_impl_sdl.cpp
        #                )
        #        set(CODEFILES ${CODEFILES}
        #                ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/general/imgui/imgui.cpp
        #                ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/general/imgui/imgui_draw.cpp
        #                ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/general/imgui/imgui_demo.cpp
        #                ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/general/imgui/imgui_widgets.cpp
        #                ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/general/imgui/imgui_demo.cpp
        #                )
    ENDIF ()
ENDIF ()

set(RES2 )
if (IOS)
    set(RES2 ${RES2} ${CMAKE_CURRENT_SOURCE_DIR}/data.0)
endif()


add_executable(${CMAKE_PROJECT_NAME}
        ${CODEFILES} ${RES2})

target_include_directories(${CMAKE_PROJECT_NAME}
        PUBLIC ${INCLUDEDIRS})

#HeadOnly Library
IF(RAPIDJSON)
    target_include_directories(${CMAKE_PROJECT_NAME}
            PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/general/rapidjson/)
ENDIF()

target_include_directories(${CMAKE_PROJECT_NAME}
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/general/entt-3.4.0/src/)

IF(PROTOBUF)
    IF (CMAKE_SYSTEM_NAME MATCHES "Windows")
        target_include_directories(${CMAKE_PROJECT_NAME}
                PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/win64/protobuf/include/)

    ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Linux")

        target_include_directories(${CMAKE_PROJECT_NAME}
                PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/win64/protobuf/include/)

    ELSE ()

        target_include_directories(${CMAKE_PROJECT_NAME}
                PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/win64/protobuf/include/)

    ENDIF ()
ENDIF()

if (NOT EMSCRIPTEN)
    if (SDL)
        #add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/general/cdl/source")
        #    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/general/sdl")
        #    set (SDL2_DIR "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/general/sdl")
        #    link_directories(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/lib)
        #PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
        #    find_package(SDL2 CONFIG REQUIRED)
        #    target_link_libraries(${target_name} SDL2::SDL2main SDL2::SDL2-static)
        IF (WIN32 AND MINGW)
            #include(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/mingw64/sdl/lib/cmake/SDL2/sdl2-config.cmake)
            set(SDL2_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/mingw64/sdl)
            set(SDL2_LIBDIR "${SDL2_PREFIX}/lib")
            set(SDL2_LIBRARIES "-L${SDL2_LIBDIR}  -lmingw32 -lSDL2main-static -lSDL2-static -lm -luser32 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lversion -luuid -ladvapi32 -lsetupapi -lshell32 -ldinput8 -ldxerr8 -lmingw32 -Wl,--no-undefined -mwindows")
            set(SDL2_INCLUDE_DIRS "${SDL2_PREFIX}/include/SDL2")
            include_directories(${CMAKE_PROJECT_NAME}
                    PUBLIC ${SDL2_INCLUDE_DIRS})
            set(LIBS ${LIBS} ${SDL2_LIBRARIES})
        ELSEIF (WIN32)

        ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Linux")

            target_include_directories(${CMAKE_PROJECT_NAME}
                    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/mingw64/sdl/include/)

        ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Darwin")
            set(SDL2_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/bsd/sdl)
            set(SDL2_LIBDIR "${SDL2_PREFIX}/lib")
            set(SDL2_LIBRARIES "-L${SDL2_LIBDIR}")
            set(SDL2_INCLUDE_DIRS "${SDL2_PREFIX}/include/SDL2")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L${SDL2_LIBDIR} -Wl,-search_paths_first  -Wno-unused-command-line-argument -lm -liconv -Wl,-framework,CoreAudio -Wl,-framework,AudioToolbox -Wl,-framework,ForceFeedback -lobjc -Wl,-framework,CoreVideo -Wl,-framework,Cocoa -Wl,-framework,Carbon -Wl,-framework,IOKit -Wl,-weak_framework,QuartzCore -Wl,-weak_framework,Metal")
            target_include_directories(${CMAKE_PROJECT_NAME}
                    PUBLIC ${SDL2_INCLUDE_DIRS})
            set(LIBS ${LIBS} ${SDL2_LIBRARIES} libSDL2.a)
            message(${SDL2_INCLUDE_DIRS})
        ELSEIF (IOS)
            set(SDL2_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/ios/sdl)
            set(SDL2_LIBDIR "${SDL2_PREFIX}/lib")
            set(SDL2_LIBRARIES "-L${SDL2_LIBDIR}")
            set(SDL2_INCLUDE_DIRS "${SDL2_PREFIX}/include/SDL2")
            message("${SDL2_INCLUDE_DIRS}")
            message(${SDL2_INCLUDE_DIRS})
            find_library(UIKIT UIKit)
            target_link_libraries(${CMAKE_PROJECT_NAME} "-framework CoreFoundation")
            #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L${SDL2_LIBDIR} -Wl,-search_paths_first  -Wno-unused-command-line-argument -lm -liconv
            #        -lobjc
            #        -Wl,-framework,AudioToolbox
            #        -Wl,-framework,AVFoundation
            #        -Wl,-framework,CoreAudio
            #        -Wl,-framework,CoreBluetooth
            #        -Wl,-framework,CoreGraphics
            #        -Wl,-framework,CoreMotion
            #        -Wl,-framework,Foundation
            #        -Wl,-framework,GameController
            #        -Wl,-framework,Metal
            #        -Wl,-framework,OpenGLES
            #        -Wl,-framework,QuartzCore
            #        -Wl,-framework,UIKit
            #
            #        -Wl,-weak_framework,QuartzCore -Wl,-weak_framework,Metal")
            include_directories(${CMAKE_PROJECT_NAME}
                    PUBLIC ${SDL2_INCLUDE_DIRS})
            set(LIBS ${LIBS} ${SDL2_LIBRARIES} libSDL2.a)
        ENDIF ()
    elseif (GLFW)
        option(USE_MSVC_RUNTIME_LIBRARY_DLL OFF)
        option(BUILD_SHARED_LIBS OFF)
        option(GLFW_BUILD_DOCS OFF)
        option(GLFW_BUILD_EXAMPLES OFF)
        option(GLFW_BUILD_TESTS OFF)
        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/general/glfw-3.3)
        include_directories(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/general/glfw-3.3/include/
                %VULKAN_SDK%/include)
        set(LIBS ${LIBS} glfw)
    else()
    endif ()
ENDIF()

IF(FMT)
    IF (CMAKE_SYSTEM_NAME MATCHES "Windows")
        target_include_directories(${CMAKE_PROJECT_NAME}
                PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/win64/fmt/include/)

    ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Linux")

        target_include_directories(${CMAKE_PROJECT_NAME}
                PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/win64/fmt/include/)

    ELSE ()
        target_include_directories(${CMAKE_PROJECT_NAME}
                PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/win64/fmt/include/)

    ENDIF ()
ENDIF()

IF (CMAKE_SYSTEM_NAME MATCHES "Windows") #target_link_libraries(MyExec PUBLIC "-static")
    IF (USE_GCC)
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
        target_link_libraries(${CMAKE_PROJECT_NAME}
                ${LIBS})
    ELSE ()

        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
        target_link_libraries(${CMAKE_PROJECT_NAME}
                ${LIBS})
    ENDIF ()
ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
    target_link_libraries(${CMAKE_PROJECT_NAME} pthread
            )
ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Darwin")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
    target_link_libraries(${CMAKE_PROJECT_NAME}
            pthread
            ${LIBS}
            )
    MESSAGE("MACOS")
ELSEIF (IOS)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -DGLES_SILENCE_DEPRECATION)
    target_link_libraries(${CMAKE_PROJECT_NAME}
            pthread
            ${LIBS}
            )
    #    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/ios")
    #    add_executable(${CMAKE_PROJECT_NAME} MACOSX_BUNDLE main.cpp)
    #        -Wl,-framework,AudioToolbox
    #        -Wl,-framework,AVFoundation
    #        -Wl,-framework,CoreAudio
    #        -Wl,-framework,CoreBluetooth
    #        -Wl,-framework,CoreGraphics
    #        -Wl,-framework,CoreMotion
    #        -Wl,-framework,Foundation
    #        -Wl,-framework,GameController
    #        -Wl,-framework,Metal
    #        -Wl,-framework,OpenGLES
    #        -Wl,-framework,QuartzCore
    #        -Wl,-framework,UIKit
    set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
            XCODE_ATTRIBUTE_OTHER_LDFLAGS "${XCODE_ATTRIBUTE_OTHER_LDFLAGS}
            -framework AudioToolbox
-framework AudioToolbox
-framework AVFoundation
-framework CoreAudio
-framework CoreBluetooth
-framework CoreGraphics
-framework CoreMotion
-framework Foundation
-framework CoreFoundation
-framework GameController
-framework Metal
-framework OpenGLES
-framework QuartzCore
-framework UIKit

")

    #set(bundle_resources ${JUCER_ICON_FILE})
    #if(IOS)
    #list(APPEND RES
    #${JUCER_XCASSETS} ${JUCER_LAUNCH_STORYBOARD_FILE}
    #)
    #endif()

    #set_source_files_properties(${RES} PROPERTIES
    #MACOSX_PACKAGE_LOCATION "Resources"
    #)
    #source_group("Resources" FILES ${RES})

    set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
            MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/cmake/ios/MacOSXBundleInfo.plist.in
            MACOSX_BUNDLE ON
            MACOSX_BUNDLE_BUNDLE_NAME ${CMAKE_PROJECT_NAME}
            MACOSX_BUNDLE_GUI_IDENTIFIER com.myname.${CMAKE_PROJECT_NAME}
            MACOSX_FRAMEWORK_IDENTIFIER com.myname.${CMAKE_PROJECT_NAME}
            MACOSX_BUNDLE_BUNDLE_IDENTIFIER com.myname.${CMAKE_PROJECT_NAME}
            MACOSX_BUNDLE_SHORT_VERSION_STRING "123"
            MACOSX_BUNDLE_LONG_VERSION_STRING "456"
            MACOSX_BUNDLE_BUNDLE_VERSION "123"
            RESOURCE "${RES2}"
            #MACOSX_BUNDLE_ICON_FILE glfw.icns
            )
    MESSAGE("IOS")
ENDIF ()


if (EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -DIMGUI_IMPL_OPENGL_ES3")
    set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES LINK_FLAGS
            "-s ASYNCIFY=1 -s USE_SDL=2  -s MAX_WEBGL_VERSION=2 -s ALLOW_MEMORY_GROWTH=1 --preload-file data.0 -O3 --closure 1 ")
    # -s ERROR_ON_UNDEFINED_SYMBOLS=0  -s USE_GLFW=3 -s USE_SDL=2 -s FULL_ES3=1 -s FULL_ES2=1 -s USE_WEBGL2=1  --preload-file data.0 --shell-file shell_minimal.html -s MIN_WEBGL_VERSION=2 -s MAX_WEBGL_VERSION=2
endif()