#include "net_helper.h"
#include "message.h"
#include "message_handler.h"
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
  npeers{nproc},
  sockfd{-1},
  net{net_helper{}},
  handler{message_handler} {
}

process::~process() {
}

int process::handler_msg_peer(const message* msg, const size_t len) {
  std::cout << "thread " << id << " recvd " << msg_type_str(msg) << '\n';
  std::string sock_file = net.get_named_socket(msg->peer_id);
  message msg_out{message::OK, id};
  std::cout << "thread " << id << " sending PING to " << sock_file << '\n';
  handler.send_message(socket(), sock_file.c_str(), &msg_out);
  std::this_thread::sleep_for(std::chrono::seconds(3));
  return 0;

}

int process::handle_msg_ping(const message* msg, const size_t len) {
  std::cout << "thread " << id << "got PING from " << msg->peer_id << "\n";
  // received a ping from a peer, reply with an ack
  message msg_out{message::OKACK, id};
  std::cout << "thread " << id << " sending ACK to " << sock_file << '\n';
  handler.send_message(socket(), sock_file.c_str(), &msg_out);
  return 0;
}

int process::handle_msg_coordinator(const message* msg, const size_t len) {
  if (coordinator_id == id) {	// this thread is now the coordinator
    is_coordinator = true;
    for (int peerid=0;peerid<npeers;++peerid) {
      if (peerid == id) { continue; }
      std::string peersockname{net.get_named_socket(peerid)};
      handler.send_message(socket(), 
	  peersockname.c_str(), 
	  {message::COORDINATOR, id}); 
    }
  } else {
    for (int peerid=id+1; peerid<npeers;++peerid) {
      std::string peersockname{net.get_named_socket(peerid)};
      handler.send_message(socket(),
	  peersockname.c_str(), 
	  {message::COORDINATOR, id}); 
    }
  }
  return 0;
}

int process::handle_msg_elect(const message* msg, const size_t len) {
  // TODO
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
