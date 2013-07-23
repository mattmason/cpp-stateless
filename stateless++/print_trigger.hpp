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

#ifndef STATELESS_PRINT_TRIGGER_HPP
#define STATELESS_PRINT_TRIGGER_HPP

namespace stateless
{

/**
 * Generic trigger value printer.
 *
 * Specialize to provide more informative printing
 * of a state machine's permitted triggers.
 */
template<typename TTrigger> struct print_trigger
{
  static void print(std::ostream& os, const TTrigger& t)
  {
    os << "TTrigger@" << &t;
  }
};

}

#endif // STATELESS_PRINT_TRIGGER_HPP

