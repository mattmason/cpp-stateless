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

#include <stateless++/detail/trigger_behaviour.hpp>

#include <state.hpp>
#include <trigger.hpp>

#include <gtest/gtest.h>

using namespace stateless::detail;
using namespace testing;

namespace
{

#ifdef _WIN32
typedef trigger_behaviour<state, trigger> TTB;
#else
using TTB = trigger_behaviour<state, trigger>;
#endif

TEST(TriggerBehaviour, WhenUnderlyingTriggerIsSet_ThenItIsExposed)
{
  TTB trigger_behaviour(
    trigger::X,
    []() { return true; },
    [](const state& source, state& destination) { return true; });
  ASSERT_EQ(trigger::X, trigger_behaviour.trigger());
}

TEST(TriggerBehaviour, WhenGuardConditionFalse_ThenIsGuardConditionMetIsFalse)
{
  TTB trigger_behaviour(
    trigger::X,
    []() { return false; },
    [](const state& source, state& destination) { return true; });
  ASSERT_FALSE(trigger_behaviour.is_condition_met());
}

TEST(TriggerBehaviour, WhenGuardConditionTrue_ThenIsGuardConditionMetIsTrue)
{
  TTB trigger_behaviour(
    trigger::X,
    []() { return true; },
    [](const state& source, state& destination) { return true; });
  ASSERT_TRUE(trigger_behaviour.is_condition_met());
}

}
