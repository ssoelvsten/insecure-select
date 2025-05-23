// --------------------------------------------------------------------------//
// Import relevant system calls

#include <unistd.h>
/* File descriptors for `stdin`, `stdout`, and `stderr` are provided with the
 * following macros:

     STDIN_FILENO, STDOUT_FILENO, and STDERR_FILENO

 * It is in general not recommended to mix use of the two, so we probably ought
 * to use these throughout. For more details:

     https://www.man7.org/linux/man-pages/man3/stdin.3.html

 * One can read from and write to these using the following two functions:

     read(int fd,           // File descriptor for input
          void buf[.count], // Buffer to write into
          size_t            // Amount to read
          )

     write(int fd,                 // File descriptor for output
           const void buf[.count], // Buffer with content to be written
           size_t count            // Size of buffer
           )

 * It should be noted that `read` on files on the disk is always non-blocking.
 * It is even so after having reached the end of the file.
 *
 * For more details, please see:

     https://man7.org/linux/man-pages/man2/read.2.html
     https://man7.org/linux/man-pages/man2/write.2.html
*/

#include <fcntl.h>
/* The `open()` function on file descriptors. */

#include <stdio.h>
/* Slightly easier than `write()` is to use `dprintf` which also works with a
 * file descriptor.

     dprintf(int fd,                  // File descriptor
             const char *char format, // String formatting
             ...                      // Values to be formatted.
             )
 */

#include <stdlib.h>
/* A few quality-of-life operations such as `exit()`. */

#include <sys/select.h>
/* The select operation that works as follows:

     // Waits for `timeout` amount of time to then return the number files with
     // new content. Which ones can be read from/written to is available in the
     // flags of the three `fd_set`'s.
     int select(int nfds,               // max(file descriptors)+1
                fd_set *readfds,        // Watched for 'ready for reading'
                fd_set *writefds,       // Watched for 'ready for writing'
                fd_set *exceptfds,      // Watched for "exceptional conditions"
                struct timeval *timeout // Time to wait until return
                )

 * For more details:

     https://man7.org/linux/man-pages/man2/select.2.html
*/

// ----------------------------------------------------------------------------
// Function which leaks information about the state of `stdin` into the state
// of the public file, `public.txt`, with the given file descriptor `p_fd`.
int leak(const int p_fd) {
  // Open secret file (stdin)
  const int s_fd = STDIN_FILENO;

  // Create `nfds`
  const int nfds = p_fd + 1;

  // Create `readfds` = { s_fd, p_fd }
  fd_set readfds;
  FD_ZERO(&readfds);
  FD_SET(STDIN_FILENO, &readfds);
  FD_SET(p_fd, &readfds);

  // Use select
  const int nready = select(nfds, &readfds, NULL, NULL, NULL);

  // Read with priority given to `s_fd`
  if (nready <= 0) {
    return -1;
  } else if (FD_ISSET(STDIN_FILENO, &readfds)) {
    char s_buffer[8];
    const int s_nread = read(STDIN_FILENO, s_buffer, 8);
    return 0;
  } else { // if (FD_ISSET(p_fd, readfds))
    char p_buffer[3];
    const int p_nread = read(p_fd, p_buffer, 3);
    return p_nread != 3;
  }
}

// ----------------------------------------------------------------------------
// Main program that only touches the public file, `public.txt`, but is able to
// deduce whether `stdin` has content or not based on what are the next tokens
// in the public file.
int main() {
  // Open public file
  const int p_fd = open("./public.txt", O_RDONLY);
  if (p_fd == -1) { return -1; };

  // Run select example
  const int leak_code = leak(p_fd);
  if (leak_code != 0) { return leak_code; }

  // Infer whether "./secret.txt" is empty or not based on the state of public.txt
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
