#include "truck.hpp"
#include <string>

TruckNS::Truck::Truck(bool init_location, int number, int weight, int speed,
                      int lenght) {
  speed_ = speed;
  weight_ = weight;
  init_location_ = init_location;
  index_ = number;
  lenght_ = lenght;

  quantity = Semaphore(2, ftok(kPCFile, 7));
  quantity.Operation(0, 1, false);
  end = Semaphore(1, ftok(kPCFile, 6));
  main = Semaphore(1, ftok(kPCFile, 3));
  factory = Semaphore(1, ftok(kPCFile, 4));
  zero_controller = MessageQueue(ftok(kPCFile, 1));
  one_controller = MessageQueue(ftok(kPCFile, 2));
  file.openf("LogTruck.txt");
  std::vector<char> log(200);
  sprintf(log.data(),
          "Был создан самосвал с такими параметрами:\nСкорость – %d; номер "
          "самосвала - %d"
          " вес самосвала – %d направление – %d\n",
          speed, number, weight, init_location);
  file.writef(log.data(), std::strlen(log.data()));
}

void TruckNS::Truck::StartProcess() {
  while (1) {
    if (end.IsZero(0, false)) {
      EndProcess();
    } else {
      try {
        init_location_ ? one_controller.Send({index_, weight_}, 1)
                       : zero_controller.Send(
                             {
                                 index_,
                                 weight_,
                             },
                             1);
        std::vector<char> log(200);
        sprintf(log.data(),
                "Самосвал номер %d подъехал к регулировщику номер %s", index_,
                init_location_ ? "1\n" : "0\n");
        file.writef(log.data(), std::strlen(log.data()));
      } catch (int) {
        EndProcess();
        exit(404);
      }
      try {
        init_location_ ? one_controller.Receive(index_ + 2, true)
                       : zero_controller.Receive(index_ + 2, true);
        std::vector<char> log(200);
        sprintf(log.data(),
                "Самосвал номер %d получил разрешение на проед от "
                "регулировщика номер %d\n",
                index_, 123);
        file.writef(log.data(), std::strlen(log.data()));
      } catch (int error) {
        if (error == EINTR) {
          EndProcess();
          return;
        } else {
          EndProcess();
          exit(404);
        }
      }
      try {
        init_location_ ? factory.Operation(0, -1, false)
                       : main.Operation(0, -1, false);
        std::vector<char> log(200);
        sprintf(log.data(),
                "Самосвал номер %d проехал мост и передал об этом сообщение "
                "регулировщику номер %s",
                index_, init_location_ ? "1\n" : "0\n");
        file.writef(log.data(), std::strlen(log.data()));
      } catch (int) {
        exit(404);
      }
      init_location_ = !init_location_;
    }
  }
}

void TruckNS::Truck::EndProcess() {
  file.closef();
  quantity.Operation(0, -1, false);
  if (quantity.IsZero(0, false)) {
    quantity.DeleteSem();
    end.DeleteSem();
  }
  exit(0);
}