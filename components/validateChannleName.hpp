#include <string>

inline bool checkValidName(std::string name)
{
	if (name[0] != '#' || name.size() < 2 || name.size() > 50)
		return false;
	for (size_t i = 1; i < name.size(); i++) {
		if (!isalnum(name[i]) && name[i] != '-' && name[i] != '_')
			return false;
	}
	return true;
}