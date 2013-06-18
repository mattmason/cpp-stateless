
#include "bug.hpp"

#include <cstdlib>
#include <iostream>

using namespace bug_tracker_example;

int main(int argc, char* argv[])
{
	bug bug("Incorrect stock count");

    bug.assign("Joe");
    bug.defer();
    bug.assign("Harry");
    bug.assign("Fred");
    bug.close();

	std::cout << "Press enter to quit..." << std::endl;
	char c;
	std::cin.get(c);

	return EXIT_SUCCESS;
}
