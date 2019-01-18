#include "process_mgr.h"
#include "process.h"

#include <thread>
#include <vector>

int process_mgr::launch(int n) {
  for (int i=0;i<n;++i) {
    threads.push_back(process(i));
  }
  return n;
}

void process_mgr::destroy(pid_t pid) {
}

const std::vector<process>& process_mgr::process_list() const {
  return threads;
}
