#ifndef __ELECTION_SIM__
#define __ELECTION_SIM__

#include "message.h"
#include "process.h"

class election_simulator {
  public:
    int send_message(int, int, message, process&);
    int simulate(int);
};

#endif
