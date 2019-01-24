#include "message_handler.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

template<typename T>
int msg_handler::send_message(
    int sockfd, const char* const peer_sockname, T* msg_out) {
  char buf[32];
  bcopy(&msg_out, buf, sizeof(msg_out));
  struct sockaddr_un name;
  name.sun_family = AF_LOCAL;
  strncpy(name.sun_path, peer_sockname, sizeof(name.sun_path));
  name.sun_path[sizeof(name.sun_path)-1] = '\0';
  ssize_t nsent = 0;
  std::cout << "send_message to: " << peer_sockname << '\n';
  if ((nsent = sendto(
	  sockfd, buf, sizeof(msg_out),
	  0,
	  (struct sockaddr*)&name,
	  sizeof(struct sockaddr_un))) != sizeof(msg_out)) {
    return -1;
  }
  return 0;
}
