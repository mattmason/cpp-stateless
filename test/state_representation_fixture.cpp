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

#include <stateless++/detail/state_representation.hpp>

#include <stateless++/detail/transition.hpp>
#include <stateless++/detail/trigger_behaviour.hpp>

#include <state.hpp>
#include <trigger.hpp>

#include <gtest/gtest.h>

using namespace stateless::detail;
using namespace testing;

namespace
{

#ifdef _WIN32
typedef state_representation<state, trigger> TSR;
typedef transition<state, trigger> TTransition;
typedef trigger_behaviour<state, trigger> TTB;
#else
using TSR = state_representation<state, trigger>;
using TTransition = transition<state, trigger>;
using TTB = trigger_behaviour<state, trigger>;
#endif

TEST(StateRepresentation, WhenEntering_ThenEnteringActionsExecute)
{
  TSR sr(state::B);
  TTransition t(state::A, state::B, trigger::X);
  bool executed = false;
  sr.add_entry_action([&](const TTransition&){ executed = true; });
  sr.enter(t);
  ASSERT_TRUE(executed);
}

TEST(StateRepresentation, WhenLeaving_ThenEnteringActionsDoNotExecute)
{
  TSR sr(state::B);
  TTransition t(state::A, state::B, trigger::X);
  bool executed = false;
  sr.add_entry_action([&](const TTransition&){ executed = true; });
  sr.exit(t);
  ASSERT_FALSE(executed);  
}

TEST(StateRepresentation, WhenLeaving_ThenLeavingActionsExecute)
{
  TSR sr(state::A);
  TTransition t(state::A, state::B, trigger::X);
  bool executed = false;
  sr.add_exit_action([&](const TTransition&){ executed = true; });
  sr.exit(t);
  ASSERT_TRUE(executed);    
}

TEST(StateRepresentation, WhenEntering_ThenLeavingActionsDoNotExecute)
{
  TSR sr(state::A);
  TTransition t(state::A, state::B, trigger::X);
  bool executed = false;
  sr.add_exit_action([&](const TTransition&){ executed = true; });
  sr.enter(t);

  ASSERT_FALSE(executed);    
}

TEST(StateRepresentation, WhenSetup_ThenIncludesUnderlyingState)
{
  TSR sr(state::B);

  ASSERT_TRUE(sr.includes(state::B));
}

TEST(StateRepresentation, WhenSetup_ThenDoesNotIncludeUnrelatedState)
{
  TSR sr(state::B);

  ASSERT_FALSE(sr.includes(state::C));
}

TEST(StateRepresentation, WhenSubstate_ThenIncludesSubstate)
{
  TSR sr_b(state::B);
  TSR sr_c(state::C);
  sr_b.add_sub_state(&sr_c);

  ASSERT_TRUE(sr_b.includes(state::C));
}

TEST(StateRepresentation, WhenSuperstate_ThenDoesNotIncludeSuperstate)
{
  TSR sr_b(state::B);
  TSR sr_c(state::C);
  sr_b.set_super_state(&sr_c);

  ASSERT_FALSE(sr_b.includes(state::C));
}

TEST(StateRepresentation, WhenSetup_ThenIsIncludedInUnderlyingState)
{
  TSR sr(state::B);
  
  ASSERT_TRUE(sr.is_included_in(state::B));
}

TEST(StateRepresentation, WhenSetup_ThenIsNotIncludedInUnrelatedState)
{
  TSR sr(state::B);
  
  ASSERT_FALSE(sr.is_included_in(state::C));
}

TEST(StateRepresentation, WhenSubstate_ThenIsNotIncludedInSubstate)
{
  TSR sr_b(state::B);
  TSR sr_c(state::C);
  sr_b.add_sub_state(&sr_c);
  
  ASSERT_FALSE(sr_b.is_included_in(state::C));
}

TEST(StateRepresentation, WhenSuperstate_ThenIsIncludedInSuperstate)
{
  TSR sr_b(state::B);
  TSR sr_c(state::C);
  sr_b.set_super_state(&sr_c);
  
  ASSERT_TRUE(sr_b.is_included_in(state::C));
}

#define CREATE_SUPER_SUB_PAIR() \
  TSR super(state::A), sub(state::B); \
  super.add_sub_state(&sub); \
  sub.set_super_state(&super);

TEST(StateRepresentation, WhenTransitioningFromSubToSuperstate_ThenSubstateEntryActionsExecute)
{
  CREATE_SUPER_SUB_PAIR();
  bool executed = false;
  sub.add_entry_action([&](const TTransition&){ executed = true; });
  TTransition transition(super.underlying_state(), sub.underlying_state(), trigger::X);
  sub.enter(transition);
    
  ASSERT_TRUE(executed);
}

TEST(StateRepresentation, WhenTransitioningFromSubToSuperstate_ThenSubstateExitActionsExecute)
{
  CREATE_SUPER_SUB_PAIR();
  bool executed = false;
  sub.add_exit_action([&](const TTransition&){ executed = true; });
  TTransition transition(sub.underlying_state(), super.underlying_state(), trigger::X);
  sub.exit(transition);
    
  ASSERT_TRUE(executed);
}

TEST(StateRepresentation, WhenTransitioningToSuperFromSubstate_ThenSuperEntryActionsDoNotExecute)
{
  CREATE_SUPER_SUB_PAIR();
  bool executed = false;
  super.add_entry_action([&](const TTransition&){ executed = true; });
  TTransition transition(super.underlying_state(), sub.underlying_state(), trigger::X);
  super.enter(transition);
    
  ASSERT_FALSE(executed);
}

TEST(StateRepresentation, WhenTransitioningFromSuperToSubstate_ThenSuperExitActionsDoNotExecute)
{
  CREATE_SUPER_SUB_PAIR();
  bool executed = false;
  super.add_exit_action([&](const TTransition&){ executed = true; });
  TTransition transition(super.underlying_state(), sub.underlying_state(), trigger::X);
  super.exit(transition);
    
  ASSERT_FALSE(executed);
}

TEST(StateRepresentation, WhenEnteringSubstate_ThenSuperEntryActionsExecute)
{
  CREATE_SUPER_SUB_PAIR();
  bool executed = false;
  super.add_entry_action([&](const TTransition&){ executed = true; });
  TTransition transition(state::C, sub.underlying_state(), trigger::X);
  sub.enter(transition);
    
  ASSERT_TRUE(executed);
}

TEST(StateRepresentation, WhenLeavingSubstate_ThenSuperExitActionsExecute)
{
  CREATE_SUPER_SUB_PAIR();
  bool executed = false;
  super.add_exit_action([&](const TTransition&){ executed = true; });
  TTransition transition(sub.underlying_state(), state::C, trigger::X);
  sub.exit(transition);
    
  ASSERT_TRUE(executed);
}

TEST(StateRepresentation, WhenEntering_ThenEntryActionsExecuteInOrder)
{
  std::vector<int> actual;
  
  TSR sr(state::B);
  sr.add_entry_action([&](const TTransition&){ actual.push_back(0); });
  sr.add_entry_action([&](const TTransition&){ actual.push_back(1); });
  
  sr.enter(TTransition(state::A, state::B, trigger::X));
  
  ASSERT_EQ(2, actual.size());
  EXPECT_EQ(0, actual.at(0));
  EXPECT_EQ(1, actual.at(1));
}

TEST(StateRepresentation, WhenLeaving_ThenExitActionsExecuteInOrder)
{
  std::vector<int> actual;
  
  TSR sr(state::B);
  sr.add_exit_action([&](const TTransition&){ actual.push_back(0); });
  sr.add_exit_action([&](const TTransition&){ actual.push_back(1); });
  
  sr.exit(TTransition(state::B, state::C, trigger::X));
  
  ASSERT_EQ(2, actual.size());
  EXPECT_EQ(0, actual.at(0));
  EXPECT_EQ(1, actual.at(1));
}

TEST(StateRepresentation, WhenTransitionExists_ThenTriggerCanBeFired)
{
  TSR sr(state::B);
  auto tb = std::make_shared<TTB>(
    trigger::X, [](){ return true; }, [](const state&, state&){ return false; });
  sr.add_trigger_behaviour(trigger::X, tb);

  ASSERT_TRUE(sr.can_handle(trigger::X));
    
}

TEST(StateRepresentation, WhenTransitionDoesNotExist_ThenTriggerCannotBeFired)
{
  TSR sr(state::B);

  ASSERT_FALSE(sr.can_handle(trigger::X));
}

TEST(StateRepresentation, WhenTransitionExistsInSupersate_ThenTriggerCanBeFired)
{
  TSR sr_b(state::B);
  auto tb = std::make_shared<TTB>(
    trigger::X, [](){ return true; }, [](const state&, state&){ return false; });
  sr_b.add_trigger_behaviour(trigger::X, tb);
  TSR sub(state::C);
  sub.set_super_state(&sr_b);
  sr_b.add_sub_state(&sub);
  
  ASSERT_TRUE(sub.can_handle(trigger::X));
}

TEST(StateRepresentation, WhenEnteringSubstate_ThenSuperstateEntryActionsExecuteBeforeSubstate)
{
  CREATE_SUPER_SUB_PAIR();
  int order = 0, sub_order = 0, super_order = 0;
  super.add_entry_action([&](const TTransition&){ super_order = order++; });
  sub.add_entry_action([&](const TTransition&){ sub_order = order++; });
  TTransition transition(state::C, sub.underlying_state(), trigger::X);
  sub.enter(transition);

  ASSERT_LT(super_order, sub_order);
}

TEST(StateRepresentation, WhenExitingSubstate_ThenSubstateEntryActionsExecuteBeforeSuperstate)
{
  CREATE_SUPER_SUB_PAIR();
  int order = 0, sub_order = 0, super_order = 0;
  super.add_exit_action([&](const TTransition&){ super_order = order++; });
  sub.add_exit_action([&](const TTransition&){ sub_order = order++; });
  TTransition transition(sub.underlying_state(), state::C, trigger::X);
  sub.exit(transition);
  
  ASSERT_LT(sub_order, super_order);
}

}
