#ifndef __PROCESS_H__
#define __PROCESS_H__

#include "net_helper.h"

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
     * @param buf: message received from the peer
     * @param len: message length
     */
    void handle_msg(const char* buf, size_t len);
    /* return the socket this thread is running on */
    int socket() const;
    /* return the socket path as string */
    static std::string get_named_socket(pid_t pid);
    /* run_proc is the call functor for the thread to run */
    struct run_proc {
      run_proc(int id):id{id} {}
      void operator()() const;
      int id;
    };
    int id;
    net_helper net;	// helper methods for networking
};

#endif
