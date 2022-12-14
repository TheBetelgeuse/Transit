#pragma once

#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>

#include <optional>

#define MSG_SIZE 22
#define PERM 0666

class MessageQueue {
 public:
  MessageQueue() = default;

  explicit MessageQueue(key_t key);

  MessageQueue(const MessageQueue& other) noexcept;

  ~MessageQueue() = default;

  MessageQueue& operator=(const MessageQueue& other) noexcept;

  std::optional<std::pair<int, int>> Receive(int64_t msg_type, bool wait);

  void Send(std::pair<int, int> message, int64_t msg_type);

  void DeleteQueue() noexcept;

  bool IsOwner() noexcept;

 private:
  int descriptor_;
  bool owner_ = false;
  struct MsgBuf {
    int64_t type;
    char msg[MSG_SIZE];
  };
};

class Semaphore {
 public:
  Semaphore() = default;

  explicit Semaphore(uint8_t num_of_sems, key_t key);

  Semaphore(const Semaphore& other) noexcept;

  ~Semaphore() = default;

  Semaphore& operator=(const Semaphore& other) noexcept;

  bool Operation(uint8_t sem_index, short operation, bool wait);

  bool IsZero(uint8_t sem_index, bool wait);

  void DeleteSem() noexcept;

 private:
  int descriptor_;
  uint8_t num_of_sems_;
};