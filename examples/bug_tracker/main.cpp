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
  bug.resolve("Mike");
  bug.close();

  std::cout << "Press enter to quit..." << std::endl;
  char c;
  std::cin.get(c);

  return EXIT_SUCCESS;
}
