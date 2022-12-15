#include <unistd.h>

#include <iostream>
#include <vector>

#include "traffic_controller.hpp"
#include "truck.hpp"

int main() {
  SystemFile file;
  file.openf("Logging/LogInitializator.txt");
  std::vector<char> logs(200);

  int max_load, length, max_number_of_trucks;
  std::cin >> max_load >> length >> max_number_of_trucks;
  sprintf(logs.data(),
          "Максимальная нагрузка - %d, длина моста - %d, максимальное "
          "количество грузовиков - %d\n",
          max_load, length, max_number_of_trucks);
  file.writef(logs.data(), std::strlen(logs.data()));

  std::vector<TCNS::TrafficController*> traffic_controllers(2, nullptr);
  for (int i = 0; i < 2; ++i) {
    traffic_controllers[i] =
        new TCNS::TrafficController{bool(i), max_load, max_number_of_trucks};

    logs = std::vector<char>(200);
    sprintf(logs.data(), "Создан регулировщик № %d\n", i);
    file.writef(logs.data(), std::strlen(logs.data()));
  }

  int pid;
  for (int i = 0; i < 2; ++i) {
    pid = fork();
    if (pid == 0) {
      traffic_controllers[i]->StartProcess();
      exit(0);
    }
  }

  int init_number_of_trucks;
  std::cin >> init_number_of_trucks;
  std::vector<TruckNS::Truck*> trucks(init_number_of_trucks, nullptr);
  for (int i = 0; i < init_number_of_trucks; ++i) {
    bool location;
    int weight, speed;
    std::cin >> location >> weight >> speed;
    trucks[i] = new TruckNS::Truck{location, i, weight, speed, length};

    logs = std::vector<char>(200);
    sprintf(logs.data(),
            "Создан грузовик № %d: масса - %d, скорость - %d, местоположение "
            "-  %d\n",
            i, weight, speed, location);
    file.writef(logs.data(), std::strlen(logs.data()));
  }

  for (int i = 0; i < init_number_of_trucks; ++i) {
    pid = fork();
    if (pid == 0) {
      trucks[i]->StartProcess();
      exit(0);
    }
  }

  file.closef();
}