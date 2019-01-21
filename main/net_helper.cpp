#include "net_helper.h"

#include <string>
#include <sstream>
#include <stddef.h>
#include <errno.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/socket.h>

int net_helper::make_sock() {
  int sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
  if (sockfd < 0) {
    perror("socket");
    exit(-1);
  }
  return sockfd;
}

std::string net_helper::get_named_socket(pid_t pid) {
  std::stringstream ss;
  ss << "/tmp/test_sim__";
  ss << "_";
  ss << pid; 
  return ss.str(); 
}

/* create a local socket and bind a name to it */
int net_helper::make_named_socket(int sockfd, const char* filename) {
  struct sockaddr_un name;
  /* Bind a name to the socket. */
  name.sun_family = AF_LOCAL;
  strncpy (name.sun_path, filename, sizeof (name.sun_path));
  name.sun_path[sizeof (name.sun_path) - 1] = '\0';

  size_t size;
  size = (offsetof (struct sockaddr_un, sun_path)
          + strlen (name.sun_path));

  if (bind (sockfd, (struct sockaddr *) &name, size) < 0)
    {
      perror("bind");
      exit(EXIT_FAILURE);
    }

  return sockfd;
}
