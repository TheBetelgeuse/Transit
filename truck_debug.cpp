#include "truck.hpp"
#include <iostream>

int main() {
  int location, number, weight, speed, length;
  std::cin >> location >> number >> weight >> speed >> length;
  TruckNS::Truck truck(location, number, weight, speed, length);
  truck.StartProcess();
}