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

namespace TCNS {

struct Truck {  // Структура, которую передаёт самосвал регулировщику
  uint8_t number;
  uint8_t weight;
};

class TrafficController {  // регулировщик
 public:
  TrafficController(bool location, uint8_t max_mass, uint8_t max_num_of_trucks);
  ~TrafficController();
  void StartProcess();

 private:
  bool controller_location_;
  size_t allowed_weight_;
  size_t curr_weight_;
  std::queue<Truck> dump_truck_queue_;

  int message_queue_descriptor;
  int sem_tc_descriptor_;
  int sem_turn_off_descriptor_;
  int sem_truck_descriptor_;
  int sem_num_of_users_descriptor_;

  SystemFile log_;

  void GetTrucks();
  std::optional<Truck> TruckArrival();
  void SendTrucksToBridgeAndWait();
  void TransferControlToAnotherControllerAndWait();

  bool IsControlMine();
  bool IsTurnedOn();
};

}  // namespace TrafficControllerNS

#endif  // TRANSIT_TRAFFIC_CONTROLLER_HPP_
