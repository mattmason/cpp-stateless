# Top level CMake file for stateless++.
cmake_minimum_required(VERSION 2.8)

if(CMAKE_COMPILER_IS_GNUCXX)
  add_definitions(--std=c++11)
endif(CMAKE_COMPILER_IS_GNUCXX)

project(stateless++)

# Note that setting the toolset doesn't have any effect at the moment
# but it probably will do in future.
if(MSVC)
  message("${CMAKE_VS_PLATFORM_TOOLSET}")
  set(CMAKE_VS_PLATFORM_TOOLSET "Microsoft Visual C++ Compiler Nov 2012 CTP (v120_CTP_Nov2012)")
endif(MSVC)

add_subdirectory(examples)
add_subdirectory(stateless++)
add_subdirectory(test)
