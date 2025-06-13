#include "../include.h"
#include "../util_echo.h"

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

  // Keep track of all accepted (and still alive) connections.
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

    // Throttle the server such that attacks are easier
    sleep(2);

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
        dprintf(STDERR_FILENO, "  nread < 0 for '%i'\n", i, fd);
      } else if (nread == 0) { // Connection closed...
        dprintf(STDOUT_FILENO, "  Closing   [%i] = (fd: %i)\n", i, fd);
        // Move the last active connection to 'i' to pack everything together
        nopen -= 1;
        if (nopen == 0 || i == nopen) { continue; }

        open_fd[i] = open_fd[nopen];
        dprintf(STDOUT_FILENO, "  Moving    [%i] = [%i] = (fd: %i)\n", i, nopen, open_fd[i]);
        // Make sure that the swapped connection also is touched.
        i -= 1;

        // Close the connection to free the file descriptor for new connections
        close(fd);
      } else { // Data! Send it back!
        const int nwrite = send(fd, buffer, nread, 0);
        if (nwrite < 0) {
          dprintf(STDERR_FILENO, "  nwrite < 0 for '%i'\n", fd);
        } else if (nwrite != nread) {
          dprintf(STDERR_FILENO, "  nwrite != nread for '%i'\n", fd);
        }
      }

      break;
    }

    // New connection?
    if (FD_ISSET(listener_fd, &readfds) && nopen < CONN) {
      open_fd[nopen++] = accept(listener_fd, NULL, NULL);
      dprintf(STDOUT_FILENO, "  Accepting [%i] = (%i)\n", nopen-1, open_fd[nopen-1]);
    }
  }
}

