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
