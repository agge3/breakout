# cmake tested versions
cmake_minimum_required(VERSION 3.22...3.27)

# Project name and a few useful settings. Other commands can pick up the results
project(practice_game
    VERSION 0.1.0
    DESCRIPTION "It's a game"
    LANGUAGES CXX)

# Configuration options
#set(SFML_STATIC_LIBRARIES FALSE CACHE BOOL "Use static SFML librares")

#set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake) # Tell CMake where to find the module
find_package(SFML REQUIRED COMPONENTS graphics window audio network system) # Look for SFML

#include_directories(${SFML_INCLUDE_DIR}) # You forgot this line above; add SFML's include dir

# link sfml libraries
#find_package(SFML 2 COMPONENTS audio graphics window system network REQUIRED)

# Optionally set things like CMAKE_CXX_STANDARD, CMAKE_POSITION_INDEPENDENT_CODE here

# directory where binary/build will go
#add_subdirectory(build)
#set(CMAKE_CURRENT_BINARY_DIR build)

# list your sources
add_executable(practice_game src/main.cpp)

target_compile_features(practice_game PRIVATE cxx_std_20)
target_include_directories(practice_game PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
#target_link_libraries(game ${SFML_LIBRARIES} ${SFML_DEPENDENIES})
target_link_libraries(practice_game
    sfml-graphics
    sfml-window
    sfml-audio
    sfml-network
    sfml-system)


#[=[



# Let's ensure -std=c++xx instead of -std=g++xx
set(CMAKE_CXX_EXTENSIONS OFF)

# Let's nicely support folders in IDEs
#set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Testing only available if this is the main app
# Note this needs to be done in the main CMakeLists
# since it calls enable_testing, which must be in the
# main CMakeLists.
include(CTest)

# Docs only available if this is the main app
find_package(Doxygen)
if(Doxygen_FOUND)
    add_subdirectory(doc)
else()
    message(STATUS "Doxygen not found, not building docs")
endif()

# The compiled library code is here
add_subdirectory(src)

# The executable code is here
add_subdirectory(build)

# header/interface files
add_subdirectory(include)

# Testing only available if this is the main app
# Emergency override MODERN_CMAKE_BUILD_TESTING provided as well
if((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME OR CMAKE_BUILD_TESTING)
   AND BUILD_TESTING)
  add_subdirectory(test)
endif()


#]=]



#[=[
Examples:

add_library(main main.cpp)
target_link_libraries(main PRIVATE Bar::Bar)

if(WIN32)
    target_sources(main PRIVATE main_win32.cpp)
    target_link_libraries(main PRIVATE bar::Win32Support)
endif()

Avoid custom var in the arguments of project commands - use targets

Don't use file globbing in projects, only in scripts

#]=]
