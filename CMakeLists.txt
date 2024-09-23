cmake_minimum_required(VERSION 3.11)

# Setup Project
project(UGP)
set(CMAKE_CXX_STANDARD 20)
set(UGP_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/Source")
set(UGP_SOURCE "${UGP_INCLUDE}/UnrealGamePackager")

include(FetchContent)

# Fetch fmt library
FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt
  GIT_TAG        e69e5f977d458f2650bb346dadf2ad30c5320281
) # 10.2.1
FetchContent_MakeAvailable(fmt)

# Find source files
file(GLOB_RECURSE UGP_SOURCES CONFIGURE_DEPENDS "${UGP_SOURCE}/*.cpp")

# Declare project
add_executable(UGP)
set_target_properties(UGP PROPERTIES OUTPUT_NAME "UnrealGamePackager")
target_include_directories(UGP PRIVATE "${UGP_INCLUDE}")
target_sources(UGP PRIVATE "${UGP_SOURCES}")
target_link_libraries(UGP PRIVATE fmt::fmt)

# Add libs
target_include_directories(UGP PUBLIC "Libraries/simpleini-4.22/include")
