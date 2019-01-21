#ifndef __NET_HELPER__
#define __NET_HELPER__

#include <sys/types.h>
#include <string>

class net_helper {
  public:
    int make_sock();
    std::string get_named_socket(pid_t pid);
    int make_named_socket(int, const char*); 
};

#endif
