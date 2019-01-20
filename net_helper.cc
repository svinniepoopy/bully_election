#include "net_helper.h"

#include <string>
#include <stddef.h>
#include <errno.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>

int net_helper::make_sock() {
  int sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
  if (sockfd < 0) {
    std::cerr << "error creating socket\n";
    exit(-1);
  }
  return sockfd;
}


/* create a local socket and bind a name to it */
int net_helper::make_named_socket(int sockfd, const std::string& filename) {
  struct sockaddr_un name;
  name.sun_family = AF_LOCAL;
  /* Bind a name to the socket. */
  name.sun_family = AF_LOCAL;
  strncpy (name.sun_path, filename.c_str(), sizeof (name.sun_path));
  name.sun_path[sizeof (name.sun_path) - 1] = '\0';

  size_t size;
  size = (offsetof (struct sockaddr_un, sun_path)
          + strlen (name.sun_path));

  if (bind (sockfd, (struct sockaddr *) &name, size) < 0)
    {
      perror ("bind");
      exit (EXIT_FAILURE);
    }

  return sockfd;
}


