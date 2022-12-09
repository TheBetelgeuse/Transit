#ifndef TRANSIT_TRAFFIC_CONTROLLER_HPP_
#define TRANSIT_TRAFFIC_CONTROLLER_HPP_

#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/types.h>

#include <iostream>
#include <optional>
#include <queue>

#include "SystemFile.hpp"

namespace TrafficControllerNS {

struct DumpTruck {  // Структура, которую передаёт самосвал регулировщику
  uint8_t number;
  uint8_t weight;
};

class TrafficController {  // регулировщик
 public:
  TrafficController(uint8_t max_mass, bool location, key_t message_queue_key,
                    key_t truck_semaphore_arr_key, key_t tc_semaphore_key,
                    key_t turn_off_semaphore, SystemFile log);
  ~TrafficController();
  void StartProcess();

 private:
  bool controller_id_;
  size_t allowed_weight_;
  size_t curr_weight_;
  std::queue<DumpTruck> dump_truck_queue_;

  int message_queue_descriptor;
  int sem_tc_descriptor_;
  int sem_turn_off_descriptor_;
  int sem_truck_descriptor_;

  SystemFile log_;

  void GetTrucks();
  std::optional<DumpTruck> TruckArrival();
  void SendTrucksToBridgeAndWait();
  void TransferControlToAnotherControllerAndWait();

  bool IsControlMine();
  bool IsTurnedOff();
};

}  // namespace TrafficControllerNS

#endif  // TRANSIT_TRAFFIC_CONTROLLER_HPP_
