#ifndef TRANSIT_INITIALIZATION_HPP_
#define TRANSIT_INITIALIZATION_HPP_
#define MESSAGE_SIZE 19

#include <iostream>
#include <string>

struct MessageBuffer {
  int32_t message_type = 0;
  char message_text[MESSAGE_SIZE];
};

void TruckFoo(uint8_t weight, uint8_t speed, uint8_t bridge_length, bool location,
             key_t message_queue_traffic_controller1,
             key_t message_queue_traffic_controller2, std::string log_dir);

void TrafficControllerFoo(uint8_t max_mass, bool location, key_t message_queue,
                         key_t semaphore, std::string log_dir);

#endif  // TRANSIT_INITIALIZATION_HPP_
