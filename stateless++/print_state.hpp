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

#ifndef STATELESS_PRINT_STATE_HPP
#define STATELESS_PRINT_STATE_HPP

namespace stateless
{

/**
 * Generic state value printer.
 *
 * Specialize to provide more informative printing
 * of a state machine's current state.
 */
template<typename TState> struct print_state
{
  static void print(std::ostream& os, const TState& s)
  {
    os << "TState@" << &s;
  }
};

}

#endif // STATELESS_PRINT_STATE_HPP

