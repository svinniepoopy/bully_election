#ifndef __PROCESS_H__
#define __PROCESS_H__

#include "net_helper.h"

#include <iostream>

/* process class which is holds a reference
 * to a thread
 */
class process {
  public:
    process(int);
    struct run_proc {
      void operator()() const;
    };
    int id;
    net_helper net;
};

#endif
