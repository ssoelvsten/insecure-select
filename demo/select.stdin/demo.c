#include "../include.h"

////////////////////////////////////////////////////////////////////////////////
/// Function which leaks information about the state of `stdin` into the state
/// of the public file, `public.txt`, with the given file descriptor `p_fd`.
////////////////////////////////////////////////////////////////////////////////
int leak(const int p_fd)
{
  // Open secret file (stdin)
  const int s_fd = STDIN_FILENO;

  // Create `nfds` (always the public file, since STDIN_FILENO = 0)
  const int nfds = p_fd + 1;

  // Create `readfds` = { s_fd, p_fd }
  fd_set readfds;
  FD_ZERO(&readfds);
  FD_SET(s_fd, &readfds);
  FD_SET(p_fd, &readfds);

  // Use select
  const int nready = select(nfds, &readfds, NULL, NULL, NULL);

  // Read with priority given to `s_fd`
  if (nready <= 0) {
    return -1;
  } else if (FD_ISSET(s_fd, &readfds)) {
    char s_buffer[8];
    const int s_nread = read(s_fd, s_buffer, 8);
    return 0;
  } else { // if (FD_ISSET(p_fd, readfds))
    char p_buffer[3];
    const int p_nread = read(p_fd, p_buffer, 3);
    return p_nread != 3;
  }
}

////////////////////////////////////////////////////////////////////////////////
/// Main program that only touches the public file, `public.txt`, but is able to
/// deduce whether `stdin` has content or not based on what are the next tokens
/// in the public file.
////////////////////////////////////////////////////////////////////////////////
int main()
{
  // Open public file
  const int p_fd = open("./public.txt", O_RDONLY);
  if (p_fd == -1) { return -1; };

  // Run select example
  const int leak_code = leak(p_fd);
  if (leak_code != 0) { return leak_code; }

  // Infer whether `STDIN_FILENO` is empty or not based on the state of
  // `public.txt`
  char p_buffer[3];
  const int p_nread = read(p_fd, p_buffer, 3);
  if (p_nread != 3) { return -3; }

  dprintf(STDOUT_FILENO, "State of 'public.txt', i.e. is 'stdin' empty?\n  ");
  const int p_written = write(STDOUT_FILENO, p_buffer, 3);
  dprintf(STDOUT_FILENO, "\n");
  if (p_written != 3) { return -3; }

  // Finished example with success!
  return 0;
}
