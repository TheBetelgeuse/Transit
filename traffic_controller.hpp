#ifndef TRANSIT_TRAFFIC_CONTROLLER_HPP_
#define TRANSIT_TRAFFIC_CONTROLLER_HPP_

#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/types.h>

#include <iostream>
#include <optional>
#include <queue>
#include <string>

#include "SystemFile.hpp"
#include "sem_and_queue.hpp"

namespace TCNS {

struct Truck {  // Структура, которую передаёт самосвал регулировщику
  int number;
  int weight;
};

class TrafficController {  // регулировщик
 public:
  TrafficController(bool location, int max_mass, int max_num_of_trucks);
  ~TrafficController();
  void StartProcess();

 private:
  bool location_;
  int allowed_weight_;
  std::queue<Truck> truck_queue_;

  MessageQueue message_queue_;
  Semaphore queue_semaphore_;
  Semaphore tc_semaphore_;
  Semaphore turn_off_semaphore_;
  Semaphore num_of_users_semaphore_;

  SystemFile log_;


  void GetTrucks();
  std::optional<Truck> TruckArrival();
  bool SendTrucksToBridgeAndWait();
  void TransferControlToAnotherControllerAndWait();

  bool IsTurnedOn();
  void Finish();
  void Logging(int mode, int add_inf1 = -1);
  std::string IntToString(int number);
};

}  // namespace TrafficControllerNS

#endif  // TRANSIT_TRAFFIC_CONTROLLER_HPP_
