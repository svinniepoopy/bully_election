#include "process.h"

#include <unistd.h>
#include <stddef.h>
#include <errno.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <sstream>
#include <thread>

process::process(int i):
  id{i},
  net{net_helper{}} {
  std::thread t{run_proc()};
  std::cout << "starting thread " << i << '\n';
  t.join();
}

#define MAXMSG 512

void process::handle_msg(const char* msg, size_t len) {
  log.write(id(), "recvd" + std::string(msg));
}

static std::string get_named_socket(pid_t pid) {
  std::stringstream ss;
  ss << "/tmp/test_sim__";
  ss << "_";
  ss << pid; 
  return tosring(ss.str());
}

void process::run_proc::operator()() const {
  int sock = get_named_socket(get_id());
  char buf[MAXMSG];
  ssize_t size;
  while (true) {
    size = recv(sock, buf, MAXMSG);
    if (size>0) {
      return handle_msg(buf, size);
    } else {
      log.write("error recv");
    }
  }
}
