# Top level CMake file for stateless++.
cmake_minimum_required(VERSION 2.8)

project(stateless++)

if(CYGWIN AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  add_definitions(--std=c++11)
  include_directories(BEFORE SYSTEM /usr/lib/gcc/i686-pc-cygwin/4.7.2/include/c++)
  include_directories(BEFORE SYSTEM /usr/lib/gcc/i686-pc-cygwin/4.7.2/include/c++/i686-pc-cygwin)
  include_directories(BEFORE SYSTEM /usr/lib/gcc/i686-pc-cygwin/4.7.2/include/c++/backward)
  include_directories(BEFORE SYSTEM /usr/lib/gcc/i686-pc-cygwin/4.7.2/include)
  include_directories(BEFORE SYSTEM /usr/lib/gcc/i686-pc-cygwin/4.7.2/include-fixed)
endif(!MSVC)

# Note that setting the toolset doesn't have any effect at the moment
# but it probably will do in future.
if(MSVC)
  message("${CMAKE_VS_PLATFORM_TOOLSET}")
  set(CMAKE_VS_PLATFORM_TOOLSET "Microsoft Visual C++ Compiler Nov 2012 CTP (v120_CTP_Nov2012)")
endif(MSVC)

add_subdirectory(examples)
add_subdirectory(stateless++)
add_subdirectory(test)
