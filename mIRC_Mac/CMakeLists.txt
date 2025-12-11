cmake_minimum_required(VERSION 3.10)
project(mIRC_Mac)

# Retro68 CMake setup
# The user is expected to run: cmake .. -DCMAKE_TOOLCHAIN_FILE=.../Retro68.toolchain.cmake

if(COMMAND add_application)
    # Retro68 environment
    set(CMAKE_CXX_STANDARD 17) # Retro68 supports modern C++

    set(SOURCE_FILES
        src/main.cpp
        src/MacApp.cpp
        src/IRCClient.cpp
        src/MockImpl.cpp
    )

    # Define the application resources
    set(RESOURCE_FILES
        resources/mIRC.r
    )

    # Create the Macintosh Application
    # 'mIRC' is the app name
    # 'MIRC' is the Creator Code (4 chars)
    # 'APPL' is the Type (standard for apps)
    add_application(mIRC
        SOURCES ${SOURCE_FILES}
        RSRC ${RESOURCE_FILES}
        CREATOR "MIRC"
        TYPE "APPL"
        mac_app_files # This variable is populated by add_application
    )

    # Link against standard Retro68 libraries if needed (usually automatic for basic stuff)
    # target_link_libraries(mIRC ...)

else()
    # Local Linux/Headless environment for syntax checking only
    message(STATUS "Retro68 toolchain not detected. Configuring for local syntax check.")

    add_definitions(-DLOCAL_TESTING)
    include_directories(include) # For mock_mac.h

    add_executable(mIRC_SyntaxCheck
        src/main.cpp
        src/MacApp.cpp
        src/IRCClient.cpp
        src/MockImpl.cpp
    )

    # We might need to link pthread or similar if we use threading,
    # but Mac SE/30 code is usually single-threaded cooperative multitasking.
endif()
