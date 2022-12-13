#include "sem_and_queue.hpp"

MessageQueue::MessageQueue(key_t key) {
  descriptor_ = msgget(key, PERM);
  if (descriptor_ == -1) {
    if (errno == ENOENT) {
      descriptor_ = msgget(key, IPC_CREAT | PERM);
      if (descriptor_ < 0) {
        throw errno;
      }
    } else {
      throw errno;
    }
  }
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

Semaphore::Semaphore(key_t key) {
  descriptor_ = semget(key, 1, PERM);
  if (descriptor_ < 0) {
    if (errno == ENOENT) {
      descriptor_ = semget(key, 1, IPC_CREAT | PERM);
      if (descriptor_ < 0) {
        throw errno;
      }
    } else {
      throw errno;
    }
  }
}

bool Semaphore::Operation(short operation, bool wait) {
  short flag = wait ? 0 : IPC_NOWAIT;
  sembuf buf = {0, operation, flag};
  if (semop(descriptor_, &buf, 1) < 0) {
    if (errno == EAGAIN) {
      return false;
    }
    throw errno;
  }
  return true;
}

bool Semaphore::IsZero(bool wait) {
  short flag = wait ? 0 : IPC_NOWAIT;
  sembuf buf = {0, 0, flag};
  if (semop(descriptor_, &buf, 1) < 0) {
    if (errno == EAGAIN) {
      return false;
    }
    throw errno;
  }
  return true;
}

void Semaphore::DeleteSem() { semctl(descriptor_, 1, IPC_RMID, nullptr); }
