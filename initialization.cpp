#include "initialization.hpp"
#include <iostream>

int main() {
  int max_load;
  int length;
  int max_number_of_trucks;
  std::cin >> max_load >> length >> max_number_of_trucks;

  int pid;
  for (int i = 0; i < 2; ++i) {
    pid = fork();
    if (pid == 0) {
      TCNS::TrafficController reg(i, max_load, max_number_of_trucks);
      reg.StartProcess();
      return 0;
    }
  }

  int init_number_of_trucks;
  std::cin >> init_number_of_trucks;
  for (int i = 0; i < init_number_of_trucks; ++i) {
    pid = fork();
    if (pid == 0) {
      int location;
      int weight;
      int speed;
      std::cin >> location >> weight >> speed;
      TruckNS::Truck truck(location, i, weight, speed, length);
      truck.StartProcess();
      return 0;
    }
  }
}