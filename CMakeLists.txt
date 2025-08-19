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
set(SOURCES src/DllMain.cpp src/Game.cpp src/FileIO.cpp src/Blips.cpp)

set(HEADERS src/Offsets.h src/Game.h src/FileIO.h src/Blips.h)

# Create the shared library (DLL)
add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})

# Default dev value
set(VANILLAHELPERS_VERSION_VALUE 999999)

set(VANILLAHELPERS_TAG
    ""
    CACHE STRING "Release tag like v1.2.3")

if(VANILLAHELPERS_TAG MATCHES "^v([0-9]+)\\.([0-9]+)\\.([0-9]+)$")
  set(_MAJOR "${CMAKE_MATCH_1}")
  set(_MINOR "${CMAKE_MATCH_2}")
  set(_PATCH "${CMAKE_MATCH_3}")
  math(EXPR VANILLAHELPERS_VERSION_VALUE
       "${_MAJOR} * 10000 + ${_MINOR} * 100 + ${_PATCH}")
elseif(VANILLAHELPERS_TAG)
  message(
    FATAL_ERROR
      "Invalid VANILLAHELPERS_TAG: ${VANILLAHELPERS_TAG} (expected vMAJOR.MINOR.PATCH)"
  )
endif()

target_compile_definitions(
  ${PROJECT_NAME}
  PRIVATE VANILLAHELPERS_VERSION_VALUE=${VANILLAHELPERS_VERSION_VALUE})

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
