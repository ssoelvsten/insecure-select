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

  // Buffer for messages that have not yet been fully received (still waiting
  // for end of line or null-termination).
  int  recv_size[CONN];
  char recv_buff[CONN][BUFF];

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
      const int nmax  = BUFF - recv_size[i];
      const int nread = read(fd, &buffer, nmax);

      if (nread < 0) { // Error?
        dprintf(STDERR_FILENO, "  nread < 0 for '%i'\n", i, fd);
      } else if (nread == 0) { // Connection closed...
        // Close the connection to free the file descriptor for new connections
        dprintf(STDOUT_FILENO, "  Closing    [%i] = (fd: %i)\n", i, fd);
        close(fd);

        // Move the last active connection to 'i' to pack everything together
        nopen -= 1;
        if (nopen == 0 || i == nopen) { continue; }

        open_fd[i]   = open_fd[nopen];
        recv_size[i] = recv_size[nopen];
        memcpy(recv_buff[i], recv_buff[nopen], recv_size[nopen]);

        dprintf(STDOUT_FILENO, "  Moving     [%i] = [%i] = (fd: %i)\n", i, nopen, open_fd[i]);
        // Make sure that the swapped connection also is touched.
        i -= 1;
      } else { // Data!
        int buffer_idx = 0;
        while (buffer_idx < nread) {
          // Copy into buffer until end-of-line/null termination
          int eom_idx = -1;
          for (; buffer_idx < nread; ++buffer_idx) {
            const int recv_buff_idx = recv_size[0] + buffer_idx;
            recv_buff[0][recv_buff_idx] = buffer[buffer_idx];

            if (buffer[buffer_idx] == '\n' || buffer[buffer_idx] == '\0') {
              eom_idx = recv_buff_idx;
              break;
            }
          }
          recv_size[0] += buffer_idx;
          buffer_idx += 1;

          // Send message back if complete!
          if (eom_idx != -1) {
            const int nwrite = send(fd, recv_buff[0], eom_idx+1, 0);
            recv_size[0] = 0;
            if (nwrite < 0) {
              dprintf(STDERR_FILENO, "  nwrite < 0 for '%i'\n", fd);
            } else if (nwrite != eom_idx+1) {
              dprintf(STDERR_FILENO, "  nwrite != nread for '%i'\n", fd);
            }
          }
        }
      }
    }

    // New connection?
    if (FD_ISSET(listener_fd, &readfds) && nopen < CONN) {
      recv_size[nopen] = 0;
      open_fd[nopen++] = accept(listener_fd, NULL, NULL);
      dprintf(STDOUT_FILENO, "  Accepting [%i] = (%i)\n", nopen-1, open_fd[nopen-1]);
    }
  }
}
