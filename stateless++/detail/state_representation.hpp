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

#ifndef STATELESS_DETAIL_STATE_REPRESENTATION_HPP
#define STATELESS_DETAIL_STATE_REPRESENTATION_HPP

#include <algorithm>
#include <map>
#include <memory>
#include <set>
#include <type_traits>
#include <vector>

#include "../error.hpp"
#include "transition.hpp"
#include "trigger_behaviour.hpp"

namespace stateless
{

namespace detail
{

class abstract_entry_action
{
public:
  virtual ~abstract_entry_action() = 0;
};

inline abstract_entry_action::~abstract_entry_action()
{}

template<typename TTransition, typename... TArgs>
struct entry_action : public abstract_entry_action
{
  entry_action(const std::function<void(const TTransition&, TArgs...)>& action)
    : execute(action)
  {}
  
  std::function<void(const TTransition&, TArgs...)> execute;
};
  
template<typename TState, typename TTrigger>
class state_representation
{
public:
  typedef transition<TState, TTrigger> TTransition;
  typedef std::shared_ptr<abstract_trigger_behaviour> TTriggerBehaviour;
  typedef std::shared_ptr<abstract_entry_action> TEntryAction;
  typedef std::function<void(const TTransition&)> TExitAction;

  state_representation(const TState& state)
    : state_(state)
    , trigger_behaviours_()
    , entry_actions_()
    , exit_actions_()
    , super_state_(nullptr)
    , sub_states_()
  {}

  bool can_handle(const TTrigger& trigger) const
  {
    return try_find_handler(trigger) != nullptr;
  }

  const TTriggerBehaviour try_find_handler(const TTrigger& trigger) const
  {
    auto handler = try_find_local_hander(trigger);
    if (handler == nullptr && super_state_ != nullptr)
    {
      handler = super_state_->try_find_handler(trigger);
    }
    return handler;
  }

  template<typename TCallable, typename... TArgs>
  void add_entry_action(TCallable action)
  {
    auto ea = std::make_shared<entry_action<TTransition, TArgs...>>(action);
    entry_actions_.push_back(ea);
  }

  template<typename TCallable, typename... TArgs>
  void add_entry_action(const TTrigger& trigger, TCallable action)
  {
    auto wrapper =
      [=](const TTransition& transition, TArgs&&... args)
      {
        if (transition.trigger() == trigger)
        {
#if _WIN32
          // Workaround for error C3849 http://msdn.microsoft.com/en-us/library/031k84se.aspx
          typedef typename std::remove_cv<TCallable>::type TCVRemoved;
          ((TCVRemoved)(action))(transition, args...);
#else
          action(transition, std::forward<TArgs>(args)...);
#endif
        }
      };
    auto ea = std::make_shared<entry_action<TTransition, TArgs...>>(wrapper);
    entry_actions_.push_back(ea);
  }

  void add_exit_action(const TExitAction& exit_action)
  {
    exit_actions_.push_back(exit_action);
  }

  template<typename... TArgs>
  void enter(const TTransition& transition, TArgs&&... args) const
  {
    if (transition.is_reentry())
    {
      execute_entry_actions(transition, std::forward<TArgs>(args)...);
    }
    else if (!includes(transition.source()))
    {
      if (super_state_ != nullptr)
      {
        super_state_->enter(transition, std::forward<TArgs>(args)...);
      }
      execute_entry_actions(transition, std::forward<TArgs>(args)...);
    }
  }

  void exit(const TTransition& transition) const
  {
    if (transition.is_reentry())
    {
      execute_exit_actions(transition);
    }
    else if (!includes(transition.destination()))
    {
      execute_exit_actions(transition);
      if (super_state_ != nullptr)
      {
        super_state_->exit(transition);
      }
    }
  }

  void add_trigger_behaviour(const TTrigger& trigger, const TTriggerBehaviour trigger_behaviour)
  {
    trigger_behaviours_[trigger].push_back(trigger_behaviour);
  }

  const state_representation& super_state() const
  {
    return *super_state_;
  }

  void set_super_state(const state_representation* super_state)
  {
    super_state_ = super_state;
  }

  const TState& underlying_state() const
  {
    return state_;
  }

  void add_sub_state(const state_representation* sub_state)
  {
    sub_states_.push_back(sub_state);
  }

  bool includes(const TState& state) const
  {
    if (state == state_)
    {
      return true;
    }
    for (const auto sub_state : sub_states_)
    {
      if (sub_state->includes(state))
      {
        return true;
      }
    }
    return false;
  }

  bool is_included_in(const TState& state) const
  {
    return (state == state_ ||
      (super_state_ != nullptr && super_state_->is_included_in(state)));
  }
  
  std::set<TTrigger> permitted_triggers() const
  {
    std::set<TTrigger> local;
    for (auto& trigger_behaviour_list : trigger_behaviours_)
    {
      for (auto& trigger_behaviour : trigger_behaviour_list.second)
      {
        if (trigger_behaviour->is_condition_met())
        {
          local.insert(trigger_behaviour_list.first);
          break;
        }
      }
    }
      
    if (super_state_ != nullptr)
    {
      auto super = super_state_->permitted_triggers();
      std::set<TTrigger> result;
      std::set_union(
        local.begin(), local.end(),
        super.begin(), super.end(),
        std::inserter(result, result.begin()));
      return result;
    }    
    return local;
  }

private:
  const TTriggerBehaviour try_find_local_hander(const TTrigger& trigger) const
  {
    TTriggerBehaviour result = nullptr;

    const auto& candidates = trigger_behaviours_.find(trigger);
    if (candidates == trigger_behaviours_.end())
    {
      return result;
    }

    for (auto& candidate : candidates->second)
    {
      if (candidate->is_condition_met())
      {
        if (result != nullptr)
        {
          throw error(
            "Multiple permitted exit transitions are "
            "configured from the current state. Guard "
            "clauses must be mutually exclusive.");
        }
        result = candidate;
      }
    }

    return result;
  }

  template<typename... TArgs>
  void execute_entry_actions(const TTransition& transition, TArgs&&... args) const
  {
    for (auto& action : entry_actions_)
    {
      if (auto ea = std::dynamic_pointer_cast<entry_action<TTransition, TArgs...>>(action))
      {
        ea->execute(transition, std::forward<TArgs>(args)...);
      }
    }
  }

  void execute_exit_actions(const TTransition& transition) const
  {
    for (auto& action : exit_actions_)
    {
      action(transition);
    }
  }

  const TState state_;

  std::map<TTrigger, std::vector<TTriggerBehaviour>> trigger_behaviours_;
  std::vector<TEntryAction> entry_actions_;
  std::vector<TExitAction> exit_actions_;

  const state_representation* super_state_;
  std::vector<const state_representation*> sub_states_;
};

}

}

#endif // STATELESS_DETAIL_STATE_REPRESENTATION_HPP
