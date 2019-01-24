
#ifndef __MSG_HANDLER__
#define __MSG_HANDLER__

/* message handler: send/recv messags to/from socket
 */
class msg_handler {
  template<typename T>
    int send_message(int sock, const char* sockaddr, const T* msg);
  template<typename T>
    int send_message(int sock, const char* sockaddr, const T& msg) {
      return send_message(sock, sockaddr, &msg);
    }
};

#endif	// MSG_HANDLER	
