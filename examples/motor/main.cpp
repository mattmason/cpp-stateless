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

// Example inspired by
// http://www.drdobbs.com/cpp/state-machine-design-in-c/184401236#.

#include <stateless++/state_machine.hpp>

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

using namespace stateless;
using namespace std::placeholders;

namespace
{

enum class state { idle, stopped, started, running };

std::ostream& operator<<(std::ostream& os, const state& s)
{
  static const char* name[] = { "idle", "stopped", "started", "running" };
  os << name[(int)s];
  return os;
}

enum class trigger { start, stop, set_speed, halt };

std::ostream& operator<<(std::ostream& os, const trigger& t)
{
  static const char* name[] = { "start", "stop", "set_speed", "halt" };
  os << name[(int)t];
  return os;
}

class motor
{
public:
  motor()
    : sm_(state::idle)
    , set_speed_trigger_(sm_.set_trigger_parameters<int>(trigger::set_speed))
    , speed_(0)
  {
    sm_.configure(state::idle)
      .permit(trigger::start, state::started);

    sm_.configure(state::stopped)
      .permit(trigger::halt, state::idle);

    sm_.configure(state::started)
      .on_entry_from(
        set_speed_trigger_,
        [=](const TTransition& t, int speed) { speed_ = speed; })
      .permit(trigger::set_speed, state::running)
      .permit(trigger::stop, state::stopped);

    sm_.configure(state::running)
      .on_entry_from(
        set_speed_trigger_,
        [=](const TTransition& t, int speed) { speed_ = speed; })
      .permit(trigger::stop, state::stopped)
      .permit_reentry(trigger::set_speed);

    // Register a callback for state transitions (the default does nothing).
    sm_.on_transition([](const TTransition& t)
      {
        std::cout << "transition from [" << t.source() << "] to ["
          << t.destination() << "] via trigger [" << t.trigger() << "]"
          << std::endl;
      });

    // Override the default behaviour of throwing when a trigger is unhandled.
    sm_.on_unhandled_trigger([](const state& s, const trigger& t)
      {
        std::cerr << "ignore unhandled trigger [" << t << "] in state [" << s
          << "]" << std::endl;
      });
  }

  void start(int speed)
  {
    sm_.fire(trigger::start);
    set_speed(speed);
  }

  void stop()
  {
    set_speed(0);
    sm_.fire(trigger::stop);
    sm_.fire(trigger::halt);
  }

  void set_speed(int speed)
  {
    sm_.fire(set_speed_trigger_, speed);
  }

  void print(std::ostream& os) const
  {
    os << print();
  }

  std::string print() const
  {
    std::ostringstream oss;
    oss << "Motor in state [" << sm_.state() << "] speed = " << speed_;
    return oss.str();
  }

private:
  typedef state_machine<state, trigger> TStateMachine;
  typedef TStateMachine::TTransition TTransition;
  typedef std::shared_ptr<stateless::trigger_with_parameters<trigger, int>> TSetSpeedTrigger;

  TStateMachine sm_;
  TSetSpeedTrigger set_speed_trigger_;
  int speed_;
};

std::ostream& operator<<(std::ostream& os, const motor& m)
{
  m.print(os);
  return os;
}

}

int main(int argc, char* argv[])
{
  motor m;
  std::cout << m << std::endl;
  m.start(10);
  std::cout << m << std::endl;
  m.set_speed(20);
  std::cout << m << std::endl;
  m.stop();
  std::cout << m << std::endl;
  m.stop();
  std::cout << m << std::endl;

  std::cout << "Press enter to quit..." << std::endl;
  char c;
  std::cin.get(c);

  return EXIT_SUCCESS;
}

