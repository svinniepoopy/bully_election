/* file election_sim.cpp */
#include "election_sim.h"
#include "message.h"
#include "net_helper.h"
#include "process.h"
#include "message_handler.h"

#include <strings.h>
#include <algorithm>
#include <functional>
#include <thread>
#include <chrono>
#include <random>
#include <vector>
#include <iostream>

static int rand_in_range(int l, int h) {
  std::random_device rd;
  std::uniform_int_distribution<int> dist{l,h};
  return dist(rd);
}

int election_simulator::simulate(int nproc) {
  std::vector<std::thread> threads;
  for (int i=0;i<nproc;++i) {
    process p{i, nproc};
    threads.push_back(std::thread{p});
  }
  std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::detach));
  std::cerr << "launched " << threads.size() << " threads.\n";
  // inform the i'th thread that it's peer is thread with id 2*i
  net_helper net;
  int dest_peer_id, sockfd = net.make_sock();
  msg_handler handler; 
  for (int i=0;i<nproc/2;++i) {
    dest_peer_id = i+nproc/2;
    std::string sockpath = net.get_named_socket(i);
    message msg{message::PEER, dest_peer_id};
    std::this_thread::sleep_for(std::chrono::seconds(2));
    handler.send_message<message>(sockfd, sockpath.c_str(), &msg);
  }

  std::this_thread::sleep_for(std::chrono::seconds(10));

  int randpid = rand_in_range(0, threads.size());
  std::cout << "SENDING COORD/KILL to " << randpid << '\n';
  message msg_coord{message::COORDINATOR, randpid};
  handler.send_message<message>(sockfd,
      net.get_named_socket(randpid),
      msg_coord);
  /* TODO: allow to kill a process. must relect if the coordinator dies 
  message msg_kill{message::KILL, randpid};
  send_message(message::KILL,
      net.get_named_socket(randpid),
      msg_kill);
  */

  while (1) {
  }

  return 1;
}
