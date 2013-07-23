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

#ifndef STATELESS_STATE_MACHINE_HPP
#define STATELESS_STATE_MACHINE_HPP

#include <functional>
#include <map>
#include <memory>
#include <set>
#include <sstream>

#include "print_state.hpp"
#include "print_trigger.hpp"
#include "state_configuration.hpp"
#include "trigger_with_parameters.hpp"

namespace stateless
{

/**
 * Models behaviour as transitions between a finite set of states.
 *
 * \tparam TState The type used to represent the states.
 * \tparam TTrigger The type used to represent the triggers that cause state transitions.
 */
template<typename TState, typename TTrigger>
class state_machine
{
public:
  /// Parameterized state configuration type.
  typedef state_configuration<TState, TTrigger> TStateConfiguration;

  /// Parameterized transition type.
  typedef typename TStateConfiguration::TTransition TTransition;

  /// Parameterized trigger with parameters type.
  typedef typename TStateConfiguration::TTriggerWithParameters TTriggerWithParameters;

  /// Signature for read access of externally managed state.
  typedef std::function<const TState()> TStateAccessor;

  /// Signature for write access to externally managed state.
  typedef std::function<void(const TState&)> TStateMutator;

  /// Signature for handler for unhandled trigger. By default this throws an error.
  typedef std::function<void(const TState&, const TTrigger&)> TUnhandledTriggerAction;

  /// Signature for handler for state transition. Does nothing by default.
  typedef std::function<void(const TTransition&)> TTransitionAction;

  /**
   * Construct a state machine with external state storage.
   *
   * \param state_accessor A function that will be called to read the current state value.
   * \param state_mutator  An action that will be called to write new state values.
   */
  state_machine(const TStateAccessor& state_accessor, const TStateMutator& state_mutator)
  {
    init(state_accessor, state_mutator);
  }

  /**
   * Construct a state machine.
   *
   * \param initial_state The initial state.
   */
  state_machine(const TState& initial_state)
  {
    auto state = std::make_shared<state_reference>(initial_state);
    using namespace std::placeholders;
    init(
      std::bind(&state_reference::get, state),
      std::bind(&state_reference::set, state, _1));
  }

  /// The current state.
  const TState state() const
  {
    return state_accessor_();
  }

  /**
   * Begin configuration of the entry/exit actions and allowed transitions
   * when the state machine is in a particular state.
   *
   * \param state The state to configure.
   *
   * \return A configuration object through which the state can be configured.
   */
  TStateConfiguration configure(const TState& state)
  {
    using namespace std::placeholders;
    typedef state_machine<TState, TTrigger> TSelf;
    return TStateConfiguration(
      get_representation(state),
      std::bind(&TSelf::get_representation, this, _1));
  }

  /**
   * Transition from the current state via the supplied trigger.
   * The target state is determined by the configuration of the current state.
   * Actions associated with leaving the current state and entering the new one
   * will be invoked.
   *
   * \param trigger The trigger to fire.
   *
   * \throw error The current state does not allow the trigger to be fired.
   */
  void fire(const TTrigger& trigger)
  {
    internal_fire(trigger);
  }

  /**
   * Transition from the current state via the supplied trigger.
   * The target state is determined by the configuration of the current state.
   * Actions associated with leaving the current state and entering the new one
   * will be invoked.
   *
   * \param trigger The trigger to fire.
   * \param args The arguments to pass in the transition.
   *
   * \throw error The current state does not allow the trigger to be fired.
   */
  template<typename... TArgs>
  void fire(
    const std::shared_ptr<trigger_with_parameters<TTrigger, TArgs...>>& trigger,
    TArgs... args)
  {
    internal_fire(trigger->trigger(), args...);
  }

  /**
   * Register a callback that will be invoked every time the state machine
   * transitions from one state into another.
   *
   * \param action The action to execute, accepting the details of the transition.
   */
  void on_transition(const TTransitionAction& action)
  {
    on_transition_ = action;
  }

  /**
   * Override the default behaviour of throwing an exception when an
   * unhandled trigger is fired.
   *
   * \param action An action to call when an unhandled trigger is fired.
   */
  void on_unhandled_trigger(const TUnhandledTriggerAction& action)
  {
    on_unhandled_trigger_ = action;
  }

  /**
   * Determine whether the state machine is in the supplied state.
   *
   * \param state The state to test for.
   *
   * \return True if the current state is equal to, or a substate of, the supplied state.
   */
  bool is_in_state(const TState& state)
  {
    return current_representation()->is_included_in(state);
  }

  /**
   * Determine whether supplied trigger can be fired in the current state.
   *
   * \param trigger Trigger to test.
   *
   * \return True if the trigger can be fired, false otherwise.
   */
  bool can_fire(const TTrigger& trigger) const
  {
    return current_representation()->can_handle(trigger);
  }

  /**
   * Specify the arguments that must be supplied when a specific trigger is fired.
   *
   * \param trigger The underlying trigger value.
   *
   * \return An object that can be passed to the Fire() method in order to 
   *         fire the parameterised trigger.
   */
  template<typename... TArgs>
  std::shared_ptr<trigger_with_parameters<TTrigger, TArgs...>>
  set_trigger_parameters(const TTrigger& trigger)
  {
    auto it = trigger_configuration_.find(trigger);
    if (it != trigger_configuration_.end())
    {
      throw error("Cannot reconfigure trigger parameters");
    }
    auto configuration =
      std::make_shared<trigger_with_parameters<TTrigger, TArgs...>>(trigger);
    trigger_configuration_[trigger] = configuration;
    return configuration;
  }

  /**
   * The currently permissible trigger values.
   */
  std::set<TTrigger> permitted_triggers() const
  {
    return current_representation()->permitted_triggers();
  }

  /**
   * A human readable representation of the state machine.
   *
   * \return A description of the current state and permitted triggers.
   */
  std::string print() const
  {
    std::ostringstream oss;
    print(oss);
    return oss.str();
  }

  /**
   * Stream output operator.
   */
  friend inline std::ostream& operator<<(
    std::ostream& os, const stateless::state_machine<TState, TTrigger>& sm)
  {
    sm.print(os);
    return os;
  }

private:
  /**
   * Wrapper class for internal state storage.
   */
  class state_reference
  {
  public:
    state_reference(const TState& initial_state)
      : state_(initial_state)
    {}

    const TState& get() const
    {
      return state_;
    }

    void set(const TState& new_state)
    {
      state_ = new_state;
    }

  private:
    TState state_;
  };

  /**
   * Perform initialization.
   *
   * \param state_accessor A function that will be called to read the current state value.
   * \param state_mutator  An action that will be called to write new state values.
   */
  void init(
    const TStateAccessor& state_accessor,
    const TStateMutator& state_mutator)
  {
    state_accessor_ = state_accessor;
    state_mutator_ = state_mutator;
    on_unhandled_trigger_ = [](const TState& state, const TTrigger& trigger)
    {
      throw error(
        "No valid leaving transitions are permitted for trigger. "
        "Consider ignoring the trigger.");
    };
  }

  /// Parameterized state representation type.
  typedef detail::state_representation<TState, TTrigger> TStateRepresentation;

  /// The current representation.
  const TStateRepresentation* current_representation() const
  {
    return get_representation(state());
  }

  /// Get the representation corresponding to the supplied state.
  TStateRepresentation* get_representation(const TState& state) const
  {
    auto it = state_configuration_.find(state);
    if (it == state_configuration_.end())
    {
      TStateRepresentation representation(state);
      auto inserted = state_configuration_.insert(
        std::make_pair(state, representation));
      return &inserted.first->second;
    }
    return &it->second;
  }

  /// Set the state.
  void set_state(const TState& new_state)
  {
    state_mutator_(new_state);
  }

  /// Implementation of state transition given a trigger.
  template<typename... TArgs>
  void internal_fire(const TTrigger& trigger, TArgs... args)
  {
    auto abstract_configuration = trigger_configuration_.find(trigger);
    if (abstract_configuration != trigger_configuration_.end())
    {
      typedef trigger_with_parameters<TTrigger, TArgs...> TParameterizedTrigger;
      auto configuration =
        std::dynamic_pointer_cast<TParameterizedTrigger>(
          abstract_configuration->second);
      if (configuration == nullptr)
      {
        throw error("Invalid number or type of parameters.");
      }
    }

    auto abstract_handler = current_representation()->try_find_handler(trigger);
    if (abstract_handler == nullptr)
    {
      on_unhandled_trigger_(
        current_representation()->underlying_state(), trigger);
      return;
    }

    const auto& source = state();
    TState destination;
    bool is_transition = false;

    typedef detail::dynamic_trigger_behaviour<TState, TTrigger, TArgs...> TDynamicTriggerBehaviour;
    typedef detail::trigger_behaviour<TState, TTrigger> TTriggerBehaviour;
    if (auto handler = std::dynamic_pointer_cast<TDynamicTriggerBehaviour>(abstract_handler))
    {
      // A dynamic behaviour is configured, so forward the arguments to it.
      is_transition = handler->results_in_transition_from(source, destination, args...);
    }
    else if (auto handler = std::dynamic_pointer_cast<TTriggerBehaviour>(abstract_handler))
    {
      // Fall back to configuration time defined transition.
      is_transition = handler->results_in_transition_from(source, destination);
    }
    else
    {
      throw error("Unable to find a suitable handler.");
    }

    if (is_transition)
    {
      TTransition transition(source, destination, trigger);
      current_representation()->exit(transition);
      set_state(transition.destination());
      current_representation()->enter(transition, args...);
      if (on_transition_)
      {
        on_transition_(transition);
      }
    }
  }

  /// Implementation for public print and stream operator.
  void print(std::ostream& os) const
  {
    auto print_permitted_triggers =
      [&](const std::set<TTrigger>& pts)
      {
        bool first = true;
        for (auto& pt : pts)
        {
          if (!first) os << ", ";
          first = false;
          print_trigger<TTrigger>(os, pt);
        }
      };
    os << "state_machine { state = ";
    print_state<TState>(os, state());
    os << ", permitted triggers = { ";
    print_permitted_triggers(permitted_triggers());
    os << " } }";
  }

  /**
   * Mapping from state to representation.
   * There is exactly one representation per configured state.
   */
  mutable std::map<TState, TStateRepresentation> state_configuration_;

  /// Mapping of triggers with arguments to the underlying trigger.
  std::map<TTrigger, TTriggerWithParameters> trigger_configuration_;

  /// The state accessor.
  TStateAccessor state_accessor_;

  /// The state mutator.
  TStateMutator state_mutator_;

  /// Function to call on unhandled trigger.
  TUnhandledTriggerAction on_unhandled_trigger_;

  /// Function to call on state transition.
  TTransitionAction on_transition_;
};

}

#endif // STATELESS_STATE_MACHINE_HPP

