/**
 * Copyright 2013 Matt Mason
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <state_machine.hpp>

#include <cstdlib>
#include <iostream>
#include <string>

using namespace stateless;

int main(int argc, char* argv[])
{
  try
  {
    std::string on("On"), off("Off");
    const char space(' ');

    state_machine<std::string, char> onOffSwitch(off);

    onOffSwitch.configure(off).permit(space, on);
    onOffSwitch.configure(on).permit(space, off);

    std::cout << "Press <space> to toggle the switch. Any other key will raise an error" << std::endl;

    while (true)
    {
      std::cout << "switch is in state " << onOffSwitch.state() << std::endl;
      char c;
      std::cin.get(c);
      std::cin.ignore();
      onOffSwitch.fire(c);
    }
  }
  catch (const std::exception& e)
  {
    std::cout << "Exception: " << e.what() << std::endl;
    std::cout << "Press enter to quit..." << std::endl;
    char c;
    std::cin.get(c);
  }
  return EXIT_SUCCESS;
}
