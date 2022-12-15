#pragma once

#include "sem_and_queue.hpp"
#include "for_society.hpp"
#include <optional>

namespace TruckNS {

class Truck {
 public:
  Truck() = default;
  Truck(bool init_location, int number, int weight, int speed, int lenght);
  void StartProcess();

 private:
  int speed_;
  int weight_;
  bool init_location_;
  int index_;
  int lenght_;
  const char* pathname = "transit";
  Semaphore quantity;
  Semaphore end;
  Semaphore main;
  Semaphore factory;
  MessageQueue zero_controller;
  MessageQueue one_controller;

  void EndProcess();
};

} // namespace TruckNS