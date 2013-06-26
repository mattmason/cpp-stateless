cpp-stateless
=============

Port of the C# Stateless library - https://code.google.com/p/stateless/ - to C++11.

License
-------
The library is licensed under the terms of the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0.

Acknowledgements
----------------
Thanks to Nicholas Blumhardt http://nblumhardt.com/ for writing the original library in C#
and making it available under a permissive license.

Overview
--------
The goal of the project is to provide an API that is as close as possible to that of the original
C# library using only standard C++11 features. No external dependencies are required.

A simple example:
```cpp
  std::string on("On"), off("Off");
  const char space(' ');

  state_machine<std::string, char> onOffSwitch(off);

  onOffSwitch.configure(off).permit(space, on);
  onOffSwitch.configure(on).permit(space, off);

  char c;
  std::cin.get(c);
  std::cin.ignore();
  onOffSwitch.fire(c); // <-- state is now "On"
```

Supported Platforms
-------------------
The library and example code has been built on gcc 4.7.2 and Clang version 3.1 on Cygwin.
It does not currently compile on Visual Studio (tested with November 2012 CTP).

Tasks
----
- [X] Basic functionality.
- [X] Parameterized triggers.
- [ ] Dynamic destination state selection.
- [ ] Unit tests.
