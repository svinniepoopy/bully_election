
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

int election_simulator::send_message(
    int sockfd, int dest_peer_id, message msg_out, process& toproc) {
  char buf[512];
  bcopy(&msg_out, buf, sizeof(msg_out));
  struct sockaddr_un name;
  name.sun_family = AF_LOCAL;
  strncpy(name.sun_path, 
      process::get_named_socket(dest_peer_id).c_str(), 
      sizeof (name.sun_path));
  name.sun_path[sizeof (name.sun_path) - 1] = '\0';
  ssize_t nsent = 0;
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
  process_mgr procmgr;
  int n = procmgr.launch(nproc);
  if (n!=nproc) {
    std::cout << "error creating all threads\n";
  }
  std::cerr << "launched " << nproc << " threads.\n";
  std::vector<process>& plist = procmgr.process_list();

  // inform the i'th process that it's 
  // peer is thread with id 2*i
  net_helper net;
  int sockfd = net.make_sock();
  int dest_peer_id;
  for (int i=0;i<nproc/2;++i) {
    dest_peer_id = i+nproc/2;
    std::cout << "PEER " << i << "->" << dest_peer_id  << '\n';
    message msg{message::PEER, dest_peer_id};
    std::this_thread::sleep_for(std::chrono::seconds(2));
    send_message(sockfd, i, msg, plist[i]);
  }

  while (1) {
  }

  const int procsz = plist.size();
  std::random_device rd;
  std::uniform_int_distribution<int> dist{0,procsz};
  int randpid = dist(rd);
  // set_message(COORDINATOR, randpid);
  return 1;
}
