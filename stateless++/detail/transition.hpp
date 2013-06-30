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

#ifndef STATELESS_DETAIL_TRANSITION_HPP
#define STATELESS_DETAIL_TRANSITION_HPP

namespace stateless
{

namespace detail
{

template<typename TState, typename TTrigger>
class transition
{
public:
  transition(
    const TState& source,
    const TState& destination,
    const TTrigger& trigger)
    : source_(source), destination_(destination), trigger_(trigger)
  {}

  const TState& source() const { return source_; }

  const TState& destination() const { return destination_; }

  const TTrigger& trigger() const { return trigger_; }

  bool is_reentry() const { return source_ == destination_; }
      
private:
  const TState source_;
  const TState destination_;
  const TTrigger trigger_;
};

}

}

#endif // STATELESS_DETAIL_TRANSITION_HPP
