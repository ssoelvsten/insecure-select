#include "../include.h"

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

  dprintf(STDOUT_FILENO, "bind(...)\n");
  // Bind to address
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port   = htons(PORT);
  inet_pton(AF_INET, ADDR, &(addr.sin_addr));
  memset(addr.sin_zero, '\0', sizeof addr.sin_zero);

  const socklen_t addrlen = sizeof addr;

  const int bind_code = bind(listener_fd, (struct sockaddr *) &addr, addrlen);
  if (bind_code < 0) { return bind_code; }

  dprintf(STDOUT_FILENO, "listen(...)\n");
  // Listen for new connections
  const int listen_code = listen(listener_fd, 2*CONN);
  if (listen_code < 0) { return listen_code; }

  return listener_fd;
}

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// Main program that only touches the public socket and leaks information as
/// it sends the received message back again.
////////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
  // Create a socket to listen for new connections
  const int listener_fd = make_listener();
  if (listener_fd < 0) { return listener_fd; }
  dprintf(STDOUT_FILENO, "Listening on: %s:%i\n\n", ADDR, PORT);

  // Keep track of all accepting (and still alive) connections.
  int nopen = 0;
  int open_fd[CONN];

  // Loop: wait for new connections and send received data back again.
  for (;;) {
    // Create `nfds` and `readfds` = { listener_fd, open_fd[..] }
    int nfds = listener_fd;

    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(listener_fd, &readfds);

    for (int i = 0; i < nopen; ++i) {
      const int fd = open_fd[i];
      nfds = nfds < fd ? fd : nfds;
      FD_SET(fd, &readfds);
    }
    nfds += 1;

    // Wait for something to do
    const int nready = select(nfds, &readfds, NULL, NULL, NULL);

    if (nready <= 0) {
      dprintf(STDERR_FILENO, "nready < 0\n");
      return -1;
    }

    // New data from current connections?
    for (int i = 0; i < nopen; ++i) {
      const int fd = open_fd[i];
      if (!FD_ISSET(fd, &readfds)) { continue; }

      char buffer[BUFF];
      const int nread = read(fd, &buffer, BUFF);

      if (nread < 0) { // Error?
      } else if (nread == 0) { // Connection closed...
        dprintf(STDOUT_FILENO, "  Closing   [%i] = (fd: %i)\n", i, fd);
        // Move the last active connection to 'i' to pack everything together
        nopen -= 1;
        if (nopen == 0) { continue; }

        open_fd[i] = open_fd[nopen];
        dprintf(STDOUT_FILENO, "  Moving    [%i] = [%i] = (fd: %i)\n", i, nopen, open_fd[i]);
        // Make sure that the swapped connection also is touched.
        i -= 1;

        // Close the connection to free the file descriptor for new connections
        close(fd);
      } else { // Data! Send it back!
        const int nwrite = send(fd, buffer, nread, 0);
        if (nwrite < 0) {
          dprintf(STDERR_FILENO, "nwrite < 0 for '%i'\n", fd);
        } else if (nwrite != nread) {
          dprintf(STDERR_FILENO, "nwrite != nread for '%i'\n", fd);
        }
      }
    }

    // New connection?
    if (FD_ISSET(listener_fd, &readfds) && nopen < CONN) {
      open_fd[nopen++] = accept(listener_fd, NULL, NULL);
      dprintf(STDOUT_FILENO, "  Accepting [%i] = (%i)\n", nopen-1, open_fd[nopen-1]);
    }
  }
}
