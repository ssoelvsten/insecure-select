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
/// Open a socket for `/tmp/socket_demo.<...>` and wait for a connection is
/// established from a client. This can be used to obtain the input/output
/// streams used to send public and secret information.
///
/// \note This one is set up to be used with `<threads.h>` to not block the main
///       thread.
////////////////////////////////////////////////////////////////////////////////
int listen_unix(const char* path)
{
  // Remove any leftover files from previous runs.
  remove(path);

  // Create socket for a unix stream.
  const int fd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (fd == -1) { return -1; }

  // Bind to address
  struct sockaddr_un addr;
  memset(&addr, 0, sizeof(struct sockaddr_un));
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, path, sizeof(addr.sun_path) - 1);

  const int addrlen = sizeof(struct sockaddr_un);

  // Bind to socket
  const int bind_errcode = bind(fd, (struct sockaddr *) &addr, addrlen);
  if (bind_errcode == -1) {
    close(fd);
    remove(path);
    return -1;
  }

  // Listen for new connections
  const int listen_errcode = listen(fd, 5);
  if (listen_errcode == -1) {
    close(fd);
    remove(path);
    return -1;
  }

  // Wait until someone connects and accept it.
  int out = accept(fd, NULL, NULL);
  close(fd);

  return out;
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
int connect_unix(const char* path)
{
  const int fd = socket(AF_UNIX, SOCK_STREAM, 0);

  struct sockaddr_un addr;
  memset(&addr, 0, sizeof(struct sockaddr_un));
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, path, sizeof(addr.sun_path) - 1);

  const int connect_errcode =
    connect(fd, (struct sockaddr *) &addr, sizeof(struct sockaddr_un));
  if (connect_errcode == -1) {
    close(fd);
    return -1;
  };
  return fd;
}

////////////////////////////////////////////////////////////////////////////////
/// Path for (public) Unix file through which the sockets communicate.
////////////////////////////////////////////////////////////////////////////////
const char* public_path = "/tmp/socket_demo.public";

////////////////////////////////////////////////////////////////////////////////
/// File descriptors for both ends of the public stream.
///
/// \note These are put here to ease cross-thread communication.
///////////////////////////////////////////////////////////////////////////////
int public_fd[2] = { -1, -1 };

int listen_unix__public(void*)
{
  public_fd[0] = listen_unix(public_path);
  return public_fd[0] < 0;
}

int connect_unix__public(void*)
{
  public_fd[1] = connect_unix(public_path);
}

////////////////////////////////////////////////////////////////////////////////
/// Path for (public) Unix file through which the sockets communicate.
////////////////////////////////////////////////////////////////////////////////
const char* secret_path = "/tmp/socket_demo.secret";

////////////////////////////////////////////////////////////////////////////////
/// File descriptors for both ends of the secret stream.
///
/// \note These are put here to ease cross-thread communication.
///////////////////////////////////////////////////////////////////////////////
int secret_fd[2] = { -1, -1 };

int listen_unix__secret(void*)
{
  secret_fd[0] = listen_unix(secret_path);
  return secret_fd[0] < 0;
}

int connect_unix__secret(void*)
{
  secret_fd[1] = connect_unix(secret_path);
}


////////////////////////////////////////////////////////////////////////////////
/// Repeat `recv` until encountering `\n`, the end of a string or the stream.
////////////////////////////////////////////////////////////////////////////////
int read_str(int fd, char* buffer, int length)
{
  int acc_nread = 0;

  while (acc_nread+1 < length) {
    char c    = 0;
    int nread = read(fd, &c, 1);

    // If at the end of the stream or file or a `\n`, then make it a `null`,
    // i.e. an end of string
    if (nread == 0 || c == '\n') {
      nread = 1;
      c = 0;
    }

    buffer[acc_nread] = c;
    acc_nread += nread;

    // Break if `c` is the end of the string
    if (c == 0) { return acc_nread; }

    // Break if an error occurred
    if (nread < 0) { return -1; }
  }
}
