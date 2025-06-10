#include "../include.h"
#include "../util.h"
#include "../util_echo.h"

#include <string.h>

////////////////////////////////////////////////////////////////////////////////
/// Send each byte one by one each 0.2s.
////////////////////////////////////////////////////////////////////////////////
int slow_send(int fd, char* buf, size_t size, int flags)
{
  dprintf(STDOUT_FILENO, "   : ");
  for (int i = 0; i < size; ++i) {
    send(fd, buf + i, 1, 0);
    dprintf(STDOUT_FILENO, ".");
    usleep(200'000);
  }
  dprintf(STDOUT_FILENO, "\n");
}

////////////////////////////////////////////////////////////////////////////////
/// Main program that connects to the given socket and forwards STDIN.
////////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
  // Set up socket
  int fd = make_connection();
  if (fd < 0) { return fd; }

  dprintf(STDOUT_FILENO, "Connections established...\n");

  // eternal loop:
  // >> : <to be sent msg>
  // << : <returned msg>
  for (;;) {
    dprintf(STDOUT_FILENO, ">> : ");

    // Create `nfds` (where `fd` is guaranteed to be the largest descriptor)
    const int nfds = fd+1;

    // Create `readfds` = { STDIN_FILENO, fd }
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);
    FD_SET(fd, &readfds);

    // Create `writefds` = { fd, STDOUT_FILENO }
    fd_set writefds;
    FD_ZERO(&writefds);
    FD_SET(fd, &writefds);
    FD_SET(STDOUT_FILENO, &writefds);

    // Create `timeval` = 1000s
    struct timeval timeout;
    timeout.tv_sec  = 1000;
    timeout.tv_usec = 0;

    // Wait for inputs or outputs
    const int nready = select(nfds, &readfds, NULL, NULL, &timeout);

    // Send user input to listener (blocks until `\n`)
    if (FD_ISSET(STDIN_FILENO, &readfds)) {
      char buffer[128];
      const int buffer_end = read_str(STDIN_FILENO, buffer, 128);
      slow_send(fd, buffer, buffer_end, 0);
    }

    // Handle returned messages to be displayed to the user
    if (FD_ISSET(fd, &readfds)) {
      char buffer[128];
      const int buffer_end = read(fd, buffer, 128);

      if (buffer_end > 0) {
        // Overwrite '>> : ' by '\r' and then write the received message
        dprintf(STDOUT_FILENO, "\r<< : %s\n", buffer);
      } else {
        // Terminate program if connection closes (or errors)
        dprintf(STDOUT_FILENO, "\n");
        return 0;
      }
    }
  }
}
