#include "for_society.hpp"

std::string IntToString(int number) {
  std::string result;
  if (number < 0) {
    result += '-';
    number = abs(number);
  }

  std::vector<char> reversed;
  do {
    reversed.push_back(number % 10);
    number /= 10;
  } while (number != 0);

  for (int i = reversed.size() - 1; i >= 0; --i) {
    result += reversed[i];
  }

  return result;
}