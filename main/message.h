#ifndef __MESSAGE_H__
#define __MESSAGE_H__

struct message {
  enum message_type {
    PEER, 
    COORDINATOR, 
    ELECT, 
    OK, 
    OKACK,
    KILL
  };
  message(const message_type& t, const int& id):
    type{t},
    peer_id{id}
  {}
  message_type type;
  int peer_id;
};

#endif
