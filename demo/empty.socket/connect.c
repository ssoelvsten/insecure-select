#include "../include.h"
#include "../util_unix.h"

#include <string.h>

////////////////////////////////////////////////////////////////////////////////
/// Main program that connects to the given socket
////////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
  // Read 'secret' or 'public' from command line arguments
  if (argc != 2) {
    dprintf(STDERR_FILENO, "Please provide either 'public' or 'secret'\n");
    return -1;
  }

  const char* path = !strcmp(argv[1], "secret") ? secret_path : public_path;

  // Set up socket
  int fd = connect_unix(path);
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
      send(fd, buffer, buffer_end, 0);
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
