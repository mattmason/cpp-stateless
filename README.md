cpp-stateless
=============

Port of the [C# Stateless library](https://code.google.com/p/stateless/) to C++11.
It's a lightweight state machine implementation with a fluent configuration interface.

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
// - less than comparable
state_machine<std::string, char> on_off_switch(off);

// Set up using fluent configuration interface.
on_off_switch.configure(off).permit(space, on);
on_off_switch.configure(on).permit(space, off);

// Drive the machine by firing triggers.
on_off_switch.fire(space); // <-- state is now "On"
...
```

See the [bug tracker example](examples/bug_tracker/bug.cpp) for a more comprehensive use of the configuration API including
parameterized triggers, sub-states and entry and exit actions.

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

The library, example code and tests have been built and run on the following platforms:

 - gcc 4.7.2 on Cygwin, gcc 4.7.3 on Ubuntu 12.04

   No known issues.

 - Clang 3.1 on Cygwin, 3.2 on Ubuntu
    
    Use the patch attached to [this bug report](http://bugs.debian.org/cgi-bin/bugreport.cgi?bug=678033) to allow use of --std=gnu++11.

 - Visual Studio 2012 on Windows 7
    
    Requires the [Microsoft Visual C++ Compiler Nov 2012 CTP Toolset](http://www.microsoft.com/en-gb/download/details.aspx?id=35515).
    The cmake build script attempts to configure this toolset but the [cmake CMAKE_VS_PLATFORM_TOOLSET variable is currently
    read-only](http://www.cmake.org/Bug/view.php?id=13774#c31828) so you have to manually update the toolset in each project file
    to "Microsoft Visual C++ Compiler Nov 2012 CTP (v120_CTP_Nov2012)". [This PowerShell script](Set-Toolset.ps1) automates the process.
    If you want to run the script you may need to run PowerShell as Administrator and run ```Set-ExecutionPolicy Unrestricted``` first.

Build and Install
-----------------
The library itself is header file only.
The examples are built by default but this can be skipped if you just want to install the library header files.
The unit tests depend on [GoogleTest](https://code.google.com/p/googletest/) version 1.6.0, but only are only built
if the gtest library is found on the target system.

The instructions for UNIX-like platforms are:
```
git clone https://github.com/mattmason/cpp-stateless
mkdir build && cd build # Build without polluting the source tree
cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr/local/ ../cpp-stateless
```
To build examples, build and run unit tests, and install the headers:
```
make && make test && make install # sudo may be required for make install
```
To install the headers without building examples and tests:
```
cd stateless++ && make install # sudo may be required for make install
```
For Visual Studio 2012 use the generated project files to build from within the IDE or on the command line.

Tasks
----
 - [ ] Dynamic destination state selection.
 - [x] Investigate Clang 3.1/cygwin 3.2/Ubuntu unit test failures and on/off example failure.
