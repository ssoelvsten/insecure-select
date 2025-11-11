#include "../../include.h"
#include "../../util.h"
#include "../unix_util.h"

#include <string.h>

////////////////////////////////////////////////////////////////////////////////
/// Function which leaks information about the state of the secret socket to the
/// state of the public one.
////////////////////////////////////////////////////////////////////////////////
int leak(const int p_fd, const int s_fd)
{
  // Create `nfds`
  const int nfds = (s_fd < p_fd ? p_fd : s_fd) + 1;

  // Create `readfds` = { s_fd, p_fd }
  fd_set readfds;
  FD_ZERO(&readfds);
  FD_SET(p_fd, &readfds);
  FD_SET(s_fd, &readfds);

  // Use select
  const int nready = select(nfds, &readfds, NULL, NULL, NULL);

  // Read with priority given to `s_fd`
  int errcode = 0;
  if (nready <= 0) {
    return -1;
  } else if (FD_ISSET(s_fd, &readfds)) {
    char buffer[128];
    int nread = read_str(s_fd, buffer, 128);

    errcode = nread < 0;
  } else { // if (FD_ISSET(p_fd, readfds))
    char buffer[128];
    int nread = read_str(p_fd, buffer, 128);

    errcode = nread < 0;
  }
  return errcode;
}

////////////////////////////////////////////////////////////////////////////////
/// Main program that only touches the public socket and leaks information as it
/// sends the received message back again.
////////////////////////////////////////////////////////////////////////////////
int main()
{
  // Set up sockets
  long unsigned int listen_public_id;
  thrd_create(&listen_public_id, listen_unix__public, NULL);

  long unsigned int listen_secret_id;
  thrd_create(&listen_secret_id, listen_unix__secret, NULL);

  thrd_join(listen_public_id, NULL);
  if (public_fd[0] == -1) { return -1; }

  thrd_join(listen_secret_id, NULL);
  if (secret_fd[0] == -1) { return -1; }

  // Give humans some time to provide some inputs in either socket.
  dprintf(STDOUT_FILENO, "Connections established");
  sleep(3);
  dprintf(STDOUT_FILENO, " .");
  sleep(3);
  dprintf(STDOUT_FILENO, " .");
  sleep(3);
  dprintf(STDOUT_FILENO, " .\n");

  // Run select example
  const int leak_errcode = leak(public_fd[0], secret_fd[0]);
  if (leak_errcode != 0) { return leak_errcode; }

  // Echo the next content on the public channel.
  char buffer[128];
  const int nread = read_str(public_fd[0], buffer, 128);
  if (nread < 0) { return -1; }

  const int nwrite = send(public_fd[0], buffer, nread, 0);
  if (nwrite < 0) { return -2; }

  // Finished example with success!
  return 0;
}
