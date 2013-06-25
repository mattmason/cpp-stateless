
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
