#include <arpa/inet.h>
#include <netinet/in.h>

////////////////////////////////////////////////////////////////////////////////
/// Port for new connections.
const int   PORT = 1505;

/// Address for new connections (localhost).
const char* ADDR = "127.0.0.1";

/// Maximum number of open connections served at the same time.
const char  CONN = 5;

/// Buffer size for received messages. If a client sends more than this in a
/// single go, then it will be handled in two rounds. That is not at all a good
/// way of handling it. But, it keeps things simple.
const int   BUFF = 128;

////////////////////////////////////////////////////////////////////////////////
/// Open a socket for `localhost:port` that listens for new connections.
///
/// \note Based on Beej's beginner's guide for socket programming:
///       https://beej.us/guide/bgnet/html/split-wide/index.html
////////////////////////////////////////////////////////////////////////////////
int make_listener()
{
  // Create socket for an internet stream.
  const int listener_fd = socket(PF_INET, SOCK_STREAM, 0);
  if (listener_fd < 0) { return listener_fd; }

  // TODO: setsocketopt(listener_fd, SOL_SOCKET, SO_LINGER/SO_REUSEADDR, ...)
  //
  // Ensure the address and port is not locked by a previous incarnation of
  // this server.

  // Bind to address
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port   = htons(PORT);
  inet_pton(AF_INET, ADDR, &(addr.sin_addr));
  memset(addr.sin_zero, '\0', sizeof addr.sin_zero);

  const socklen_t addrlen = sizeof addr;

  const int bind_code = bind(listener_fd, (struct sockaddr *) &addr, addrlen);
  if (bind_code < 0) { return bind_code; }

  // Listen for new connections
  const int listen_code = listen(listener_fd, 2*CONN);
  if (listen_code < 0) { return listen_code; }

  return listener_fd;
}
