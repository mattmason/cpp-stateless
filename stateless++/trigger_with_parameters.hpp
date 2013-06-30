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

#ifndef STATELESS_TRIGGER_WITH_PARAMETERS_HPP
#define STATELESS_TRIGGER_WITH_PARAMETERS_HPP

namespace stateless
{

template<typename TTrigger>
class abstract_trigger_with_parameters
{
public:
  abstract_trigger_with_parameters(const TTrigger& underlying_trigger)
    : underlying_trigger_(underlying_trigger)
  {}
  
  virtual ~abstract_trigger_with_parameters() = 0;

  const TTrigger& trigger() const
  {
    return underlying_trigger_;
  }

private:
  const TTrigger underlying_trigger_;
};

template<typename TTrigger>
inline abstract_trigger_with_parameters<TTrigger>::~abstract_trigger_with_parameters()
{}

template<typename TTrigger, typename... TArgs>
class trigger_with_parameters : public abstract_trigger_with_parameters<TTrigger>
{
public:
  /**
   * Construct a parameterized trigger.
   * Not for client use; use state_machine::set_trigger_parameters.
   */
  trigger_with_parameters(const TTrigger& underlying_trigger)
    : abstract_trigger_with_parameters<TTrigger>(underlying_trigger)
  {}
};

}

#endif // STATELESS_TRIGGER_WITH_PARAMETERS_HPP
