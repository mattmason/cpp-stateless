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

#include <memory>
#include <type_traits>

namespace stateless
{

/**
 * Generic trigger value printer.
 *
 * Specialize to provide more informative printing
 * of a state machine's permitted triggers.
 */
template<typename TTrigger>
inline void print_trigger(std::ostream& os, const TTrigger& t)
{ os << "TTrigger@" << std::addressof(t); }

#ifndef STATELESS_NO_PRETTY_PRINT

/**
 * Specialize trigger printing for string.
 */
template<typename TTrigger>
inline void print_trigger(
  std::ostream& os,
  const typename std::enable_if<std::is_convertible<TTrigger, std::string>::value, std::string>::type& t)
{ os << t; }

/**
 * Specialize trigger printing for enum types.
 */
template<typename TTrigger>
inline void print_trigger(
  std::ostream& os,
  const typename std::enable_if<std::is_enum<TTrigger>::value, TTrigger>::type& t)
{ os << static_cast<typename std::underlying_type<TTrigger>::type>(t); }

/**
 * Specialize trigger printing for arithmetic types.
 */
template<typename TTrigger>
inline void print_trigger(
  std::ostream& os,
  const typename std::enable_if<std::is_arithmetic<TTrigger>::value, TTrigger>::type& t)
{ os << t; }

#endif // STATELESS_NO_PRETTY_PRINT

}

#endif // STATELESS_PRINT_TRIGGER_HPP

