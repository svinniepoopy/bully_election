#ifndef __PROCESS_H__
#define __PROCESS_H__

#include "net_helper.h"
#include "message.h"

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

    /* return the socket this thread is running on */
    int socket() const;
    /* return the socket path as string */
    static std::string get_named_socket(pid_t pid);

    friend struct run_proc;
    int id;
    int sockfd;
    net_helper net;	// helper methods for networking
};

/* run_proc is the call functor for the thread to run */
struct run_proc {
  run_proc(int id, process* procs):
    id{id},
    proc{procs}
  {}
  void operator()() const;
  int id;
  process* proc;
};

#endif
