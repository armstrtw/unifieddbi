#include <sstream>
#include "conversion.utils.hpp"

using std::string;
using std::stringstream;

string itos(const int i) {
  stringstream s;
  s << i;
  return s.str();
}
