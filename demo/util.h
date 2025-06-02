#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/un.h>

////////////////////////////////////////////////////////////////////////////////
/// Highly secret value that should not in any way leak to `stdout` (hint, hint)
////////////////////////////////////////////////////////////////////////////////
unsigned char secret  = 0;

////////////////////////////////////////////////////////////////////////////////
/// Initialises `secret` with the value from the command line arguments.
////////////////////////////////////////////////////////////////////////////////
int init_secret(int argc, char* argv[])
{
  if (argc != 2) {
    dprintf(STDERR_FILENO, "Please provide a single 'secret' value\n");
    return -1;
  }

  char* strtol_end = NULL;
  errno = 0;
  secret = strtol(argv[1], &strtol_end, 10);

  return errno;
}


////////////////////////////////////////////////////////////////////////////////
/// Port for public information
////////////////////////////////////////////////////////////////////////////////
const int PORT_PUBLIC 2802;

////////////////////////////////////////////////////////////////////////////////
/// Port for secret information
////////////////////////////////////////////////////////////////////////////////
const int PORT_SECRET 1505;

////////////////////////////////////////////////////////////////////////////////
/// Open a socket for `localhost:port` and wait for a connection is established
/// with a client. This can be used to obtain the input/output streams used to
/// send public and secret information.
///
/// \note Based on Beej's beginner's guide for socket programming:
///       https://beej.us/guide/bgnet/html/split-wide/system-calls-or-bust.html
////////////////////////////////////////////////////////////////////////////////
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


////////////////////////////////////////////////////////////////////////////////
/// Path for unix file through which the sockets communicate
////////////////////////////////////////////////////////////////////////////////
const char* sun_path = "/tmp/socket_demo";

////////////////////////////////////////////////////////////////////////////////
/// File descriptor with the `source` end of the (technically bi-directional)
/// stream.
///
/// \note These are put here to ease cross-thread communication.
///////////////////////////////////////////////////////////////////////////////
int source_fd = -1;

////////////////////////////////////////////////////////////////////////////////
/// File descriptor with the `target` end of the (technically bi-directional)
/// stream.
///
/// \note These are put here to ease cross-thread communication.
///////////////////////////////////////////////////////////////////////////////
int target_fd = -1;

////////////////////////////////////////////////////////////////////////////////
/// Open a socket for `/tmp/socket_demo` and wait for a connection is
/// established from a client. This can be used to obtain the input/output
/// streams used to send public and secret information.
///
/// \note This one is set up to be used with `<threads.h>` to not block the main
///       thread.
////////////////////////////////////////////////////////////////////////////////
int init_target(void*)
{
  const int fd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (fd == -1) { return -1; }

  struct sockaddr_un addr;
  memset(&addr, 0, sizeof(struct sockaddr_un));
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, sun_path, sizeof(addr.sun_path) - 1);

  const int bind_errcode =
    bind(fd, (struct sockaddr *) &addr, sizeof(struct sockaddr_un));
  if (bind_errcode == -1) {
    close(fd);
    remove(sun_path);
    return -1;
  }

  const int listen_errcode = listen(fd, 5);
  if (listen_errcode == -1) {
    close(fd);
    remove(sun_path);
    return -1;
  }

  target_fd = accept(fd, NULL, NULL);
  close(fd);

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
/// Connect a socket for `/tmp/socket_demo`. This can be used to obtain the
/// input/output streams used to send public and secret information.
///
/// \note This will fail if the other side is not already `listen`ing.
///
/// \note This one is set up to be used with `<threads.h>` to not block the main
///       thread.
////////////////////////////////////////////////////////////////////////////////
int init_source()
{
  source_fd = socket(AF_UNIX, SOCK_STREAM, 0);

  struct sockaddr_un addr;
  memset(&addr, 0, sizeof(struct sockaddr_un));
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, sun_path, sizeof(addr.sun_path) - 1);

  const int connect_errcode =
    connect(source_fd, (struct sockaddr *) &addr, sizeof(struct sockaddr_un));
  if (connect_errcode == -1) {
    close(source_fd);
    source_fd = -1;
    return -1;
  };
  return 0;
}


////////////////////////////////////////////////////////////////////////////////
/// Repeat `recv` until encountering `\n`, the end of stream, or an error.
///
/// \todo output into an actual buffer?
////////////////////////////////////////////////////////////////////////////////
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
