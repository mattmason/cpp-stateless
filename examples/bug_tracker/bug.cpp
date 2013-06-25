
#include "bug.hpp"

#include <iostream>

namespace bug_tracker_example
{

using namespace stateless;
using namespace std::placeholders;

bug::bug(const std::string& title)
	: state_(state::open)
	, title_(title)
	, assignee_()
	, state_machine_(std::bind(&bug::get_state, this), std::bind(&bug::set_state, this, _1))
	, assign_trigger_()
	, resolve_trigger_()
{
	assign_trigger_ = state_machine_.set_trigger_parameters<std::string>(trigger::assign);
	resolve_trigger_ = state_machine_.set_trigger_parameters<std::string>(trigger::resolve);

	state_machine_.configure(state::open)
		.permit(trigger::assign, state::assigned);

	state_machine_.configure(state::assigned)
		.sub_state_of(state::open)
		.on_entry_from(assign_trigger_, std::bind(&bug::on_assigned, this, _1, _2))
		.permit_reentry(trigger::assign)
		.permit(trigger::resolve, state::resolved)
		.permit(trigger::close, state::closed)
		.permit(trigger::defer, state::deferred)
		.on_exit(std::bind(&bug::on_deassigned, this));

	state_machine_.configure(state::deferred)
		.on_entry([=](const TTransition& t){ assignee_.reset(); })
		.permit(trigger::assign, state::assigned);
		
	state_machine_.configure(state::resolved)
		.on_entry<std::string>(std::bind(&bug::on_resolved, this, _1, _2))
		.permit(trigger::close, state::closed)
		.permit(trigger::open, state::open);
	
	state_machine_.configure(state::closed)
		.permit(trigger::open, state::open);
}

void bug::close()
{
	state_machine_.fire(trigger::close);
}

void bug::assign(const std::string& assignee)
{
	state_machine_.fire(assign_trigger_, assignee);
}

bool bug::can_assign() const
{
	return state_machine_.can_fire(trigger::assign);
}

void bug::defer()
{
	state_machine_.fire(trigger::defer);
}

void bug::resolve(const std::string& assignee)
{
	state_machine_.fire(resolve_trigger_, assignee);
}

void bug::open()
{
	state_machine_.fire(trigger::open);
}

void bug::on_assigned(const bug::TTransition& transition, const std::string& assignee)
{
	if (assignee_ != nullptr && assignee != *assignee_)
	{
		send_email_to_assignee("Don't forget to help the new guy.");
	}
	assignee_ = std::make_shared<std::string>(assignee);
	send_email_to_assignee("You own it.");
}

void bug::on_resolved(const bug::TTransition& transition, const std::string& assignee)
{
	assignee_ = std::make_shared<std::string>(assignee);
	send_email_to_assignee("It's fixed and ready for test.");
}

void bug::on_deassigned()
{
	send_email_to_assignee("You're off the hook.");
}

void bug::send_email_to_assignee(const std::string& message)
{
	std::cout << "To: " << *assignee_ << " Re: " << title_ << std::endl
		<< "--" << std::endl << message << std::endl;
}

const bug::state& bug::get_state() const
{
	return state_;
}

void bug::set_state(const bug::state& new_state)
{
	state_ = new_state;
}

}
