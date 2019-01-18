#include <thread>

struct proc {
  void operator()() {
  }
};

struct Task{
  Task() {
    std::thread t{proc()};
    t.join();
  }
};

int main() {
  Task t;
  return 0;
}
