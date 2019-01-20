#ifndef __MESSAGE_H__
#define __MESSAGE_H__

struct message {
  enum message_type {PEER, COORDINATOR, ELECT, OK};
  message_type type;
  int peer_id;
};

#endif
