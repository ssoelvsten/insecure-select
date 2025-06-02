#include "../include.h"
#include "../util.h"

#include <threads.h>

////////////////////////////////////////////////////////////////////////////////
int fill_public(void*)
{
  for (int i = 0; i < 256; ++i) {
    const unsigned char i_char = i;
    send(source_fd, &i_char, 1, 0);
  }
  return 0;
}

int drop_secret(void*)
{
  for (unsigned char i = 0; i < secret; ++i) {
    unsigned char _;
    recv(target_fd, &_, 1, 0);
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
  long unsigned int init_target_id;
  thrd_create(&init_target_id, init_target, NULL);
  sleep(1);
  init_source();
  thrd_join(init_target_id, NULL);
  if (source_fd == -1 || target_fd == -1) { return -1; }

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
  recv(target_fd, &leak, 1, 0);
  dprintf(STDOUT_FILENO, "leak = %i\n", leak);

  // Final clean up
  close(source_fd);
  close(target_fd);
  remove(sun_path);

  return 0;
}
