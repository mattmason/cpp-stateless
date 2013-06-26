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

#ifndef STATELESS_TRIGGER_BEHAVIOUR_HPP
#define STATELESS_TRIGGER_BEHAVIOUR_HPP

#include <functional>

namespace stateless
{

namespace detail
{

template<typename TState, typename TTrigger>
class trigger_behaviour
{
public:
  typedef std::function<bool()> TGuard;
  typedef std::function<bool(const TState&, TState&)> TDecision;

  trigger_behaviour(const TTrigger& trigger, const TGuard& guard, const TDecision& decision)
    : trigger_(trigger)
    , guard_(guard)
    , decision_(decision)
  {}

  const TTrigger& trigger() const
  {
    return trigger_;
  }

  bool is_condition_met() const
  {
    return guard_();
  }

  const bool results_in_transition_from(const TState& source, TState& destination) const
  {
    return decision_(source, destination);
  }

private:
  const TTrigger trigger_;
  TGuard guard_;
  TDecision decision_;
};

}

}

#endif // STATELESS_TRIGGER_BEHAVIOUR_HPP
