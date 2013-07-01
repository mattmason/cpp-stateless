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

#include <stateless++/state_machine.hpp>

#include <state.hpp>
#include <trigger.hpp>

#include <gtest/gtest.h>

using namespace stateless;
using namespace testing;

namespace
{

#ifdef _WIN32
typedef state_machine<state, trigger> TStateMachine;
#else
using TStateMachine = state_machine<state, trigger>;
#endif

TEST(StateMachine, WhenFireTrigger_ThenTransitionsToConfiguredDestinationState)
{
  const std::string state_a("A"), state_b("B"), state_c("C");
  const std::string trigger_x("X"), trigger_y("Y");

  state_machine<std::string, std::string> sm(state_a);

  sm.configure(state_a).permit(trigger_x, state_b);

  sm.fire(trigger_x);

  ASSERT_EQ(state_b, sm.state());
}

TEST(StateMachine, WhenConstructed_ThenInitialStateIsCurrent)
{
  TStateMachine sm(state::B);
  ASSERT_EQ(state::B, sm.state());
}

TEST(StateMachine, WhenStateIsStoredExternally_ThenItIsRetrieved)
{
  state s = state::B;
  TStateMachine sm([&](){ return s; }, [&](const state& new_s){ s = new_s; });

  sm.configure(state::B).permit(trigger::X, state::C);

  ASSERT_EQ(state::B, sm.state());

  sm.fire(trigger::X);

  ASSERT_EQ(state::C, sm.state());
}

TEST(StateMachine, WhenSubstate_ThenItIsIncludedInCurrentState)
{
  TStateMachine sm(state::B);
  sm.configure(state::B).sub_state_of(state::C);

  ASSERT_EQ(state::B, sm.state());
  ASSERT_TRUE(sm.is_in_state(state::C));
}

TEST(StateMachine, WhenInSubstate_ThenTriggerIgnoredInSuperstateRemainsInSubstate)
{
  TStateMachine sm(state::B);
  sm.configure(state::B).sub_state_of(state::C);
  sm.configure(state::C).ignore(trigger::X);
  sm.fire(trigger::X);

  ASSERT_EQ(state::B, sm.state());
}

TEST(StateMachine, WhenPermittedTriggersIncludeSuperstate_ThenPermittedTriggersAreIncluded)
{
  TStateMachine sm(state::B);
  sm.configure(state::A).permit(trigger::Z, state::B);
  sm.configure(state::B).sub_state_of(state::C).permit(trigger::X, state::A);
  sm.configure(state::C).permit(trigger::Y, state::A);
  
  auto permitted = sm.permitted_triggers();
  
  EXPECT_NE(permitted.end(), permitted.find(trigger::X));
  EXPECT_NE(permitted.end(), permitted.find(trigger::Y));
  EXPECT_EQ(permitted.end(), permitted.find(trigger::Z));
}

TEST(StateMachine, WhenPermittedTriggers_ThenTheyAreDistinctValues)
{
  TStateMachine sm(state::B);
  sm.configure(state::B).sub_state_of(state::C).permit(trigger::X, state::A);
  sm.configure(state::C).permit(trigger::X, state::B);
  
  auto permitted = sm.permitted_triggers();
  
  EXPECT_EQ(1, permitted.size());
  EXPECT_EQ(trigger::X, *permitted.begin());
}

TEST(StateMachine, WhenPermittedTriggerIncludesGuard_ThenGuardIsRespected)
{
  TStateMachine sm(state::B);
  sm.configure(state::B).permit_if(trigger::X, state::A, [](){ return false; });
  
  EXPECT_EQ(0, sm.permitted_triggers().size());
}

TEST(StateMachine, WhenDiscriminatedByGuard_ThenChoosesPermittedTransition)
{
  TStateMachine sm(state::B);
  sm.configure(state::B)
    .permit_if(trigger::X, state::A, [](){ return false; })
    .permit_if(trigger::X, state::C, [](){ return true; });
  sm.fire(trigger::X);
  
  ASSERT_EQ(state::C, sm.state());
}

TEST(StateMachine, WhenTriggerIsIgnored_ThenActionsAreNotExecuted)
{
  TStateMachine sm(state::B);

  bool fired = false;
  sm.configure(state::B)
    .on_entry([&](const TStateMachine::TTransition&){ fired = true; })
    .ignore(trigger::X);
  sm.fire(trigger::X);

  ASSERT_FALSE(fired);
}

TEST(StateMachine, WhenSelfTransitionPermited_ThenActionsFire)
{
  TStateMachine sm(state::B);

  bool fired = false;
  sm.configure(state::B)
    .on_entry([&](const TStateMachine::TTransition&){ fired = true; })
    .permit_reentry(trigger::X);
  sm.fire(trigger::X);

  ASSERT_TRUE(fired);
}

TEST(StateMachine, WhenImplicitReentryIsAttempted_ThenErrorIsRaised)
{
  TStateMachine sm(state::B);
  ASSERT_THROW(
    sm.configure(state::B).permit(trigger::X, state::B),
    stateless::error);
}

TEST(StateMachine, WhenTriggerParametersAreSet_ThenTheyAreImmutable)
{
  TStateMachine sm(state::B);
  sm.set_trigger_parameters<std::string, int>(trigger::X);

  ASSERT_THROW(
    sm.set_trigger_parameters<std::string>(trigger::X),
    stateless::error);
}

TEST(StateMachine, WhenParametersSuppliedToFire_ThenTheyArePassedToEntryAction)
{
  TStateMachine sm(state::B);
  auto x = sm.set_trigger_parameters<std::string, int>(trigger::X);
  sm.configure(state::B).permit(trigger::X, state::C);
  
  std::string assigned_string;
  int assigned_int = 0;
  sm.configure(state::C)
    .on_entry_from(
      x,
      [&](const TStateMachine::TTransition& transition, const std::string& s, int i)
      {
        assigned_string = s;
        assigned_int = i;
      });
      
  const std::string supplied_string = "something";
  const int supplied_int = 42;
  
  sm.fire(x, supplied_string, supplied_int);
  
  ASSERT_EQ(supplied_string, assigned_string);
  ASSERT_EQ(supplied_int, assigned_int);
}

TEST(StateMachine, WhenUnhandledTriggerIsFired_ThenTheProvidedHandlerIsCalledWithStateAndTrigger)
{
  TStateMachine sm(state::B);
  
  state unhandled_state = state::A;
  trigger unhandled_trigger = trigger::X;
  
  sm.on_unhandled_trigger(
    [&](const state& s, const trigger& t)
    {
      unhandled_state = s;
      unhandled_trigger = t;
    });
    
  sm.fire(trigger::Z);
  
  ASSERT_EQ(state::B, unhandled_state);
  ASSERT_EQ(trigger::Z, unhandled_trigger);
}

TEST(StateMachine, WhenTransitionOccurs_ThenTheOnTransitionEventFires)
{
  TStateMachine sm(state::B);

  sm.configure(state::B).permit(trigger::X, state::A);
  sm.on_transition(
    [](const TStateMachine::TTransition& t)
    {
      ASSERT_EQ(trigger::X, t.trigger());
      ASSERT_EQ(state::B, t.source());
      ASSERT_EQ(state::A, t.destination());
    });
    
    sm.fire(trigger::X);
}

}
