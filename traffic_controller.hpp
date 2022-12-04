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

struct DumpTruck {
  key_t semaphore;
  size_t weight;
};

class TrafficController {
 public:
  TrafficController(uint8_t max_mass, bool location, key_t message_queue_key,
                    key_t semaphore_key, SystemFile log);
  ~TrafficController();
  void GetTrucks();
  void SendTrucksToBridgeAndWait();
  void TransferControlToAnotherControllerAndWait();

 private:
  bool controller_id_;
  size_t allowed_weight_;
  size_t curr_weight_;
  std::queue<DumpTruck> dump_truck_queue_;
  int message_queue_descriptor;
  int sem_traffic_controller_descriptor_;
  sembuf wait_for_control_;
  sembuf transfer_control_;
  SystemFile log_;

  std::optional<DumpTruck> TruckArrival();
  bool IsControlMine();
};

#endif  // TRANSIT_TRAFFIC_CONTROLLER_HPP_
