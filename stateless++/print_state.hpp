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

#include <memory>
#include <type_traits>

namespace stateless
{

/**
 * Generic state value printer.
 *
 * Specialize to provide more informative printing
 * of a state machine's current state.
 */
template<typename TState>
inline void print_state(std::ostream& os, const TState& s)
{ os << "TState@" << std::addressof(s); }

#ifndef STATELESS_NO_PRETTY_PRINT

/**
 * Specialize state printing for string.
 */
template<typename TState>
inline void print_state(
  std::ostream& os,
  const typename std::enable_if<std::is_convertible<TState, std::string>::value, std::string>::type& s)
{ os << s; }

/**
 * Specialize state printing for enum types.
 */
template<typename TState>
inline void print_state(
  std::ostream& os,
  const typename std::enable_if<std::is_enum<TState>::value, TState>::type& s)
{ os << static_cast<typename std::underlying_type<TState>::type>(s); }

/**
 * Specialize state printing for arithmetic types.
 */
template<typename TState>
inline void print_state(
  std::ostream& os,
  const typename std::enable_if<std::is_arithmetic<TState>::value, TState>::type& s)
{ os << s; }

#endif // STATELESS_NO_PRETTY_PRINT

}

#endif // STATELESS_PRINT_STATE_HPP

