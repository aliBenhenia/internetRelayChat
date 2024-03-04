#ifndef VALIDATE_NICK_HPP
#define VALIDATE_NICK_HPP

#pragma once

#include <string>

#include <defines.hpp>

inline int	validateNickname(const std::string nickname) {
	if (!isalpha(nickname[0]) || nickname.empty() || nickname.size() > 15)
		return false;
	for (size_t i = 0; i < nickname.size(); i++) {
		if (!isalnum(nickname[i]) && nickname[i] != '_' && nickname[i] != '-')
			return false;
	}
	return true;
}

inline int	validateFullname(const std::string fullname) {
	int spaceCount = 0;
	if (fullname.empty() || fullname.size() > 10)
		return false;
	for (size_t i = 0; i < fullname.size(); i++) {
		if (fullname[i] == ' ')
			spaceCount++;
		if (!isalpha(fullname[i]) || spaceCount > 1)
			return false;
	}
	return true;
}

#endif