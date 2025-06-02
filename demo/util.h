#include<sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// ----------------------------------------------------------------------------
#define PORT_PUBLIC 2802
#define PORT_SECRET 1505

// ----------------------------------------------------------------------------
// Open a socket for `localhost:port` and wait for a connection is established
// with a client. This can be used to obtain the input/output streams used to
// send public and secret information.
//
// Based on Beej's beginner's guide for socket programming
//
//   https://beej.us/guide/bgnet/html/split-wide/system-calls-or-bust.html
int listen_localhost(short port)
{
  // Create address
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port   = htons(port);
  inet_pton(AF_INET, "127.0.0.1", &(addr.sin_addr));
  memset(addr.sin_zero, '\0', sizeof addr.sin_zero);

  socklen_t addrlen = sizeof addr;

  // Create socket and bind it to the address and listen until there is a
  // connection. Accept the incoming connection and return its file descriptor.
  const int socket_fd = socket(PF_INET, SOCK_STREAM, 0);

  const int bind_code = bind(socket_fd, (struct sockaddr *) &addr, addrlen);
  if (bind_code < 0) { return bind_code; }

  const int listen_code = listen(socket_fd, 2);
  if (listen_code < 0) { return listen_code; }

  struct sockaddr_storage accept_addr;
  int accept_addrlen = sizeof accept_addr;

  dprintf(STDOUT_FILENO, "Awaiting connection at '127.0.0.1:%i\n", port);
  const int conn_fd = accept(socket_fd, (struct sockaddr *) &accept_addr, &accept_addrlen);

  // Close connection such that restarting the program does not break?
  close(socket_fd);
  return conn_fd;
}
