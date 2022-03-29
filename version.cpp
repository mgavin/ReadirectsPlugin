#include "version.h"
std::string get_version() {
	std::time_t t	 = std::time(nullptr);
	std::tm *		lt = std::localtime(&t);
	char				thetime[255];
	// year, month, day, minutes since start of day
	std::strftime(thetime, 128, "%Y.%m.%d", lt);
	std::string added		 = std::to_string((lt->tm_hour * 60 + lt->tm_min));
	std::string complete = std::string(thetime) + added;
	return complete;
}