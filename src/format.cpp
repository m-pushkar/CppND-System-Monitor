#include "format.h"

#include <iostream>
#include <sstream>
#include <string>

using std::string;
using std::stringstream;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
  // Converts system recorded seconds to H:M:S
  long hour, min, sec;
  hour = seconds / 3600;
  min = (seconds % 3600) / 60;
  sec = seconds % 60;

  // Display converted output
  stringstream output{};
  output.fill('0');
  output.width(2);
  output << hour << ":";
  output.fill('0');
  output.width(2);
  output << min << ":";
  output.fill('0');
  output.width(2);
  output << sec;
  return output.str();
}
