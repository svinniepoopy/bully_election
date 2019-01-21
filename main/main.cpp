#include "election_sim.h"
#include "process.h"

#include <iostream>

int main(int argc, char** argv) {
  if (argc < 2) { std::cerr << "Usage: run #num-threads\n"; return -1; }
  int nthreads = atoi(argv[1]);
  if (nthreads%2 != 0) {
    std::cerr << "num-threads must be even\n";
    return -1;
  }
  election_simulator sim;
  sim.simulate(nthreads);
  return 0;
}
