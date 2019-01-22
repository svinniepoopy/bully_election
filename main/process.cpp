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
    case message::OKACK:
      return std::string("OKACK");
    case message::KILL:
      return std::string("KILL");
    default:
      return std::string("ERRMSG");
  }
}

process::process(int i, int nproc):
  is_coordinator{false},
  is_peer{false},
  is_client{false},
  id{i},
  maxpeers{nproc},
  sockfd{-1},
  net{net_helper{}} {
}

process::~process() {
}

int process::handle_msg_peer(const message* msg, const size_t len) {
  std::cout << "thread " << id << " recvd " << msg_type_str(msg) << '\n';
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
  std::cout << "thread " << id << " sending PING to " << sock_file << '\n';
    sz = sendto(socket(), buf, sizeof(msg),
	MSG_DONTWAIT,
	(struct sockaddr*)&name,
	sizeof(struct sockaddr_un));
    if (sz != sizeof(msg)) {
      return -1;
    }
    std::this_thread::sleep_for(std::chrono::seconds(3));
  return 0;
}

int process::handle_msg_ping(const message* msg, const size_t len) {
  std::cout << "thread " << id << "got PING from " << msg->peer_id << "\n";
  ssize_t sz=0;
  char buf[MAXMSG];
  // received a ping from a peer, reply with an ack
  message msg_out{message::OKACK, id};
  bcopy(&msg_out, buf, sizeof(msg_out));
  struct sockaddr_un name;
  name.sun_family = AF_LOCAL;
  std::string sock_file = net.get_named_socket(msg->peer_id);
  strncpy(name.sun_path, 
      sock_file.c_str(),
      sizeof(name.sun_path));
  name.sun_path[sizeof(name.sun_path)-1] = '\0';
  std::cout << "thread " << id << " sending ACK to " << sock_file << '\n';

  sz = sendto(socket(), buf, MAXMSG,
      0,	// block until a msg is received
      (struct sockaddr*)&name,
      sizeof(struct sockaddr_un));
  if (sz != sizeof(buf)) {
    return -1;
  }
  std::cout << "thread " << this->id << "recvd PING " << msg_type_str((message*)buf) << '\n'; 

  return 0;
}

int process::handle_msg_coordinator(const message* msg, const size_t len) {
  is_coordinator = true;
  return 0;
}

int process::handle_msg_elect(const message* msg, const size_t len) {
  return 0;
}

int process::handle_msg_kill(const message* msg, const size_t len) {
  std::cout << "!!!!!!! KILL received (ID " << id <<" )!!!!!!!!\n";
  return 1;
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
    case message::OKACK:
      return handle_msg_peer(msg, len);
    case message::KILL:
      return handle_msg_kill(msg, len);
  }
  return 0;
}

int process::socket() const {
  return sockfd;
}

void process::operator()() {
  net_helper net;
  int sockfd = net.make_sock();
  std::string sock_file = net.get_named_socket(id);
  sockfd = net.make_named_socket(sockfd, sock_file.c_str()); 
  this->sockfd = sockfd; 
  std::cout << "started thread " << id << 
    " socket " << sockfd << " at path " << sock_file << '\n';
  char buf[MAXMSG];
  ssize_t size;
  while (true) {
    size = recv(sockfd, buf, MAXMSG, 0);
    std::this_thread::sleep_for(std::chrono::seconds(5));
    message* msg = (message*)buf;
    if (size>0) {
      std::cout << ">> " << id << " << recvd msg [" 
	<< msg_type_str(msg) << "]\n"; 
      if (handle_msg(msg, size) == 1) {
	break;
      }
    }
  }
  std::cout << "EXIT! unlinking " << id << '\n';
  unlink(net.get_named_socket(id).c_str());
}
