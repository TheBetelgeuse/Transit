#ifndef TRANSIT__SEM_AND_QUEUE_HPP_
#define TRANSIT__SEM_AND_QUEUE_HPP_

#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>

#include <optional>

#define MSG_SIZE 22
#define PERM 0666

class MessageQueue {
 public:
  explicit MessageQueue(key_t key);

  ~MessageQueue() = default;

  std::optional<std::pair<int, int>> Receive(int64_t msg_type, bool wait);

  void Send(std::pair<int, int> message, int64_t msg_type);

  void DeleteQueue();

 private:
  int descriptor_;
  struct MsgBuf {
    int64_t type;
    char msg[MSG_SIZE];
  };
};

class Semaphore {
 public:
  explicit Semaphore(key_t key);

  ~Semaphore() = default;

  bool Operation(short operation, bool wait);

  bool IsZero(bool wait);

  void DeleteSem();

 private:
  int descriptor_;
};

#endif  // TRANSIT__SEM_AND_QUEUE_HPP_
