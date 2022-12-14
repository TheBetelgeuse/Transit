#ifndef TRANSIT__TRUCK_HPP_
#define TRANSIT__TRUCK_HPP_

#include "sem_and_queue.hpp"
#include "initialization.hpp"
#include <optional>

namespace TruckNS {

class Truck {
 public:
  Truck(bool init_location, int number, int weight, int speed, int lenght);
  int GetSpeed() const { return speed_; }
  int GetWeight() const { return weight_; }
  bool GetPosition() const { return init_location_; }

 private:
  int speed_;
  int weight_;
  bool init_location_;
  int index_;
  int lenght_;
  void StartProcess();
};

Truck::Truck(bool init_location, int number, int weight, int speed, int lenght) {
  speed_ = speed;
  weight_ = weight;
  init_location_ = init_location;
  index_ = number;
  lenght_ = lenght;

  const char* pathname = "transit";
  Semaphore end(2 ,ftok(pathname, 7));

}
void Truck::StartProcess() {
  while (1) {
    if (init_location_ == 0) {

    }
  }

}
} // namespace TruckNS


#endif // TRANSIT__TRUCK_HPP_