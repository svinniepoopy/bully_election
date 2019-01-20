#ifndef __NET_HELPER__
#define __NET_HELPER__

#include <string>

class net_helper {
  public:
    int make_sock();
    int make_named_socket(int, const std::string&);
};

#endif
