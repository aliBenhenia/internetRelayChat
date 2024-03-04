#ifndef SPLIT_HPP
#define SPLIT_HPP

#pragma once

#include <sstream>
#include <string>
#include <vector>

inline std::pair<std::vector<std::string>, size_t>	split(const std::string input, char delimiter) {
	std::vector<std::string>	tokens;

	if (!input.empty()) {
		std::string				token;
		std::istringstream	tokenStream(input);

		while (getline(tokenStream, token, delimiter)) {
			tokens.push_back(token);
		}
	}

	return std::make_pair(tokens, tokens.size());
}

inline std::vector<std::string> split(const std::string input, const char* delimiter) {
	std::vector<std::string>	tokens;
	std::string						tmp;
	size_t pos = 0, prev = 0, delimiterSize = strlen(delimiter);

	if (!input.empty()) {
		while ((pos = input.find(delimiter, prev)) != std::string::npos) {
			tmp = input.substr(prev, pos - prev);
			if (!tmp.empty())
				tokens.push_back(tmp);
			prev = pos + delimiterSize;
		}

		tmp = input.substr(prev);
		if (!tmp.empty())
			tokens.push_back(tmp);
	}

	return tokens;
}

#endif /* SPLIT_HPP */