#include "../include.h"
#include "../util_echo.h"

////////////////////////////////////////////////////////////////////////////////
/// Echo server that leaks information by only serving one at a time while also
/// sending an incrementing counter (clock value) back again.
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

  // Keep track of the number of select statements. This provides you with a 'clock'.
  int select_count = 0;

  // Loop: wait for new connections and send received data back again.
  for (;;) {
    // Throttle the server such that attacks are easier
    sleep(2);

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
    select_count += 1;

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

      if (nread < 0) {
        // Error?
        dprintf(STDERR_FILENO, "  nread < 0 for '%i'\n", i, fd);
      } else if (nread == 0) {
        // Connection closed...
        close_accepted(open_fd, &nopen, &i);
      } else {
        // Data! Send back message '<sc_prefix>        <message>'
        const int sc_prefix = 15;
        char send_buffer[sc_prefix + BUFF];

        const int sc_write = sprintf(send_buffer, "%i", select_count);
        for (int i = sc_write; i < sc_prefix; ++i) { send_buffer[i] = ' '; }
        memcpy(send_buffer + sc_prefix, buffer, nread);

        const int nwrite = send(fd, send_buffer, nread + sc_prefix, 0);
        if (nwrite < 0) {
          dprintf(STDERR_FILENO, "  nwrite < 0 for '%i'\n", fd);
        } else if (nwrite != nread + sc_prefix) {
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

