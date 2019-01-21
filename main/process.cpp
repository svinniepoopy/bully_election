#include "net_helper.h"
#include "message.h"
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
#include <chrono>

namespace {
const size_t MAXMSG = 512;
}; // namespace

static std::string msg_type_str(const message* msg) {
  switch (msg->type) {
    case message::PEER:
      return std::string("PEER");
    case message::OK:
      return std::string("OK");
    case message::ELECT:
      return std::string("ELECT");
    case message::COORDINATOR:
      return std::string("COORDINATOR");
    default:
      return std::string("ERRMSG");
  }
}

process::process(int i):
  id{i},
  net{net_helper{}} {
  std::thread t{run_proc(i, this)};
  std::cout << "starting thread " << id << '\n';
  t.detach();
}

int process::handle_msg_peer(const message* msg, const size_t len) {
  std::cout << "thread " << id << " recvd PEER\n";
  char buf[MAXMSG];
  message msg_out{message::OK, id};
  bcopy(&msg_out, buf, sizeof(*msg));
  ssize_t sz=0;
  struct sockaddr_un name;
  name.sun_family = AF_LOCAL;
  std::string sock_file = net.get_named_socket(msg->peer_id);
  strncpy(name.sun_path, 
      sock_file.c_str(),
      sizeof(name.sun_path));
  name.sun_path[sizeof(name.sun_path)-1] = '\0';
  std::cout << "thread " << id << " starting PING to " << sock_file << '\n';
  while (true) {
    sz = sendto(socket(), buf, sizeof(msg),
	MSG_DONTWAIT,
	(struct sockaddr*)&name,
	sizeof(struct sockaddr_un));
    if (sz != sizeof(msg)) {
      return -1;
    }
    std::this_thread::sleep_for(std::chrono::seconds(3));
  }
  return 0;
}

int process::handle_msg_ping(const message* msg, const size_t len) {
  std::cout << "thread " << id << "got PING from " << msg->peer_id << "\n";
  ssize_t sz=0;
  char buf[MAXMSG];
  struct sockaddr_un name;
  socklen_t namelen=sizeof(name);
  while (true) {
    sz = recvfrom(socket(), buf, MAXMSG,
	0,	// block until a msg is received
	(struct sockaddr*)&name,
	&namelen);
    if (sz != sizeof(buf)) {
      return -1;
    }
    std::cout << "thread " << this->id << "recvd PING " << msg_type_str((message*)buf) << '\n'; 
  }
}

int process::handle_msg_coordinator(const message* msg, const size_t len) {
  return 0;
}

int process::handle_msg_elect(const message* msg, const size_t len) {
  return 0;
}

int process::handle_msg(const message* msg, size_t len) {
  switch(msg->type) {
    case message::PEER:
      return handle_msg_peer(msg, len);
    case message::COORDINATOR:
      return handle_msg_coordinator(msg, len);
    case message::ELECT:
      return handle_msg_elect(msg, len);
    case message::OK:
      return handle_msg_ping(msg, len);
  }
  return 0;
}

int process::socket() const {
  return sockfd;
}

void run_proc::operator()() const {
  net_helper net;
  int sockfd = net.make_sock();
  std::string sock_file = net.get_named_socket(id);
  sockfd = net.make_named_socket(sockfd, sock_file.c_str()); 
  proc->sockfd = sockfd; 
  std::cout << "started thread " << id << 
    " socket " << sockfd << " at path " << sock_file << '\n';
  proc->id = id;
  char buf[MAXMSG];
  ssize_t size;
  while (true) {
    size = recv(sockfd, buf, MAXMSG, 0);
    std::this_thread::sleep_for(std::chrono::seconds(5));
    message* msg = (message*)buf;
    if (size>0) {
      std::cout << ">> " << id << " << recvd msg [" 
	<< msg_type_str(msg) << "]\n"; 
      if (proc->handle_msg(msg, size) == -1) {
	break;
      }
    }
  }
}
