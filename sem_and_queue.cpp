#include "sem_and_queue.hpp"

MessageQueue::MessageQueue(key_t key) {
  descriptor_ = msgget(key, PERM);
  if (descriptor_ == -1) {
    if (errno == ENOENT) {
      descriptor_ = msgget(key, IPC_CREAT | PERM);
      if (descriptor_ < 0) {
        throw errno;
      }
      owner_ = true;
    } else {
      throw errno;
    }
  }
}

MessageQueue::MessageQueue(const MessageQueue& other) {
  descriptor_ = other.descriptor_;
}

MessageQueue& MessageQueue::operator=(const MessageQueue& other) {
  descriptor_ = other.descriptor_;
}

std::optional<std::pair<int, int>> MessageQueue::Receive(int64_t msg_type,
                                                         bool wait) {
  MsgBuf msg;
  int flag = wait ? 0 : IPC_NOWAIT;
  if (msgrcv(descriptor_, &msg, MSG_SIZE, msg_type, flag) < 0) {
    if (errno == ENOMSG) {
      return {};
    }
    throw errno;
  }
  std::pair<int, int> result;
  sscanf(msg.msg, "%d %d", &result.first, &result.second);
  return result;
}

void MessageQueue::Send(std::pair<int, int> message, int64_t msg_type) {
  MsgBuf msg_buf;
  sprintf(msg_buf.msg, "%d %d", message.first, message.second);
  msg_buf.type = msg_type;
  if (msgsnd(descriptor_, &msg_buf, strlen(msg_buf.msg) + 1, 0) < 0) {
    throw errno;
  }
}

void MessageQueue::DeleteQueue() { msgctl(descriptor_, IPC_RMID, nullptr); }

bool MessageQueue::IsOwner() { return owner_; }

Semaphore::Semaphore(uint8_t num_of_sems, key_t key) {
  num_of_sems_ = num_of_sems;
  descriptor_ = semget(key, num_of_sems_, PERM);
  if (descriptor_ < 0) {
    if (errno == ENOENT) {
      descriptor_ = semget(key, num_of_sems_, IPC_CREAT | PERM);
      if (descriptor_ < 0) {
        throw errno;
      }
      owner_ = true;
    } else {
      throw errno;
    }
  }
}

Semaphore::Semaphore(const Semaphore& other) {
  descriptor_ = other.descriptor_;
}

Semaphore& Semaphore::operator=(const Semaphore& other) {
  descriptor_ = other.descriptor_;
}

bool Semaphore::Operation(uint8_t sem_index, short operation, bool wait) {
  short flag = wait ? 0 : IPC_NOWAIT;
  sembuf buf = {sem_index, operation, flag};
  if (semop(descriptor_, &buf, 1) < 0) {
    if (errno == EAGAIN) {
      return false;
    }
    throw errno;
  }
  return true;
}

bool Semaphore::IsZero(uint8_t sem_index, bool wait) {
  short flag = wait ? 0 : IPC_NOWAIT;
  sembuf buf = {sem_index, 0, flag};
  if (semop(descriptor_, &buf, 1) < 0) {
    if (errno == EAGAIN) {
      return false;
    }
    throw errno;
  }
  return true;
}

void Semaphore::DeleteSem() {
  semctl(descriptor_, num_of_sems_, IPC_RMID, nullptr);
}

bool Semaphore::IsOwner() { return owner_; }