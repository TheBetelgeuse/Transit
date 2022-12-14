#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>

#include <iostream>
#define PERM 0666

const char kLogFile[] = "transit";

bool SemaphoreCheck(key_t key, short num_of_sems) {
  int descriptor = semget(key, num_of_sems, PERM);
  bool result = descriptor < 0;
  if (!result) {
    semctl(descriptor, num_of_sems, IPC_RMID, NULL);
  }
  return result;
}

bool MsgQueueCheck(key_t key) {
  int descriptor = msgget(key, PERM);
  bool result = descriptor < 0;
  if (!result) {
    msgctl(descriptor, IPC_RMID, NULL);
  }
  return result;
}

int main() {
  std::cout << "Mine msg: ";
  if (MsgQueueCheck(ftok(kLogFile, 1))) {
    std::cout << "OK\n";
  } else {
    std::cout << "error\n";
  }

  std::cout << "Factory msg: ";
  if (MsgQueueCheck(ftok(kLogFile, 2))) {
    std::cout << "OK\n";
  } else {
    std::cout << "error\n";
  }

  std::cout << "Mine sem: ";
  if (SemaphoreCheck(ftok(kLogFile, 3), 1)) {
    std::cout << "OK\n";
  } else {
    std::cout << "error\n";
  }

  std::cout << "Factory sem: ";
  if (SemaphoreCheck(ftok(kLogFile, 4), 1)) {
    std::cout << "OK\n";
  } else {
    std::cout << "error\n";
  }

  std::cout << "Traffic Controller sem: ";
  if (SemaphoreCheck(ftok(kLogFile, 5), 1)) {
    std::cout << "OK\n";
  } else {
    std::cout << "error\n";
  }

  std::cout << "End sem: ";
  if (SemaphoreCheck(ftok(kLogFile, 6), 1)) {
    std::cout << "OK\n";
  } else {
    std::cout << "error\n";
  }

  std::cout << "Num of users sem: ";
  if (SemaphoreCheck(ftok(kLogFile, 7), 2)) {
    std::cout << "OK\n";
  } else {
    std::cout << "error\n";
  }
}