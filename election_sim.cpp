#include "election_sim.h"
#include "process_mgr.h"

#include <strings.h>
#include <random>
#include <vector>
#include <iostream>



int election_simulator::send_message(int pid, message msg, const process& proc) {
  char buf[512];
  bcopy(&msg, buf, sizeof(msg));
  if ((nsent = sendto(proc.socket(), buf, sizeof(msg), 
	  net.get_sockaddr_frompid(pid), 
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
  const std::vector<process>& plist = procmgr.process_list();

  // inform the i'th process that it's 
  // peer is thread with id 2*i
  //

  for (int i=0;i<nproc/2;++i) {
    message msg{PEER, i+nproc/2;};
    send_message(i, msg, plist[i]);
  }

  const int procsz = plist.size();
  std::random_device rd;
  std::uniform_int_distribution<int> dist{0,procsz};
  int randpid = dist(rd);
  set_message(COORDINATOR, randpid);
  return 1;
}
