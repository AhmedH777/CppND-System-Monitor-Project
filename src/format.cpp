#include <string>
#include <sstream>
#include <iomanip>

#include "format.h"

using std::string;
using std::stringstream;

// Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds)
{
	int h, m, s;
	stringstream output;

	h = (seconds / 3600) % 100; // Number of hours returns to 0 after 99
	m = (seconds / 60) % 60;
	s = seconds % 60;

	output << std::setfill('0') << std::setw(2) << h << ":"<< std::setfill('0') << std::setw(2) << m << ":"<< std::setfill('0') << std::setw(2) << s;

	return output.str();
}
