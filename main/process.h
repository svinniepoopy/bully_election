#ifndef __PROCESS_H__
#define __PROCESS_H__

#include "net_helper.h"
#include "message.h"
#include "message_handler.h"

/* process: wrapper to a thread
 *
 * runs a thread that sends and receives
 * messages. 
 */
class process {
  public:
    /* 
     * creates and runs a thread with a given id
     * 
     * @param in: thread id
     */
    process(int id);
    ~process();
    /* 
     * handles a character string of a given size
     *
     * @param msg: message received from the peer
     * @param len: received message length
     */
    int handle_msg(const message* msg, size_t len);
    int handle_msg_peer(const message* msg, const size_t len);
    int handle_msg_coordinator(const message* msg, const size_t len);
    int handle_msg_elect(const message* msg, const size_t len);
    int handle_msg_ping(const message* msg, const size_t len);
    int handle_msg_kill(const message* msg, const size_t len);

    void operator()();

    /* return the socket this thread is running on */
    int socket() const;
    /* return the socket path as string */
    static std::string get_named_socket(pid_t pid);

    bool is_coordinator;
    bool is_peer;
    bool is_client;
    int id;
    int npeers;
    int sockfd;
    net_helper net;	// helper methods for networking
    message_handler handler;	// helper methods to send messages
};

#endif
