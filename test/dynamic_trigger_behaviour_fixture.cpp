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
using namespace stateless::detail;
using namespace testing;

namespace
{

#ifdef _WIN32
typedef state_machine<state, trigger> TStateMachine;
#else
using TStateMachine = state_machine<state, trigger>;
#endif

TEST(DynamicTriggerBehaviour, WhenPermitDynamic_ThenDestinationStateIsDynamic)
{
  TStateMachine sm(state::A);
  sm.configure(state::A)
    .permit_dynamic(trigger::X, [](){ return state::B; });

  sm.fire(trigger::X);

  EXPECT_EQ(state::B, sm.state());
}

TEST(DynamicTriggerBehaviour, WhenTriggerHasParameters_ThenTheyAreUsedToCalculateDestinationState)
{
#if 0
  var sm = new StateMachine<State, Trigger>(State.A);
  var trigger = sm.SetTriggerParameters<int>(Trigger.X);
  sm.Configure(State.A)
      .PermitDynamic(trigger, i => i == 1 ? State.B : State.C);

  sm.Fire(trigger, 1);

  Assert.AreEqual(State.B, sm.State);
#endif
  TStateMachine sm(state::A);
  auto pt = sm.set_trigger_parameters<int>(trigger::X);
  sm.configure(state::A)
    .permit_dynamic(pt, [](const int i){ return i == 1 ? state::B : state::C; });

  sm.fire(pt, 1);

  EXPECT_EQ(state::B, sm.state());
}

}
