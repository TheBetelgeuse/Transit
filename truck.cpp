#include "truck.hpp"

TruckNS::Truck::Truck(bool init_location, int number, int weight, int speed, int lenght) {
  speed_ = speed;
  weight_ = weight;
  init_location_ = init_location;
  index_ = number;
  lenght_ = lenght;


  quantity = Semaphore(2 ,ftok(kPCFile, 7));
  quantity.Operation(0, 1, false);
  end = Semaphore(1, ftok(kPCFile, 6));
  main = Semaphore(1, ftok(kPCFile, 3));
  factory = Semaphore(1, ftok(kPCFile, 4));
  zero_controller = MessageQueue(ftok(kPCFile, 1));
  one_controller = MessageQueue(ftok(kPCFile, 2));

}

void TruckNS::Truck::StartProcess() {
  while (1) {
    if (end.IsZero(0, false)) {
      EndProcess();
    } else {
      try {
        init_location_ ? one_controller.Send({index_, weight_}, 1) : zero_controller.Send({index_, weight_,}, 1);
      } catch (int) {
        EndProcess();
        exit(404);
      }
      try {
        init_location_ ? one_controller.Receive(index_ + 2, true) : zero_controller.Receive(index_ + 2, true);
      } catch (int error) {
        if (error == EINTR) {
          EndProcess();
          return;
        } else {
          EndProcess();
          exit (404);
        }
      }
      try {
        init_location_ ? factory.Operation(0, -1, false) : main.Operation(0, -1, false);
      } catch (int) {
        exit (404);
      }
      init_location_ = !init_location_;

    }
  }
}

void TruckNS::Truck::EndProcess() {
  quantity.Operation(0, -1, false);
  if (quantity.IsZero(0, false)) {
    quantity.DeleteSem();
    end.DeleteSem();
  }
  exit(0);
}