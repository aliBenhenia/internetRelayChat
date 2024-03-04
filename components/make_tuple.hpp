#ifndef MAKE_TUPLE
#define MAKE_TUPLE

#pragma once

#include <defines.hpp>
#include <string>

inline tuple make_tuple(RS_CODE code, CMD cmd, std::string message) {
	return tuple(code, cmd, message);
}

#endif