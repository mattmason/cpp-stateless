
#ifndef STATELESS_TRANSITION_HPP
#define STATELESS_TRANSITION_HPP

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
	const TTrigger& trigger_;
};

}

}

#endif // STATELESS_TRANSITION_HPP
