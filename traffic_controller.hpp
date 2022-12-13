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
  TrafficController(bool location, uint8_t max_mass, uint8_t max_num_of_trucks);
  ~TrafficController();
  void StartProcess();

 private:
  bool location_;
  size_t allowed_weight_;
  size_t curr_weight_ = 0;
  std::queue<Truck> truck_queue_;

  MessageQueue message_queue_;
  Semaphore tc_semaphore_;
  Semaphore turn_off_semaphore_;
  Semaphore num_of_users_semaphore_;

  SystemFile log_;


  void GetTrucks();
  std::optional<Truck> TruckArrival();
  void SendTrucksToBridgeAndWait();
  void TransferControlToAnotherControllerAndWait();

  bool IsTurnedOn();
  void Finish();
  void Logging(int mode, int add_inf1 = 0, int add_inf2 = 0);
};

}  // namespace TrafficControllerNS

#endif  // TRANSIT_TRAFFIC_CONTROLLER_HPP_
