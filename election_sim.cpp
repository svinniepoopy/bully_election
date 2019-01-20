
#include "process.h"
#include "election_sim.h"
#include "process_mgr.h"
#include "message.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <strings.h>
#include <random>
#include <vector>
#include <iostream>

int election_simulator::send_message(int sockfd, int pid, message msg, process& proc) {
  char buf[512];
  bcopy(&msg, buf, sizeof(msg));
  struct sockaddr_un name;
  name.sun_family = AF_LOCAL;
  strncpy (name.sun_path, process::get_named_socket(pid).c_str(), sizeof (name.sun_path));
  name.sun_path[sizeof (name.sun_path) - 1] = '\0';

  ssize_t nsent = 0;
  if ((nsent = sendto(
	  sockfd, buf, sizeof(msg),
	  0,
	  (struct sockaddr*)&name,
	  sizeof(struct sockaddr_un))) != sizeof(msg)) {
    return -1;
  }
  return 0;
}

int election_simulator::simulate(int nproc) {
  process_mgr procmgr;
  int n = procmgr.launch(nproc);
  if (n!=nproc) {
    std::cout << "error creating all threads\n";
  }
  std::cerr << "launched " << nproc << " processes.\n";
  std::vector<process>& plist = procmgr.process_list();

  // inform the i'th process that it's 
  // peer is thread with id 2*i
  net_helper net;
  int sockfd = net.make_sock();
  for (int i=0;i<nproc/2;++i) {
    message msg{message::PEER, i+nproc/2};
    send_message(sockfd, i, msg, plist[i]);
  }

  const int procsz = plist.size();
  std::random_device rd;
  std::uniform_int_distribution<int> dist{0,procsz};
  int randpid = dist(rd);
  // set_message(COORDINATOR, randpid);
  return 1;
}
