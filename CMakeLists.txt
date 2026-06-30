cmake_minimum_required(VERSION 3.12)
project(Universe)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)



# ---- SFML ----
if(CMAKE_CROSSCOMPILING)
    set(SFML_DIR "${CMAKE_SOURCE_DIR}/Third_party/SFML/lib/cmake/SFML")
endif()
find_package(SFML 3 COMPONENTS Audio Graphics Network System Window REQUIRED)

# ---- LUA ----
set(LUA_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/Third_party/LUA/include")
if(CMAKE_CROSSCOMPILING OR WIN32)
    
    set(LUA_WIN_DIR "${CMAKE_SOURCE_DIR}/Third_party/LUA/lib/windows")
    
    if(EXISTS "${LUA_WIN_DIR}/liblua.dll.a")
        set(LUA_LIBRARY "${LUA_WIN_DIR}/liblua.dll.a")
    elseif(EXISTS "${LUA_WIN_DIR}/liblua_static.a")
        set(LUA_LIBRARY "${LUA_WIN_DIR}/liblua_static.a")
    else()
        message(FATAL_ERROR "Nie znaleziono pliku biblioteki (.dll.a lub .a) Lua w Third_party/LUA/lib/windows!")
    endif()
    
    message(STATUS "Lua (Windows): ${LUA_LIBRARY}")
else()
    find_package(Lua 5.4 REQUIRED)
    message(STATUS "Lua (Linux): ${LUA_LIBRARIES}")
endif()

# ---- ENET ----
if(CMAKE_CROSSCOMPILING OR WIN32)
    set(ENET_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/Third_party/ENET/include")
    set(ENET_LIBRARY "${CMAKE_SOURCE_DIR}/Third_party/ENET/lib/libenet.a")
    message(STATUS "ENet (Windows): ${ENET_LIBRARY}")
else()
    find_library(ENET_LIBRARY enet)
    find_path(ENET_INCLUDE_DIR enet/enet.h)
    message(STATUS "ENet (Linux): ${ENET_LIBRARY}")
endif()

# ---- BOX2D ----
if(CMAKE_CROSSCOMPILING OR WIN32)
    set(BOX2D_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/Third_party/BOX2D/include")
    set(BOX2D_LIB_DIR "${CMAKE_SOURCE_DIR}/Third_party/BOX2D/lib")

    if(EXISTS "${BOX2D_LIB_DIR}/libbox2d.a")
        set(BOX2D_LIBRARY "${BOX2D_LIB_DIR}/libbox2d.a")
    elseif(EXISTS "${BOX2D_LIB_DIR}/libbox2d.dll.a")
        set(BOX2D_LIBRARY "${BOX2D_LIB_DIR}/libbox2d.dll.a")
    elseif(EXISTS "${BOX2D_LIB_DIR}/box2d.lib")
        set(BOX2D_LIBRARY "${BOX2D_LIB_DIR}/box2d.lib")
    else()
        message(FATAL_ERROR "Nie znaleziono biblioteki Box2D w Third_party/BOX2D/lib!")
    endif()

    message(STATUS "Box2D (Windows): ${BOX2D_LIBRARY}")
else()
    find_library(BOX2D_LIBRARY box2d)
    find_path(BOX2D_INCLUDE_DIR box2d/box2d.h)

    if(NOT BOX2D_LIBRARY OR NOT BOX2D_INCLUDE_DIR)
        message(FATAL_ERROR "Nie znaleziono Box2D zainstalowanego systemowo na Linuksie!")
    endif()

    message(STATUS "Box2D (Linux): ${BOX2D_LIBRARY}")
endif()


file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "Src/*.cpp" "Src/*.h" "Src/*.hpp")

add_executable(Universe ${SOURCES})

set_target_properties(Universe PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Game"
    OUTPUT_NAME "Universe"
)

set_target_properties(Universe PROPERTIES WIN32_EXECUTABLE FALSE)

target_include_directories(Universe PRIVATE
    ${LUA_INCLUDE_DIR}
    ${ENET_INCLUDE_DIR}
    ${BOX2D_INCLUDE_DIR}
)


# ---- SFML ----
target_link_libraries(Universe PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
    SFML::Audio
)

# ---- LUA ----
if(CMAKE_CROSSCOMPILING OR WIN32)
    target_link_libraries(Universe PRIVATE ${LUA_LIBRARY})
else()
    target_link_libraries(Universe PRIVATE ${LUA_LIBRARIES})
endif()

# ---- ENET ----
target_link_libraries(Universe PRIVATE ${ENET_LIBRARY})

target_link_libraries(Universe PRIVATE ${BOX2D_LIBRARY})

if(CMAKE_CROSSCOMPILING OR WIN32)
    target_link_libraries(Universe PRIVATE ws2_32 winmm)
endif()
# ---- BOX2D ----

# ---- FANN ----
if(CMAKE_CROSSCOMPILING OR WIN32)
    target_include_directories(Universe PRIVATE
        "${CMAKE_SOURCE_DIR}/Third_party/FANN/include"
    )
    target_link_libraries(Universe PRIVATE
        "${CMAKE_SOURCE_DIR}/Third_party/FANN/lib/libfann.dll.a"
    )
    add_custom_command(TARGET Universe POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_SOURCE_DIR}/Third_party/FANN/bin/libfann.dll"
            "$<TARGET_FILE_DIR:Universe>"
        COMMENT "Kopiowanie FANN DLL..."
    )
else()
    find_library(FANN_LIB fann)
    if(FANN_LIB)
        target_link_libraries(Universe PRIVATE ${FANN_LIB})
    else()
        target_include_directories(Universe PRIVATE
            "${CMAKE_SOURCE_DIR}/Third_party/FANN/include"
        )
        target_link_libraries(Universe PRIVATE
            "${CMAKE_SOURCE_DIR}/Third_party/FANN/lib/libfann_static.a"
        )
    endif()
endif()


if(UNIX AND NOT CMAKE_CROSSCOMPILING)

    target_link_libraries(Universe PRIVATE m pthread dl)

    set(APPIMAGETOOL "${CMAKE_BINARY_DIR}/appimagetool-x86_64.AppImage")
    if(NOT EXISTS "${APPIMAGETOOL}")
        message(STATUS "Pobieranie appimagetool...")
        file(DOWNLOAD
            "https://github.com/AppImage/AppImageKit/releases/download/continuous/appimagetool-x86_64.AppImage"
            "${APPIMAGETOOL}"
            SHOW_PROGRESS
        )
        execute_process(COMMAND chmod +x "${APPIMAGETOOL}")
    endif()


    file(WRITE "${CMAKE_BINARY_DIR}/AppRun.sh"
[==[#!/bin/bash
HERE="$(dirname "$(readlink -f "$0")")"
OWD="$(pwd)"
export LD_LIBRARY_PATH="${HERE}/usr/lib${LD_LIBRARY_PATH:+:${LD_LIBRARY_PATH}}"
cd "${OWD}"
exec "${HERE}/usr/bin/HolyRock" "$@"
]==])

    file(WRITE "${CMAKE_BINARY_DIR}/build_appimage.cmake" "
        set(APP_DIR \"\${CMAKE_BINARY_DIR}/AppDir\")
        set(EXE     \"\${DEST}/HolyRock\")

        file(REMOVE_RECURSE \"\${APP_DIR}\")
        file(MAKE_DIRECTORY \"\${APP_DIR}/usr/bin\")
        file(MAKE_DIRECTORY \"\${APP_DIR}/usr/lib\")
        file(MAKE_DIRECTORY \"\${APP_DIR}/usr/share/icons/hicolor/256x256/apps\")

        file(COPY \"\${EXE}\" DESTINATION \"\${APP_DIR}/usr/bin/\")

        execute_process(
            COMMAND ldd \"\${EXE}\"
            OUTPUT_VARIABLE LDD_OUTPUT
            ERROR_QUIET
        )
        string(REPLACE \"\\n\" \";\" LDD_LINES \"\${LDD_OUTPUT}\")
        foreach(LINE \${LDD_LINES})
            if(LINE MATCHES \"=> (/[^ ]+)\")
                set(SO_PATH \"\${CMAKE_MATCH_1}\")
                if(SO_PATH MATCHES \"/libc\\.so|/ld-linux|/libdl\\.so|/libm\\.so|/libpthread\\.so|/librt\\.so|/libgcc_s\")
                    continue()
                endif()
                if(EXISTS \"\${SO_PATH}\")
                    get_filename_component(REAL_PATH \"\${SO_PATH}\" REALPATH)
                    get_filename_component(REAL_FNAME \"\${REAL_PATH}\" NAME)
                    get_filename_component(LINK_FNAME \"\${SO_PATH}\" NAME)
                    if(NOT EXISTS \"\${APP_DIR}/usr/lib/\${REAL_FNAME}\")
                        message(STATUS \"  .so: \${REAL_FNAME}\")
                        file(COPY \"\${REAL_PATH}\" DESTINATION \"\${APP_DIR}/usr/lib/\")
                    endif()
                    if(NOT LINK_FNAME STREQUAL REAL_FNAME AND NOT EXISTS \"\${APP_DIR}/usr/lib/\${LINK_FNAME}\")
                        execute_process(COMMAND \${CMAKE_COMMAND} -E create_symlink
                            \"\${REAL_FNAME}\" \"\${APP_DIR}/usr/lib/\${LINK_FNAME}\")
                    endif()
                endif()
            endif()
        endforeach()

        foreach(DIR /usr/lib /usr/local/lib /usr/lib/x86_64-linux-gnu /usr/lib/aarch64-linux-gnu)
            file(GLOB STB_FILES \"\${DIR}/libstb.so*\")
            foreach(F \${STB_FILES})
                get_filename_component(FNAME \${F} NAME)
                if(NOT EXISTS \"\${APP_DIR}/usr/lib/\${FNAME}\")
                    message(STATUS \"  .so (stb fallback): \${FNAME}\")
                    file(COPY \${F} DESTINATION \"\${APP_DIR}/usr/lib/\")
                endif()
            endforeach()
        endforeach()

        if(EXISTS \"\${APP_DIR}/usr/lib/libstb.so.0.0\" AND NOT EXISTS \"\${APP_DIR}/usr/lib/libstb.so.0\")
            execute_process(COMMAND \${CMAKE_COMMAND} -E create_symlink libstb.so.0.0 \"\${APP_DIR}/usr/lib/libstb.so.0\")
        endif()
        if(EXISTS \"\${APP_DIR}/usr/lib/libstb.so.0\" AND NOT EXISTS \"\${APP_DIR}/usr/lib/libstb.so\")
            execute_process(COMMAND \${CMAKE_COMMAND} -E create_symlink libstb.so.0 \"\${APP_DIR}/usr/lib/libstb.so\")
        endif()

        file(COPY \"\${CMAKE_BINARY_DIR}/AppRun.sh\" DESTINATION \"\${APP_DIR}/\")
        file(RENAME \"\${APP_DIR}/AppRun.sh\" \"\${APP_DIR}/AppRun\")
        execute_process(COMMAND chmod +x \"\${APP_DIR}/AppRun\")

        file(WRITE \"\${APP_DIR}/HolyRock.desktop\"
\"[Desktop Entry]
Name=HolyRock
Exec=HolyRock
Icon=HolyRock
Type=Application
Categories=Game;
\")

        if(EXISTS \"\${SRC_DIR}/Assets/icon.png\")
            file(COPY \"\${SRC_DIR}/Assets/icon.png\" DESTINATION \"\${APP_DIR}/\")
            file(RENAME \"\${APP_DIR}/icon.png\" \"\${APP_DIR}/HolyRock.png\")
            file(COPY \"\${APP_DIR}/HolyRock.png\"
                 DESTINATION \"\${APP_DIR}/usr/share/icons/hicolor/256x256/apps/\")
        else()
            file(WRITE \"\${APP_DIR}/HolyRock.png\" \"\")
            file(WRITE \"\${APP_DIR}/usr/share/icons/hicolor/256x256/apps/HolyRock.png\" \"\")
        endif()

        # POPRAWKA 2: --appimage-extract-and-run omija wymóg FUSE podczas budowania
        message(STATUS \"Budowanie HolyRock.AppImage...\")
        execute_process(
            COMMAND env ARCH=x86_64 \${APPIMAGETOOL} --appimage-extract-and-run
                \"\${APP_DIR}\" \"\${DEST}/HolyRock.AppImage\"
            RESULT_VARIABLE RESULT
        )
        if(RESULT EQUAL 0)
            message(STATUS \">>> HolyRock.AppImage gotowy w Game/\")
        else()
            message(WARNING \"appimagetool nie powiodło się! Zainstaluj: sudo apt install libfuse2\")
        endif()

        if(EXISTS \"\${SRC_DIR}/Assets\")
            file(COPY \"\${SRC_DIR}/Assets\" DESTINATION \"\${DEST}/\")
            message(STATUS \">>> Assets/ skopiowane do Game/\")
        endif()
    ")

    add_custom_command(TARGET Universe POST_BUILD
        COMMAND ${CMAKE_COMMAND}
            -DEXE=$<TARGET_FILE:Universe>
            -DDEST=$<TARGET_FILE_DIR:Universe>
            -DSRC_DIR=${CMAKE_SOURCE_DIR}
            -DCMAKE_BINARY_DIR=${CMAKE_BINARY_DIR}
            -DAPPIMAGETOOL=${APPIMAGETOOL}
            -P "${CMAKE_BINARY_DIR}/build_appimage.cmake"
        COMMENT "Budowanie HolyRock.AppImage..."
    )

endif()


if(CMAKE_CROSSCOMPILING OR WIN32)

    if(CMAKE_CROSSCOMPILING)
        target_include_directories(Universe PRIVATE
            "${CMAKE_SOURCE_DIR}/Third_party/SFML/include"
        )
        target_link_libraries(Universe PRIVATE winmm gdi32 opengl32)
    endif()

    add_custom_command(TARGET Universe POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/Third_party/SFML/bin"
            "$<TARGET_FILE_DIR:Universe>"
        COMMENT "Kopiowanie DLL SFML..."
    )

    add_custom_command(TARGET Universe POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/Third_party/LUA/lib/windows/liblua.dll"
            "$<TARGET_FILE_DIR:Universe>/"
        COMMENT "Kopiowanie Lua DLL..."
    )

    add_custom_command(TARGET Universe POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/Assets"
            "$<TARGET_FILE_DIR:Universe>/Assets"
        COMMENT "Kopiowanie Assets/..."
    )

endif()


if(MSVC)
    target_compile_options(Universe PRIVATE /W4 /utf-8)
else()
    target_compile_options(Universe PRIVATE -Wall -Wextra)
endif()
