
#ifndef STATELESS_STATE_REPRESENTATION_HPP
#define STATELESS_STATE_REPRESENTATION_HPP

#include <vector>

#include "error.hpp"
#include "transition.hpp"
#include "trigger_behaviour.hpp"

namespace stateless
{

namespace detail
{

class abstract_entry_action
{
public:
	virtual ~abstract_entry_action() = 0;
};

inline abstract_entry_action::~abstract_entry_action()
{}

template<typename TTransition, typename... TArgs>
struct entry_action : public abstract_entry_action
{
	entry_action(const std::function<void(const TTransition&, TArgs...)>& action)
		: execute(action)
	{}
	
	std::function<void(const TTransition&, TArgs...)> execute;
};
	
template<typename TState, typename TTrigger>
class state_representation
{
public:
	typedef transition<TState, TTrigger> TTransition;
	typedef trigger_behaviour<TState, TTrigger> TTriggerBehaviour;
	typedef std::shared_ptr<abstract_entry_action> TEntryAction;
	typedef std::function<void(const TTransition&)> TExitAction;

	state_representation(const TState& state)
		: state_(state)
		, trigger_behaviours_()
		, entry_actions_()
		, exit_actions_()
		, super_state_(nullptr)
		, sub_states_()
	{}

	bool can_handle(const TTrigger& trigger) const
	{
		return try_find_handler(trigger) != nullptr;
	}

	const TTriggerBehaviour* try_find_handler(const TTrigger& trigger) const
	{
		auto handler = try_find_local_hander(trigger);
		if (handler == nullptr && super_state_ != nullptr)
		{
			handler = super_state_->try_find_handler(trigger);
		}
		return handler;
	}

	template<typename TCallable, typename... TArgs>
	void add_entry_action(TCallable action)
	{
		auto ea = std::make_shared<entry_action<TTransition, TArgs...>>(action);
		entry_actions_.push_back(ea);
	}

	template<typename TCallable, typename... TArgs>
	void add_entry_action(const TTrigger& trigger, TCallable action)
	{
		auto wrapper =
			[=](const TTransition& transition, TArgs... args)
			{
				if (transition.trigger() == trigger)
				{
					action(transition, args...);
				}
			};
		auto ea = std::make_shared<entry_action<TTransition, TArgs...>>(wrapper);
		entry_actions_.push_back(ea);
	}

	void add_exit_action(const TExitAction& exit_action)
	{
		exit_actions_.push_back(exit_action);
	}

	template<typename... TArgs>
	void enter(const TTransition& transition, TArgs... args) const
	{
		if (transition.is_reentry())
		{
			execute_entry_actions(transition, args...);
		}
		else if (!includes(transition.source()))
		{
			if (super_state_ != nullptr)
			{
				super_state_->enter(transition, args...);
			}
			execute_entry_actions(transition, args...);
		}
	}

	void exit(const TTransition& transition) const
	{
		if (transition.is_reentry())
		{
			execute_exit_actions(transition);
		}
		else if (!includes(transition.destination()))
		{
			if (super_state_ != nullptr)
			{
				super_state_->exit(transition);
			}
			execute_exit_actions(transition);
		}
	}

	void add_trigger_behaviour(const TTriggerBehaviour& trigger_behaviour)
	{
		auto& it = trigger_behaviours_[trigger_behaviour.trigger()];
		it.push_back(trigger_behaviour);
	}

	const state_representation& super_state() const
	{
		return *super_state_;
	}

	void set_super_state(const state_representation* super_state)
	{
		super_state_ = super_state;
	}

	const TState& underlying_state() const
	{
		return state_;
	}

	void add_sub_state(const state_representation* sub_state)
	{
		sub_states_.push_back(sub_state);
	}

	bool includes(const TState& state) const
	{
		if (state == state_)
		{
			return true;
		}
		for (const auto sub_state : sub_states_)
		{
			if (sub_state->includes(state))
			{
				return true;
			}
		}
		return false;
	}

	bool is_included_in(const TState& state) const
	{
		return (state == state() ||
			(super_state_ != nullptr && super_state_->is_included_in(state)));
	}

private:
	const TTriggerBehaviour* try_find_local_hander(const TTrigger& trigger) const
	{
		const TTriggerBehaviour* result = nullptr;

		const auto& candidates = trigger_behaviours_.find(trigger);
		if (candidates == trigger_behaviours_.end())
		{
			return result;
		}

		for (auto& candidate : candidates->second)
		{
			if (candidate.is_condition_met())
			{
				if (result != nullptr)
				{
					throw error(
						"Multiple permitted exit transitions are "
						"configured from the current state. Guard "
						"clauses must be mutually exclusive.");
				}
				result = &candidate;
			}
		}

		return result;
	}

	template<typename... TArgs>
	void execute_entry_actions(const TTransition& transition, TArgs... args) const
	{
		for (auto& action : entry_actions_)
		{
			auto ea = std::dynamic_pointer_cast<entry_action<TTransition, TArgs...>>(action);
			if (ea != nullptr)
			{
				ea->execute(transition, args...);
			}
			else
			{
				throw error("Invalid entry action");
			}
		}
	}

	void execute_exit_actions(const TTransition& transition) const
	{
		for (auto& action : exit_actions_)
		{
			action(transition);
		}
	}

	const TState state_;

	std::map<TTrigger, std::vector<TTriggerBehaviour>> trigger_behaviours_;
	std::vector<TEntryAction> entry_actions_;
	std::vector<TExitAction> exit_actions_;

	const state_representation* super_state_;
	std::vector<const state_representation*> sub_states_;
};

}

}

#endif // STATELESS_STATE_REPRESENTATION_HPP
