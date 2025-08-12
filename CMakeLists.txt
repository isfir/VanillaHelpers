cmake_minimum_required(VERSION 3.10)

# Set the project name and specify it's a shared library (DLL)
project(VanillaHelpers LANGUAGES CXX)

# Enable C++17 (or adjust to your required C++ standard)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")

# Add the MinHook submodule
add_subdirectory(external/minhook)

# Add the source files for your project
set(SOURCES src/DllMain.cpp src/Game.cpp)

set(HEADERS src/Offsets.h src/Game.h)

# Create the shared library (DLL)
add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})

# Link MinHook to your project
target_link_libraries(${PROJECT_NAME} PRIVATE minhook)

# Include directories for your project and the MinHook submodule
target_include_directories(
  ${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
                          external/minhook/include)

# Set the output name for the DLL
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "VanillaHelpers")

# Use static runtime and optimize for smaller size
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MT")
set(CMAKE_CXX_FLAGS_RELEASE "/MT /O2 /Ob2 /DNDEBUG")
set(CMAKE_SHARED_LINKER_FLAGS
    "/OPT:REF /OPT:ICF /MERGE:.rdata=.text /MERGE:.pdata=.text /SECTION:.text,EWR /NODEFAULTLIB:MSVCRT"
)
