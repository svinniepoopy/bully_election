#ifndef __PROCESS_MGR___
#define __PROCESS_MGR___

#include <vector>
#include <thread>

#include "process.h"

class process_mgr {
  public:
    /* create and launch n threads in serial 
     */
    int launch(int nthreads);
    void destroy(pid_t pid);
    const std::vector<process>& process_list() const;
    std::vector<process> threads;
};

#endif
