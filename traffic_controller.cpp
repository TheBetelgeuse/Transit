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

void TrafficController::GetTrucks() {
  while (true) {
    std::optional<DumpTruck> input_truck = TruckArrival();
    if (!input_truck.has_value()) {
      break;
    }
    dump_truck_queue_.push(input_truck.value());
  }
}