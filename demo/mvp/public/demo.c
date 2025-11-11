#include "../../include.h"
#include "../unix_util.h"

////////////////////////////////////////////////////////////////////////////////
int fill_public(void*)
{
  for (int i = 0; i < 256; ++i) {
    const unsigned char i_char = i;
    send(public_fd[1], &i_char, 1, 0);
  }
  return 0;
}

int drop_secret(void*)
{
  for (unsigned char i = 0; i < secret; ++i) {
    unsigned char _;
    recv(public_fd[0], &_, 1, 0);
  }
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

  // Fill message queue with public messages
  long unsigned int fill_public_id;
  thrd_create(&fill_public_id, fill_public, NULL);
  thrd_join(fill_public_id, NULL);

  // Drop secret number of messages
  long unsigned int drop_secret_id;
  thrd_create(&drop_secret_id, drop_secret, NULL);
  thrd_join(drop_secret_id, NULL);

  // NOTE: Blocking Label
  //   Reaching this point of execution only reveals that secret < 256

  unsigned char leak;
  recv(public_fd[0], &leak, 1, 0);
  dprintf(STDOUT_FILENO, "leak = %i\n", leak);

  return 0;
}
