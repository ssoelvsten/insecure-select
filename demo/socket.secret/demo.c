#include "../include.h"
#include "../util.h"

const int char_max = 256;

int fill_secret(void*)
{
  for (int i = 0; i <= secret; ++i) {
    const unsigned char i_char = 0;
    send(public_fd[1], &i_char, 1, 0);
  }
  return 0;
}

int fill_public(void*)
{
  for (int i = char_max-1; 0 <= i; --i) {
    const unsigned char i_char = i;
    send(public_fd[1], &i_char, 1, 0);
  }
  return 0;
}

int drop_public(void*)
{
  for (int i = 0; i < char_max; ++i) {
    unsigned char _;
    recv(public_fd[0], &_, 1, 0);
  }
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
/// Main program that only touches the public socket and leaks information as it
/// sends the received message back again.
////////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
  // Set up secret
  int secret_errcode = init_secret(argc, argv);
  if (secret_errcode) { return -1; }

  // Set up socket(s
  long unsigned int listen_public_id;
  thrd_create(&listen_public_id, listen_unix__public, NULL);
  sleep(1);
  connect_unix__public(NULL);
  thrd_join(listen_public_id, NULL);
  if (public_fd[0] == -1 || public_fd[1] == -1) { return -1; }

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
  recv(public_fd[0], &leak, 1, 0);
  dprintf(STDOUT_FILENO, "leak = %i\n", leak);

  return 0;
}
