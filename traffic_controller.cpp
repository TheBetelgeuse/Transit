#include "traffic_controller.hpp"

#include "initialization.hpp"

void TrafficControllerFoo(uint8_t max_mass, bool location, key_t message_queue,
                          key_t semaphore, std::string log_dir) {
  log_dir += "LogTrafficController";
  log_dir += char(location);
  log_dir += ".txt";
  SystemFile log;
  if (log.openf(log_dir.c_str())) {
    exit(1);
  }
  TrafficController traffic_controller(max_mass, location, message_queue,
                                       semaphore, log);
  while (true) {
    traffic_controller.GetTrucks();
    traffic_controller.SendTrucksToBridgeAndWait();
    traffic_controller.TransferControlToAnotherControllerAndWait();
  }
}
