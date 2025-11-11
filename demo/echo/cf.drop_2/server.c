#include "../../include.h"
#include "../server_util.h"

////////////////////////////////////////////////////////////////////////////////
/// Echo server that each round (1) drops data from the first available
/// connection and then (2) responds to everyone with all their remaining data.
////////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
  // Create a socket to listen for new connections
  const int listener_fd = make_listener();
  if (listener_fd < 0) { return listener_fd; }
  dprintf(STDOUT_FILENO, "Listening on: %s:%i\n\n", ADDR, PORT);

  // Keep track of all accepted (and still alive) connections.
  int nopen = 0;
  int open_fd[CONN];

  // Loop: wait for new connections and send received data back again.
  for (;;) {
    // Throttle the server such that attacks are easier
    sleep(2);

    // First round: drop data from first connection!
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
    int nready = select(nfds, &readfds, NULL, NULL, NULL);

    if (nready <= 0) {
      dprintf(STDERR_FILENO, "nready < 0\n");
      return -1;
    }

    // New connection?
    if (FD_ISSET(listener_fd, &readfds) && nopen < CONN) {
      open_fd[nopen++] = accept(listener_fd, NULL, NULL);
      dprintf(STDOUT_FILENO, "  Accepting [%i] = (%i)\n", nopen-1, open_fd[nopen-1]);
      continue;
    }

    // Drop the first message from the first open connection
    for (int i = 0; i < nopen; ++i) {
      const int fd = open_fd[i];
      if (!FD_ISSET(fd, &readfds)) { continue; }

      char buffer[BUFF];
      const int nread = read(fd, &buffer, BUFF);

      if (nread < 0) { // Error?
        dprintf(STDERR_FILENO, "  nread < 0 for '%i'\n", i, fd);
      } else if (nread == 0) { // Connection closed...
        close_accepted(open_fd, &nopen, &i);
      } else {
        // Data! Drop and break.
        break;
      }
    }

    // Restart if all connections are closed.
    if (nopen == 0) { continue; }

    // Throttle the server such that attacks are easier
    sleep(2);

    // Second round: read and respond to all connections.
    nfds = 0;

    FD_ZERO(&readfds);
    for (int i = 0; i < nopen; ++i) {
      const int fd = open_fd[i];
      nfds = nfds < fd ? fd : nfds;
      FD_SET(fd, &readfds);
    }
    nfds += 1;

    // Wait for something to do
    nready = select(nfds, &readfds, NULL, NULL, NULL);

    for (int i = 0; i < nopen; ++i) {
      const int fd = open_fd[i];
      if (!FD_ISSET(fd, &readfds)) { continue; }

      char buffer[BUFF];
      const int nread = read(fd, &buffer, BUFF);

      if (nread < 0) { // Error?
        dprintf(STDERR_FILENO, "  nread < 0 for '%i'\n", i, fd);
      } else if (nread == 0) { // Connection closed...
        close_accepted(open_fd, &nopen, &i);
      } else { // Data! Send it back!
        const int nwrite = send(fd, buffer, nread, 0);
        if (nwrite < 0) {
          dprintf(STDERR_FILENO, "  nwrite < 0 for '%i'\n", fd);
        } else if (nwrite != nread) {
          dprintf(STDERR_FILENO, "  nwrite != nread for '%i'\n", fd);
        }
      }
    }
  }
}

