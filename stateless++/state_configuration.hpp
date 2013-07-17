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

#ifndef STATELESS_STATE_CONFIGURATION_HPP
#define STATELESS_STATE_CONFIGURATION_HPP

#include "detail/no_guard.hpp"
#include "detail/state_representation.hpp"
#include "detail/transition.hpp"
#include "trigger_with_parameters.hpp"

#include <functional>

namespace stateless
{

template<typename TState, typename TTrigger>
class state_machine;

/**
 * The configuration for a single state value.
 *
 * \tparam TState The type used to represent the states.
 * \tparam TTrigger The type used to represent the triggers that cause state transitions.
 */
template<typename TState, typename TTrigger>
class state_configuration
{
public:
  /// Parameterized state representation type.
  typedef detail::state_representation<TState, TTrigger> TStateRepresentation;

  /// Parameterized trigger behaviour type.
  typedef typename TStateRepresentation::TTriggerBehaviour TTriggerBehaviour;

  /// Parameterized trigger with parameters type.
  typedef std::shared_ptr<abstract_trigger_with_parameters<TTrigger>> TTriggerWithParameters;

  /// Parameterized transition type.
  typedef typename TStateRepresentation::TTransition TTransition;

  /// Entry action type.
  typedef typename TStateRepresentation::TEntryAction TEntryAction;

  /// Exit action type.
  typedef typename TStateRepresentation::TExitAction TExitAction;

  /// Signature for guard function.
  typedef std::function<bool()> TGuard;

  ///Signature for lookup function.
  typedef std::function<TStateRepresentation*(const TState&)> TLookup;

  /**
   * Accept the specified trigger and transition to the destination state.
   *
   * \param trigger The accepted trigger.
   * \param destination_state The state that the trigger will cause a transition to.
   *
   * \return This configuration object.
   */
  state_configuration& permit(
    const TTrigger& trigger, const TState& destination_state)
  {
    enforce_not_identity_transition(destination_state);
    return internal_permit(trigger, destination_state);
  }

  /**
   * Accept the specified trigger and transition to the destination state.
   *
   * \param trigger The accepted trigger.
   * \param destination_state The state that the trigger will cause a transition to.
   * \param guard Function that must return true in order for the trigger to be accepted.
   *
   * \return This configuration object.
   */
  state_configuration& permit_if(
    const TTrigger& trigger,
    const TState& destination_state,
    const TGuard& guard)
  {
    enforce_not_identity_transition(destination_state);
    return internal_permit_if(trigger, destination_state, guard);
  }

  /**
   * Accept the specified trigger, execute exit actions and re-execute entry actions.
   * Reentry behaves as though the configured state transitions to an identical sibling state.
   *
   * \param trigger The accepted trigger.
   *
   * \return This configuration object.
   *
   * \note Applies to the current state only. Will not re-execute superstate actions,
   * or cause actions to execute transitioning between super- and sub-states.
   */
  state_configuration& permit_reentry(const TTrigger& trigger)
  {
    return internal_permit(trigger, representation_->underlying_state());
  }

  /**
   * Accept the specified trigger, execute exit actions and re-execute entry actions.
   * Reentry behaves as though the configured state transitions to an identical sibling state.
   *
   * \param trigger The accepted trigger.
   * \param guard Function that must return true in order for the trigger to be accepted.
   *
   * \return This configuration object.
   *
   * \note Applies to the current state only. Will not re-execute superstate actions,
   * or cause actions to execute transitioning between super- and sub-states.
   */
  state_configuration& permit_reentry_if(
    const TTrigger& trigger, const TGuard& guard)
  {
    return internal_permit_if(
      trigger, representation_->underlying_state(), guard);
  }

  /**
   * Ignore the specified trigger when in the configured state.
   *
   * \param trigger The trigger to ignore.
   *
   * \return This configuration object.
   */
  state_configuration& ignore(const TTrigger& trigger)
  {
    return ignore_if(trigger, detail::no_guard);
  }

  /**
   * Ignore the specified trigger when in the configured state.
   *
   * \param trigger The trigger to ignore.
   * \param guard Function that must return true in order for the trigger to be accepted.
   *
   * \return This configuration object.
   */
  state_configuration& ignore_if(const TTrigger& trigger, const TGuard& guard)
  {
    auto decision =
      [=](const TState& source, TState& destination)
      {
        return false;
      };
    auto behaviour = std::make_shared<detail::trigger_behaviour<TState, TTrigger>>(
      trigger, guard, decision);
    representation_->add_trigger_behaviour(trigger, behaviour);
    return *this;
  }

  /**
   * Specify an action that will execute when transitioning into the configured state.
   *
   * \param entry_action Action to execute, providing details of the transition.
   *
   * \return This configuration object.
   */
  template<typename... TArgs, typename TCallable>
  state_configuration& on_entry(TCallable entry_action)
  {
    representation_->template add_entry_action<TCallable, TArgs...>(entry_action);
    return *this;
  }

  /**
   * Specify an action that will execute when transitioning into the configured state.
   *
   * \param trigger The trigger by which the state must be entered in order for the action to execute.
   * \param entry_action Action to execute, providing details of the transition.
   *
   * \return This configuration object.
   */
  template<typename... TArgs, typename TCallable>
  state_configuration& on_entry_from(
    const TTrigger& trigger, TCallable entry_action)
  {
    representation_->template add_entry_action<TCallable, TArgs...>(trigger, entry_action);
    return *this;
  }

  /**
   * Specify an action that will execute when transitioning into the configured state.
   *
   * \param trigger The trigger by which the state must be entered in order for the action to execute.
   * \param entry_action Action to execute, providing details of the transition.
   *
   * \return This configuration object.
   */
  template<typename... TArgs, typename TCallable>
  state_configuration& on_entry_from(
    const std::shared_ptr<trigger_with_parameters<TTrigger, TArgs...>>& trigger,
    TCallable entry_action)
  {
    representation_->template add_entry_action<TCallable, TArgs...>(trigger->trigger(), entry_action);
    return *this;
  }

  /**
   * Specify an action that will execute when transitioning from the configured state.
   *
   * \param exit_action Action to execute, providing details of the transition.
   *
   * \return This configuration object.
   */
  template<typename TCallable>
  state_configuration& on_exit(TCallable exit_action)
  {
    representation_->add_exit_action(exit_action);
    return *this;
  }

  /**
   * Set the superstate that the configured state is a substate of.
   *
   * Substates inherit the allowed transitions of their superstate.
   * When entering directly into a substate from outside of the superstate,
   * entry actions for the superstate are executed.
   * Likewise when leaving from the substate to outside the supserstate,
   * exit actions for the superstate will execute.
   *
   * \param superstate The superstate.
   *
   * \return This configuration object.
   */
  state_configuration& sub_state_of(const TState& super_state)
  {
    auto super_representation = lookup_(super_state);
    representation_->set_super_state(super_representation);
    super_representation->add_sub_state(representation_);
    return *this;
  }

  /**
   * Accept the specified trigger and transition to the destination state, calculated
   * dynamically by the supplied function.
   *
   * \param trigger The accepted trigger.
   * \param decision Function to calculate the state that the trigger will cause a transition to.
   *
   * \return This configuration object.
   */
  template<typename TCallable>
  state_configuration& permit_dynamic(const TTrigger& trigger, TCallable decision)
  {
    return this->template internal_permit_dynamic_if<>(
      trigger, detail::no_guard, decision);
  }

  /**
   * Accept the specified trigger and transition to the destination state, calculated
   * dynamically by the supplied function.
   *
   * \param trigger The accepted trigger.
   * \param decision Function to calculate the state that the trigger will cause a transition to.
   *
   * \return This configuration object.
   */
  template<typename... TArgs, typename TCallable>
  state_configuration& permit_dynamic(
    const std::shared_ptr<trigger_with_parameters<TTrigger, TArgs...>>& trigger,
    TCallable decision)
  {
    return this->template internal_permit_dynamic_if<TCallable, TArgs...>(
      trigger->trigger(), detail::no_guard, decision);
  }

  /**
   * Accept the specified trigger and transition to the destination state, calculated
   * dynamically by the supplied function.
   *
   * \param trigger The accepted trigger.
   * \param guard Function that must return true in order for the trigger to be accepted.
   * \param decision Function to calculate the state that the trigger will cause a transition to.
   *
   * \return This configuration object.
   */
  template<typename TCallable>
  state_configuration& permit_dynamic_if(const TTrigger& trigger, const TGuard& guard, TCallable decision)
  {
    return this->template internal_permit_dynamic_if<>(
      trigger, guard, decision);
  }

  /**
   * Accept the specified trigger and transition to the destination state, calculated
   * dynamically by the supplied function.
   *
   * \param trigger The accepted trigger.
   * \param guard Function that must return true in order for the trigger to be accepted.
   * \param decision Function to calculate the state that the trigger will cause a transition to.
   *
   * \return This configuration object.
   */
  template<typename... TArgs, typename TCallable>
  state_configuration& permit_dynamic_if(
    const std::shared_ptr<trigger_with_parameters<TTrigger, TArgs...>>& trigger,
    const TGuard& guard,
    TCallable decision)
  {
    return this->template internal_permit_dynamic_if<TCallable, TArgs...>(
      trigger->trigger(), guard, decision);
  }

private:
  friend state_machine<TState, TTrigger>;

  /**
   * Construct a configuration object for a single state.
   * Not for client use; configuration objects are created by the state_machine.
   */
  state_configuration(
    TStateRepresentation* representation, const TLookup& lookup)
    : representation_(representation)
    , lookup_(lookup)
  {}

  void enforce_not_identity_transition(const TState& destination)
  {
    if (destination == representation_->underlying_state())
    {
      throw error(
        "permit() (and permit_if()) require that the destination state is not "
        "equal to the source state. To accept a trigger without changing state, "
        "use either ignore() or permit_reentry().");
    }
  }

  state_configuration& internal_permit(
    const TTrigger& trigger, const TState& destination_state)
  {
    return internal_permit_if(
      trigger,
      destination_state,
      detail::no_guard);
  }

  state_configuration& internal_permit_if(
    const TTrigger& trigger,
    const TState& destination_state,
    const TGuard& guard)
  {
    auto decision =
      [=](const TState& source, TState& destination)
      -> bool
      {
        destination = destination_state;
        return true;
      };
    auto behaviour = std::make_shared<detail::trigger_behaviour<TState, TTrigger>>(
      trigger, guard, decision);
    representation_->add_trigger_behaviour(trigger, behaviour);
    return *this;
  }

  template<typename TCallable, typename... TArgs>
  state_configuration& internal_permit_dynamic_if(
    const TTrigger& trigger,
    const TGuard& guard,
    TCallable decision)
  {
    auto behaviour =
      std::make_shared<detail::dynamic_trigger_behaviour<TState, TTrigger, TArgs...>>(
        trigger, guard, decision);
    representation_->add_trigger_behaviour(trigger, behaviour);
    return *this;
  }

  TStateRepresentation* representation_;
  TLookup lookup_;
};

}

#endif // STATELESS_STATE_CONFIGURATION_HPP
