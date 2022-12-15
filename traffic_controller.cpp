#include "traffic_controller.hpp"

#include <string>

#include "for_society.hpp"
#include "unistd.h"

enum LoggingModes {
  PrecessConnectionInitFailed = -1,
  ProcessConnectionOperationFailed = -2,
  TruckArrived = 0,
  NotSupportedTruckInFront = 1,
  NoTruckInQueue = 2,
  TruckSendToBridge = 3,
  TrucksPassedBridge = 4,
  ControlTransferred = 5,
  ControlCannotBeTransferred = 6,
  ControlGot = 7,
  StartintProcess = 8,
  FinishingProcess = 9
};

TCNS::TrafficController::TrafficController(bool location, int max_mass,
                                           int max_num_of_trucks) {
  location_ = location;
  allowed_weight_ = max_mass;
  std::string log_file = "LogTrafficController_";
  if (location == mine) {
    log_file += "mine.txt";
  } else {
    log_file += "factory.txt";
  }
  log_.openf(log_file.c_str());
  try {
    message_queue_ = MessageQueue(ftok(kPCFile, int(location_) + 1));
    queue_semaphore_ = Semaphore(1, ftok(kPCFile, int(location_) + 3));
    tc_semaphore_ = Semaphore(1, ftok(kPCFile, 5));
    turn_off_semaphore_ = Semaphore(1, ftok(kPCFile, 6));

    if (location_ == mine) {
      try {
        turn_off_semaphore_.Operation(0, max_num_of_trucks, false);
      } catch (int error) {
        Logging(ProcessConnectionOperationFailed, error);
        Finish();
        exit(ProcessConnectionOperationFailed);
      }
    }

    num_of_users_semaphore_ = Semaphore(2, ftok(kPCFile, 7));
    try {
      num_of_users_semaphore_.Operation(0, 1, false);
      if (location_ == factory) {
        num_of_users_semaphore_.Operation(1, 1, false);
      }
    } catch (int error) {
      Logging(ProcessConnectionOperationFailed, error);
      Finish();
      exit(ProcessConnectionOperationFailed);
    }

  } catch (int error) {
    Logging(PrecessConnectionInitFailed, error);
    Finish();
    exit(PrecessConnectionInitFailed);
  }
}

void TCNS::TrafficController::StartProcess() {
  Logging(StartintProcess);
  if (location_ == factory) {
    try {
      tc_semaphore_.Operation(0, -1, true);
      Logging(ControlGot);
    } catch (int error) {
      Logging(ProcessConnectionOperationFailed, error);
      Finish();
      exit(ProcessConnectionOperationFailed);
    }
  }
  while (IsTurnedOn()) {
    sleep(1);
    GetTrucks();
    SendTrucksToBridgeAndWait();
    TransferControlToAnotherControllerAndWait();
  }
  Finish();
}

void TCNS::TrafficController::GetTrucks() {
  std::optional<Truck> truck = TruckArrival();
  if (!truck.has_value()) {
    Logging(NoTruckInQueue);
  }
  while (truck.has_value()) {
    truck_queue_.push(truck.value());
    truck = TruckArrival();
  }
}

std::optional<TCNS::Truck> TCNS::TrafficController::TruckArrival() {
  std::optional<std::pair<int, int>> raw_truck = {};
  try {
    raw_truck = message_queue_.Receive(1, false);
  } catch (int error) {
    Logging(ProcessConnectionOperationFailed, error);
    Finish();
    exit(ProcessConnectionOperationFailed);
  }
  std::optional<TCNS::Truck> result = {};
  if (raw_truck.has_value()) {
    result = {raw_truck.value().first, raw_truck.value().second};
    Logging(TruckArrived, result.value().number);
  }
  return result;
}

void TCNS::TrafficController::SendTrucksToBridgeAndWait() {
  int curr_weight = 0;

  if (!truck_queue_.empty() && truck_queue_.front().weight > allowed_weight_) {
    Logging(NotSupportedTruckInFront, truck_queue_.front().number);
    Finish();
  }
  while (!truck_queue_.empty() && IsTurnedOn()) {
    if (truck_queue_.front().weight + curr_weight > allowed_weight_) {
      break;
    }

    try {
      queue_semaphore_.Operation(0, 1, false);
      turn_off_semaphore_.Operation(0, -1, false);
      message_queue_.Send({0, 0}, truck_queue_.front().number + 2);
    } catch (int error) {
      Logging(ProcessConnectionOperationFailed, error);
      Finish();
      exit(ProcessConnectionOperationFailed);
    }

    Logging(TruckSendToBridge, truck_queue_.front().number);

    curr_weight += truck_queue_.front().weight;
    truck_queue_.pop();
  }

  try {
    queue_semaphore_.IsZero(0, true);
    if (curr_weight != 0) {
      Logging(TrucksPassedBridge);
    }
  } catch (int error) {
    Logging(ProcessConnectionOperationFailed, error);
    Finish();
    exit(ProcessConnectionOperationFailed);
  }
}

void TCNS::TrafficController::TransferControlToAnotherControllerAndWait() {
  try {
    if (num_of_users_semaphore_.IsZero(1, false)) {
      Logging(ControlCannotBeTransferred);
      return;
    }
    if (location_ == mine) {
      tc_semaphore_.Operation(0, 2, false);
      Logging(ControlTransferred);
      tc_semaphore_.IsZero(0, true);
      Logging(ControlGot);
    } else {
      tc_semaphore_.Operation(0, -1, false);
      Logging(ControlTransferred);
      tc_semaphore_.Operation(0, -1, true);
      Logging(ControlGot);
    }
  } catch (int error) {
    Logging(ProcessConnectionOperationFailed, error);
    Finish();
    exit(ProcessConnectionOperationFailed);
  }
}

bool TCNS::TrafficController::IsTurnedOn() {
  try {
    if (num_of_users_semaphore_.IsZero(1, false)) {
      if (!num_of_users_semaphore_.Operation(0, -2, false)) {
        return false;
      }
      num_of_users_semaphore_.Operation(0, 2, false);
    }
    return !turn_off_semaphore_.IsZero(0, false);
  } catch (int error) {
    Logging(ProcessConnectionOperationFailed, error);
    Finish();
    exit(ProcessConnectionOperationFailed);
  }
}

void TCNS::TrafficController::Finish() {
  message_queue_.DeleteQueue();
  queue_semaphore_.DeleteSem();
  try {
    if (num_of_users_semaphore_.IsZero(1, false)) {
      tc_semaphore_.DeleteSem();
      while (turn_off_semaphore_.Operation(0, -1, false))
        ;
    } else {
      num_of_users_semaphore_.Operation(1, -1, false);
      if (location_ == mine) {
        tc_semaphore_.Operation(0, 2, false);
      } else {
        tc_semaphore_.Operation(0, -1, false);
      }
    }
    num_of_users_semaphore_.Operation(0, -1, false);
    if (num_of_users_semaphore_.IsZero(0, false)) {
      turn_off_semaphore_.DeleteSem();
      num_of_users_semaphore_.DeleteSem();
    }
  } catch (int error) {
    tc_semaphore_.DeleteSem();
    turn_off_semaphore_.DeleteSem();
    num_of_users_semaphore_.DeleteSem();
    Logging(ProcessConnectionOperationFailed, error);
    log_.closef();
    exit(ProcessConnectionOperationFailed);
  }
  Logging(FinishingProcess);
  log_.closef();
  exit(0);
}

void TCNS::TrafficController::Logging(int mode, int add_inf) {
  std::vector<char> message(200);
  switch (mode) {
    case PrecessConnectionInitFailed:
      sprintf(message.data(),
              "Не удалось создать инструменты МПВ. Код ошибки: %d\n", add_inf);
      break;
    case ProcessConnectionOperationFailed:
      sprintf(message.data(),
              "Ошибка при обращении к инструментам МПВ. Код ошибки: %d\n",
              add_inf);
      break;
    case TruckArrived:
      sprintf(message.data(), "К регулировщику прибыл самосвал № %d\n",
              add_inf);
      break;
    case NotSupportedTruckInFront:
      sprintf(
          message.data(),
          "К регулировщику прибыл самосвал недопустимого веса с номером %d\n",
          add_inf);
      break;
    case NoTruckInQueue:
      sprintf(message.data(), "В очереди к регулировщику не самосвалов\n");
      break;
    case TruckSendToBridge:
      sprintf(message.data(), "Самосвал № %d отправлен на мост\n", add_inf);
      break;
    case TrucksPassedBridge:
      sprintf(message.data(),
              "Все отправленные на мост самосвалы успешно его преодолели\n");
      break;
    case ControlTransferred:
      sprintf(message.data(),
              "Контроль над мостом передан другому регулировщику\n");
      break;
    case ControlCannotBeTransferred:
      sprintf(message.data(),
              "Контроль не может быть передан другому регулировщика, так как "
              "его не существует\n");
      break;
    case ControlGot:
      sprintf(message.data(), "Контроль над мостом возвращён\n");
      break;
    case StartintProcess:
      sprintf(message.data(), "Регулировщик запущен\n");
      break;
    case FinishingProcess:
      sprintf(message.data(), "Регулировщик завершил работу");
      break;
    default:
      sprintf(message.data(), "Это сообщение не должно быть выведено!\n");
  }
  if (!log_.writef(message.data(), std::strlen(message.data()))) {
    std::cout << "Не удалось записать лог в файл!\n";
  }
}