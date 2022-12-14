#include "traffic_controller.hpp"
#include <iostream>

int main() {
  int location, max_weight, num_of_trucks;
  std::cin >> location >> max_weight >> num_of_trucks;
  TCNS::TrafficController traffic_controller(location, max_weight, num_of_trucks);
}