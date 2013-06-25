
#ifndef STATELESS_TRIGGER_BEHAVIOUR_HPP
#define STATELESS_TRIGGER_BEHAVIOUR_HPP

#include <functional>

namespace stateless
{

namespace detail
{

template<typename TState, typename TTrigger>
class trigger_behaviour
{
public:
  typedef std::function<bool()> TGuard;
  typedef std::function<bool(const TState&, TState&)> TDecision;

  trigger_behaviour(const TTrigger& trigger, const TGuard& guard, const TDecision& decision)
    : trigger_(trigger)
    , guard_(guard)
    , decision_(decision)
  {}

  const TTrigger& trigger() const
  {
    return trigger_;
  }

  bool is_condition_met() const
  {
    return guard_();
  }

  const bool results_in_transition_from(const TState& source, TState& destination) const
  {
    return decision_(source, destination);
  }

private:
  const TTrigger trigger_;
  TGuard guard_;
  TDecision decision_;
};

}

}

#endif // STATELESS_TRIGGER_BEHAVIOUR_HPP
