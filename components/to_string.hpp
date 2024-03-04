#ifndef TO_STRING_HPP
#define TO_STRING_HPP

#pragma once

#include <sstream>

template <typename T>
inline std::string to_string( const T& value ) {
	std::stringstream	ss;
	ss << value;
	return ss.str();
}

#endif /* TO_STRING_HPP */