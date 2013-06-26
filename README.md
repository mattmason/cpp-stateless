cpp-stateless
=============

Port of the [C# Stateless library](https://code.google.com/p/stateless/) to C++11.

The goal of the project is to provide an API that is as close as possible to that of the original
C# library using only standard C++11 features. No external dependencies are required.

A simple example:
```cpp
#include <stateless++/state_machine.hpp>
...
std::string on("On"), off("Off");
const char space(' ');

// Create a state machine with state type string and trigger type char.
// The state and trigger types can be any type that is
// - default constructible
// - assignable and copyable
// - equality comparable
state_machine<std::string, char> onOffSwitch(off);

// Set up using fluent configuration interface.
onOffSwitch.configure(off).permit(space, on);
onOffSwitch.configure(on).permit(space, off);

// Drive the machine by firing triggers.
onOffSwitch.fire(space); // <-- state is now "On"
...
```

See the [bug tracker example](examples/bug_tracker/bug.cpp) for a more comprehensive use of the configuration API including
parameterized triggers and entry and exit actions.

License
-------
The library is licensed under the terms of the [Apache License 2.0](http://www.apache.org/licenses/LICENSE-2.0.html).

Acknowledgements
----------------
Thanks to [Nicholas Blumhardt](http://nblumhardt.com/) for writing the original library in C#
and making it available under a permissive license.

Supported Platforms
-------------------
[CMake](http://www.cmake.org/) build files are supplied to provide portability with minimal effort.

The library and example code has been built on gcc 4.7.2 and Clang version 3.1 on Cygwin.

Visual Studio 2012 requires the [Microsoft Visual C++ Compiler Nov 2012 CTP
Toolset](http://www.microsoft.com/en-gb/download/details.aspx?id=35515).
The cmake build script attempts to configure this toolset but the [cmake CMAKE_VS_PLATFORM_TOOLSET variable is currently
read-only](http://www.cmake.org/Bug/view.php?id=13774#c31828) so you have to manually update the toolset in each project file
to "Microsoft Visual C++ Compiler Nov 2012 CTP (v120_CTP_Nov2012)".

Build and Install
-----------------
The library itself consists purely of header files. The examples and tests are built but this can be skipped if desired.
The instructions are the same for all target platforms.
 1. git clone https://github.com/mattmason/cpp-stateless
 1. cd cpp-stateless
 1. mkdir build && cd build
 1. cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr/local/include/ ../cpp-stateless
 1. To build examples, tests, and install the headers: make install
 1. To install the headers without building examples and tests: cd stateless++ && make install

Tasks
----
 - [x] Basic functionality.
 - [x] Parameterized triggers.
 - [ ] Dynamic destination state selection.
 - [ ] Unit tests.
 - [x] Investigate VS2012 build failure.
