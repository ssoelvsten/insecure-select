#include "../include.h"
#include <sys/un.h>
#include <threads.h>

// ----------------------------------------------------------------------------
unsigned char secret  = 0;

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

// ----------------------------------------------------------------------------
const char* sun_path = "/tmp/socket_demo";

int source_fd = -1;
int target_fd = -1;

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

// ----------------------------------------------------------------------------
int char_max = 256;

int fill_secret(void*)
{
  for (int i = 0; i <= secret; ++i) {
    const unsigned char i_char = 0;
    send(source_fd, &i_char, 1, 0);
  }
  return 0;
}

int fill_public(void*)
{
  for (int i = char_max-1; 0 <= i; --i) {
    const unsigned char i_char = i;
    send(source_fd, &i_char, 1, 0);
  }
  return 0;
}

int drop_public(void*)
{
  for (int i = 0; i < char_max; ++i) {
    unsigned char _;
    recv(target_fd, &_, 1, 0);
  }
  return 0;
}

// ----------------------------------------------------------------------------
// Main program that only touches the public socket and leaks information as it
// sends the received message back again.
int main(int argc, char* argv[])
{
  // Set up secret
  int secret_errcode = init_secret(argc, argv);
  if (secret_errcode) { return -1; }

  // Set up socket(s
  long unsigned int init_target_id;
  thrd_create(&init_target_id, init_target, NULL);
  sleep(1);
  init_source();
  thrd_join(init_target_id, NULL);
  if (source_fd == -1 || target_fd == -1) { return -1; }

  // Fill message queue with secret messages
  long unsigned int fill_secret_id;
  thrd_create(&fill_secret_id, fill_secret, NULL);
  thrd_join(fill_secret_id, NULL);

  sleep(1);

  // Fill message queue with public messages
  long unsigned int fill_public_id;
  thrd_create(&fill_public_id, fill_public, NULL);

  // Drop public number of messages
  long unsigned int drop_public_id;
  thrd_create(&drop_public_id, drop_public, NULL);
  thrd_join(drop_public_id, NULL);

  // NOTE: One has to join this late to mitigate the `send` from blocking
  thrd_join(fill_public_id, NULL);

  unsigned char leak;
  recv(target_fd, &leak, 1, 0);
  dprintf(STDOUT_FILENO, "leak = %i\n", leak);

  // Final clean up
  close(source_fd);
  close(target_fd);
  remove(sun_path);

  return 0;
}
