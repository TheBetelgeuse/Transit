#include "traffic_controller.hpp"

#include "initialization.hpp"

enum LoggingModes {
  PrecessConnectionInitFailed = 0,
  ProcessConnectionOperationFailed = 1,
  TruckArrived = 2,
  NoTruckInQueue = 3
};

TCNS::TrafficController::TrafficController(bool location, uint8_t max_mass,
                                           uint8_t max_num_of_trucks) {
  location_ = location;
  allowed_weight_ = max_mass;
  std::string log_file = "LogTrafficController ";
  log_file += char(location_);
  if (log_.openf(log_file.c_str()) < 0) {
    Finish();
    exit(1);
  }
  try {
    message_queue_ = MessageQueue(ftok(kLogFile, int(location_) + 1));
    tc_semaphore_ = Semaphore(1, ftok(kLogFile, 3));
    turn_off_semaphore_ = Semaphore(1, ftok(kLogFile, 4));

    if (turn_off_semaphore_.IsOwner()) {
      try {
        turn_off_semaphore_.Operation(0, max_num_of_trucks, false);
      } catch (int error) {
        Logging(ProcessConnectionOperationFailed, error);
        Finish();
      }
    }

    num_of_users_semaphore_ = Semaphore(2, ftok(kLogFile, 5));
    try {
      num_of_users_semaphore_.Operation(0, 1, false);
      num_of_users_semaphore_.Operation(1, 1, false);
    } catch (int error) {
      Logging(ProcessConnectionOperationFailed, error);
      Finish();
    }

  } catch (int error) {
    Logging(PrecessConnectionInitFailed, error);
    Finish();
  }
}

TCNS::TrafficController::~TrafficController() { Finish(); }

void TCNS::TrafficController::StartProcess() {
  if (location_ == factory) {
    tc_semaphore_.Operation(0, -1, true);
  }
  while (IsTurnedOn()) {
    GetTrucks();
    SendTrucksToBridgeAndWait();
    TransferControlToAnotherControllerAndWait();
  }
}

void TCNS::TrafficController::GetTrucks() {
  std::optional<Truck> truck = TruckArrival();
  if (!truck.has_value()) {
    Logging(NoTruckInQueue);
  }
  while (truck.has_value()) {
    truck_queue_.push(truck.value());
    Logging(TruckArrived, truck.value().number);
    truck = TruckArrival();
  }
}

std::optional<TCNS::Truck> TCNS::TrafficController::TruckArrival() {
  std::optional<std::pair<int, int>> raw_truck =
      message_queue_.Receive(1, false);
  return raw_truck.has_value() ? {raw_truck.value().first, raw_truck.value().second}
}