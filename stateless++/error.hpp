
#ifndef STATELESS_ERROR_HPP
#define STATELESS_ERROR_HPP

#include <stdexcept>

namespace stateless
{

/**
 * Represents an error caused by state machine configuration errors.
 */
class error : public std::runtime_error
{
public:
	error(const char* what)
		: std::runtime_error(what)
	{}
};

}

#endif // STATELESS_ERROR_HPP
