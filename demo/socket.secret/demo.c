#include "../include.h"
#include "../util.h"

#include <threads.h>

const int char_max = 256;

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
