# Copyright 2013 Matt Mason
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# Top level CMake file for stateless++.

cmake_minimum_required(VERSION 2.8)

project(stateless++)

enable_testing()

if (CYGWIN AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  include_directories(BEFORE SYSTEM /usr/lib/gcc/i686-pc-cygwin/4.7.2/include/c++)
  include_directories(BEFORE SYSTEM /usr/lib/gcc/i686-pc-cygwin/4.7.2/include/c++/i686-pc-cygwin)
  include_directories(BEFORE SYSTEM /usr/lib/gcc/i686-pc-cygwin/4.7.2/include/c++/backward)
  include_directories(BEFORE SYSTEM /usr/lib/gcc/i686-pc-cygwin/4.7.2/include)
  include_directories(BEFORE SYSTEM /usr/lib/gcc/i686-pc-cygwin/4.7.2/include-fixed)
endif (CYGWIN AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")

if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux" AND ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
  include_directories(BEFORE SYSTEM /usr/include/x86_64-linux-gnu/c++/4.7)
endif (${CMAKE_SYSTEM_NAME} STREQUAL "Linux" AND ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
message(STATUS "CMAKE_CXX_FLAGS_RELWITHDEBINFO = " ${CMAKE_CXX_FLAGS_RELWITHDEBINFO})
message(STATUS "CMAKE_BUILD_TYPE = " ${CMAKE_BUILD_TYPE})

if (CYGWIN)
  add_definitions(--std=gnu++11)
endif (CYGWIN)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
  add_definitions(--std=c++11)
endif (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")

if (MSVC)
  # Note that setting the toolset doesn't have any effect at the moment
  # but it probably will do in future.
  set(CMAKE_VS_PLATFORM_TOOLSET "v120_CTP_Nov2012")
endif (MSVC)

add_subdirectory(examples)
add_subdirectory(stateless++)
add_subdirectory(test)
