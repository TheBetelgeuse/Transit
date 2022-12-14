#include <iostream>
#include "truck.hpp"
#include "traffic_controller.hpp"

int main() {
  SystemFile file;
  file.openf("LogInitializator.txt");

  int max_load;
  int length;
  int max_number_of_trucks;
  std::cin >> max_load >> length >> max_number_of_trucks;

  std::string bridge = "Задан мост с такими параметрами ";
  bridge += max_load;
  bridge += length;
  bridge += max_number_of_trucks;
  bridge += "\n";
  file.writef(bridge.c_str(), bridge.length() + 1);

  int pid;
  for (int i = 0; i < 2; ++i) {
    pid = fork();
    if (pid == 0) {
      TCNS::TrafficController reg(i, max_load, max_number_of_trucks);
      std::string log = "Создан регулировщик номер \n";
      log += char(i);
      file.writef(log.c_str(), log.length() + 1);
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
      std::string log = "Cоздан грузовик номер ";
      log += i;
      log += "со скоростью ";
      log += speed;
      log += " и местоположением ";
      log += location;
      log += "\n";
      file.writef(log.c_str(), log.length() + 1);
      truck.StartProcess();
      return 0;
    }
  }
  /*******************************************************/
  for (int i = 0; i < init_number_of_trucks + 2; ++i) {
    int tmp;
    pid = wait(&tmp);
  }
  std::cout << "Init Finish\n";
  /*******************************************************/
}