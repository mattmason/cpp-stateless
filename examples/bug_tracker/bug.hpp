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

#include <state_machine.hpp>

#include <string>

namespace bug_tracker_example
{

class bug
{
public:
  enum class state { open, assigned, deferred, resolved, closed };

  enum class trigger { open, assign, defer, resolve, close };
  
  typedef stateless::state_machine<state, trigger> TStateMachine;

  typedef TStateMachine::TTransition TTransition;

  bug(const std::string& title);

  void close();

  void assign(const std::string& assignee);

  bool can_assign() const;

  void defer();

  void resolve(const std::string& assignee);
  
  void open();

  const state& get_state() const;

private:
  void on_assigned(const bug::TTransition& transition, const std::string& assignee);

  void on_resolved(const bug::TTransition& transition, const std::string& assignee);

  void on_deassigned();

  void send_email_to_assignee(const std::string& message);

  void set_state(const state& new_state);

  state state_;
  std::string title_;
  std::shared_ptr<std::string> assignee_;

  TStateMachine state_machine_;

  typedef std::shared_ptr<stateless::trigger_with_parameters<trigger, std::string>> TAssignTrigger;
  TAssignTrigger assign_trigger_;
  TAssignTrigger resolve_trigger_;
};

}
