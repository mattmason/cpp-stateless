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

#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <string>

namespace
{

enum class state
{
  off_hook,
  ringing,
  connected,
  on_hold,
  phone_destroyed
};

const char* state_name[] = 
{
  "off_hook",
  "ringing",
  "connected",
  "on_hold",
  "phone_destroyed"
};

std::ostream& operator<<(std::ostream& os, const state& s)
{
  os << state_name[(int)s];
  return os;
}

enum class trigger
{
  call_dialled,
  hung_up,
  call_connected,
  left_message,
  placed_on_hold,
  taken_off_hold,
  phone_hurled_against_wall
};

const char* trigger_name[] = 
{
  "call_dialled",
  "hung_up",
  "call_connected",
  "left_message",
  "placed_on_hold",
  "taken_off_hold",
  "phone_hurled_against_wall"
};

std::ostream& operator<<(std::ostream& os, const trigger& t)
{
  os << trigger_name[(int)t];
  return os;
}

}

namespace stateless
{

template<> void print_state<state>(std::ostream& os, const state& s)
{ os << s; }

template<> void print_trigger<trigger>(std::ostream& os, const trigger& t)
{ os << t; }

}

namespace
{

using namespace stateless;

#if _WIN32
using std::put_time;
#else
// No std::put_time on gcc4.7 / cygwin.
std::string put_time(std::tm* t, const char* fmt)
{
  std::time_t now = std::time(nullptr);
  std::tm* info = std::gmtime(&now);
  char buffer[80] = { 0 };
  std::strftime(buffer, 80, fmt, info);
  return buffer;
}
#endif

void start_call_timer()
{
  std::time_t now = std::time(nullptr);
  std::cout << "Call started at " << put_time(std::gmtime(&now), "%c") << std::endl;
}

void stop_call_timer()
{
  std::time_t now = std::time(nullptr);
  std::cout << "Call ended at " << put_time(std::gmtime(&now), "%c") << std::endl;
}

void fire(state_machine<state, trigger>& sm, const trigger& trigger, std::ostream& os = std::cout)
{
  os << "Firing [" << trigger_name[(int)trigger] << "]" << std::endl;
  sm.fire(trigger);
}

}

int main(int argc, char* argv[])
{
  state_machine<state, trigger> phone_call(state::off_hook);

  phone_call.configure(state::off_hook)
      .permit(trigger::call_dialled, state::ringing);
              
    phone_call.configure(state::ringing)
    .permit(trigger::hung_up, state::off_hook)
    .permit(trigger::call_connected, state::connected);
             
    phone_call.configure(state::connected)
    .on_entry(std::bind(start_call_timer))
        .on_exit(std::bind(stop_call_timer))
    .permit(trigger::left_message, state::off_hook)
      .permit(trigger::hung_up, state::off_hook)
    .permit(trigger::placed_on_hold, state::on_hold);

  phone_call.configure(state::on_hold)
    .sub_state_of(state::connected)
    .permit(trigger::taken_off_hold, state::connected)
    .permit(trigger::hung_up, state::off_hook)
    .permit(trigger::phone_hurled_against_wall, state::phone_destroyed);

  std::cout << phone_call << std::endl;
    fire(phone_call, trigger::call_dialled);
  std::cout << phone_call << std::endl;
    fire(phone_call, trigger::call_connected);
  std::cout << phone_call << std::endl;
    fire(phone_call, trigger::placed_on_hold);
  std::cout << phone_call << std::endl;
    fire(phone_call, trigger::taken_off_hold);
  std::cout << phone_call << std::endl;
    fire(phone_call, trigger::hung_up);
  std::cout << phone_call << std::endl;

  std::cout << "Press enter to quit..." << std::endl;
  char c;
  std::cin.get(c);

  return EXIT_SUCCESS;
}
