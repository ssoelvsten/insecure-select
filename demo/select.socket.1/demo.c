// --------------------------------------------------------------------------//
// Import relevant system calls
#include "../include.h"

int recv_line(int fd)
{
  char buffer[1] = { 'a' };
  int nread = -1;
  int errcode = 0;

  do {
    nread = recv(fd, buffer, 1, 0);
    errcode = nread < 0;
  } while (errcode == 0 && !(nread == 0 || buffer[0] == '\n'));

  return errcode;
}

// ----------------------------------------------------------------------------
// Function which leaks information about the state of the secret socket to the
// state of the public one.
int leak(const int p_fd) {
  // Open secret socket
  const int s_fd = listen_localhost(PORT_SECRET);
  if (s_fd < 0) { return s_fd; }

  dprintf(STDOUT_FILENO, "Connections established");
  sleep(3);
  dprintf(STDOUT_FILENO, " .");
  sleep(3);
  dprintf(STDOUT_FILENO, " .");
  sleep(3);
  dprintf(STDOUT_FILENO, " .\n");

  // Create `nfds`
  const int nfds = (s_fd < p_fd ? p_fd : s_fd) + 1;

  // Create `readfds` = { s_fd, p_fd }
  fd_set readfds;
  FD_ZERO(&readfds);
  FD_SET(s_fd, &readfds);
  FD_SET(p_fd, &readfds);

  // Use select
  const int nready = select(nfds, &readfds, NULL, NULL, NULL);

  // Read with priority given to `s_fd`
  int errcode = 0;
  if (nready <= 0) {
    return -1;
  } else if (FD_ISSET(s_fd, &readfds)) {
    errcode = recv_line(s_fd);
  } else { // if (FD_ISSET(p_fd, readfds))
    errcode = recv_line(p_fd);
  }
  close(s_fd);
  return errcode;
}

// ----------------------------------------------------------------------------
// Main program that only touches the public socket and leaks information as it
// sends the received message back again.
int main() {
  // Open public socket
  const int p_fd = listen_localhost(PORT_PUBLIC);
  if (p_fd < 0) { return p_fd; }

  // Run select example
  const int leak_code = leak(p_fd);
  if (leak_code != 0) { return leak_code; }

  // Return the rest on the public socket.
  char p_buffer[1024];
  const int p_nread = recv(p_fd, p_buffer, 1024, 0);
  if (p_nread < 0) { return -1; }

  const int p_written = send(p_fd, p_buffer, p_nread, 0);
  if (p_written < 0) { return -2; }

  // Finished example with success!
  close(p_fd);
  return 0;
}
