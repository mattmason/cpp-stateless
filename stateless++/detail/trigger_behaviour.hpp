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

#ifndef STATELESS_DETAIL_TRIGGER_BEHAVIOUR_HPP
#define STATELESS_DETAIL_TRIGGER_BEHAVIOUR_HPP

#include <functional>

#include "../error.hpp"

namespace stateless
{

namespace detail
{

class abstract_trigger_behaviour
{
public:
  typedef std::function<bool()> TGuard;

  abstract_trigger_behaviour(const TGuard& guard)
    : guard_(guard)
  {}

  bool is_condition_met() const
  {
    return guard_();
  }

  virtual ~abstract_trigger_behaviour() = 0;

private:
  TGuard guard_;
};

inline abstract_trigger_behaviour::~abstract_trigger_behaviour()
{}

template<typename TState, typename TTrigger>
class trigger_behaviour
  : public abstract_trigger_behaviour
{
public:
  typedef std::function<bool(const TState&, TState&)> TDecision;

  trigger_behaviour(
    const TTrigger& trigger,
    const abstract_trigger_behaviour::TGuard& guard,
    const TDecision& decision)
    : abstract_trigger_behaviour(guard)
    , trigger_(trigger)
    , decision_(decision)
  {}

  const TTrigger& trigger() const
  {
    return trigger_;
  }

  bool results_in_transition_from(const TState& source, TState& destination) const
  {
    if (!decision_)
    {
      throw error("Static trigger behaviour decision is not set. "
        "The state machine is misconfigured.");
    }
    return decision_(source, destination);
  }

  trigger_behaviour(
    const TTrigger& trigger,
    const abstract_trigger_behaviour::TGuard& guard)
    : abstract_trigger_behaviour(guard)
    , trigger_(trigger)
  {}

private:
  const TTrigger trigger_;
  TDecision decision_;
};

template<typename TState, typename TTrigger, typename... TArgs>
class dynamic_trigger_behaviour
  : public trigger_behaviour<TState, TTrigger>
{
public:
  typedef typename std::function<TState(TArgs...)> TDecision;

  dynamic_trigger_behaviour(
    const TTrigger& trigger,
    const abstract_trigger_behaviour::TGuard& guard,
    const TDecision& decision)
    : trigger_behaviour<TState, TTrigger>(trigger, guard)
    , decision_(decision)
  {}

  bool results_in_transition_from(const TState& source, TState& destination, TArgs&&... args) const
  {
    destination = decision_(std::forward<TArgs>(args)...);
    return true;
  }

private:
  TDecision decision_;
};

}

}

#endif // STATELESS_DETAIL_TRIGGER_BEHAVIOUR_HPP
