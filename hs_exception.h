#pragma once

#include <string>
#include <exception>

class hs_exception : public std::exception
{
	public:
		explicit hs_exception(const std::string& what_arg):
			_what(what_arg)
		{}
		virtual const char* what() const noexcept
		{
			return _what.c_str();
		}

	private:
		std::string _what;
};
