
#ifndef STATELESS_TRIGGER_WITH_PARAMETERS_HPP
#define STATELESS_TRIGGER_WITH_PARAMETERS_HPP

namespace stateless
{

template<typename TTrigger>
class abstract_trigger_with_parameters
{
public:
	abstract_trigger_with_parameters(const TTrigger& underlying_trigger)
		: underlying_trigger_(underlying_trigger)
	{}

	const TTrigger& trigger() const
	{
		return underlying_trigger_;
	}

private:
	const TTrigger& underlying_trigger_;
};

template<typename TTrigger, typename... TArgs>
class trigger_with_parameters : public abstract_trigger_with_parameters<TTrigger>
{
public:
	trigger_with_parameters(const TTrigger& underlying_trigger)
		: abstract_trigger_with_parameters<TTrigger>(underlying_trigger)
	{}
};

}

#endif // STATELESS_TRIGGER_WITH_PARAMETERS_HPP
