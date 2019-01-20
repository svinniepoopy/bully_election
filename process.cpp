#include "net_helper.h"
#include "process.h"

#include <unistd.h>
#include <stddef.h>
#include <errno.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <iostream>
#include <string>
#include <sstream>
#include <thread>

namespace {
const size_t MAXMSG = 512;
}; // namespace

process::process(int i):
  id{i},
  net{net_helper{}} {
  std::thread t{run_proc(id)};
  std::cout << "starting thread " << i << '\n';
  t.join();
}

void process::handle_msg(const char* msg, size_t len) {
  std::cerr << "received " << std::string(msg) << '\n';
}

std::string process::get_named_socket(pid_t pid) {
  std::stringstream ss;
  ss << "/tmp/test_sim__";
  ss << "_";
  ss << pid; 
  return ss.str(); 
}

void process::run_proc::operator()() const {
  net_helper net;
  int sock = net.make_named_socket(net.make_sock(), process::get_named_socket(id));
  char buf[MAXMSG];
  ssize_t size;
  while (true) {
    size = recv(sock, buf, MAXMSG, 0);
    if (size>0) {
      std::cerr << "received " << buf << '\n'; 
    }
  }
}
