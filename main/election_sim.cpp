/* file election_sim.cpp */
#include "election_sim.h"
#include "message.h"
#include "net_helper.h"
#include "process.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <thread>
#include <chrono>
#include <strings.h>
#include <random>
#include <vector>
#include <iostream>

template<typename T>
static int send_message(
    int sockfd, const std::string& peer_sockname, T msg_out) {
  char buf[512];
  bcopy(&msg_out, buf, sizeof(msg_out));
  struct sockaddr_un name;
  name.sun_family = AF_LOCAL;
  strncpy(name.sun_path, 
      peer_sockname.c_str(), 
      sizeof (name.sun_path));
  name.sun_path[sizeof (name.sun_path) - 1] = '\0';
  ssize_t nsent = 0;
  std::cout << "send_message: " << peer_sockname << '\n';
  if ((nsent = sendto(
	  sockfd, buf, sizeof(msg_out),
	  0,
	  (struct sockaddr*)&name,
	  sizeof(struct sockaddr_un))) != sizeof(msg_out)) {
    return -1;
  }
  return 0;
}

int election_simulator::simulate(int nproc) {
  std::vector<process> threads;
  for (int i=0;i<nproc;++i) {
    threads.push_back(process(i));
  }
  std::cerr << "launched " << threads.size() << " threads.\n";
  // inform the i'th thread that it's 
  // peer is thread with id 2*i
  net_helper net;
  int sockfd = net.make_sock();
  int dest_peer_id;
  for (int i=0;i<nproc/2;++i) {
    dest_peer_id = i+nproc/2;
    std::string sockpath = net.get_named_socket(i);
    std::cout << "PEER " << i << "->" << dest_peer_id  << '\n';
    message msg{message::PEER, dest_peer_id};
    std::this_thread::sleep_for(std::chrono::seconds(2));
    send_message<message>(sockfd, sockpath, msg);
  }

  while (1) {
  }

  /*TODO
  const int procsz = threads.size();
  std::random_device rd;
  std::uniform_int_distribution<int> dist{0,procsz};
  int randpid = dist(rd);
  // set_message(COORDINATOR, randpid);
  // */
  return 1;
}
