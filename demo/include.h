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

 * Also includes file handling such as file deletion

     remove(const char* file_path)
 */

#include<sys/socket.h>
/* Unix sockets and their related functions. In particular, the server would
 * use the following operations to set up a socket:

     // Create a new socket at an address (IP + Port)
     int socket(domain,  // AF_LOCAL (localhost), ...
                type,    // SOCK_STREAM (TCP), SOCK_DGRAM (UDP)
                protocol // 0 (IP)
     )

     // Bind socket to an address
     void bind(int       fd,     // File descriptor for socket
               sockaddr* addr,   // Socket's address
               socklen_t addrlen // Length of socket's address
     )

     // Set up to wait for incoming connections
     listen(int fd,     // File descriptor of socket
            int backlog // Maximum number of connections waiting
     )

     // Accept a new connection on a socket (and get a new file descriptor for
     // that particular connection).
     int accept(int fd,            // File descriptor for socket
                sockaddr*  addr,   // Socket's address
                socklen_t* addrlen // Length of socket's address
     )

 * The client would use many of the same above. But, instead of `listen` and
 * `accept`, we should use the following to connect to the server:

     // Connect socket to the given address.
     void connect(int fd,           // File descriptor for socket
                  sockaddr *addr,   // Address to dial
                  socklen_t addrlen // Length of address to dial
     )

 * When the connection has been established, then one can send and receive
 * messages as follows:

     // Send a message on a socket. Returns -1 if something went wrong locally.
     int send(int fd,          // File descriptor for socket
              const void* buf, // Buffer with message to write
              size_t length,   // Length of buffer
              int flags        // Flags
     )

     // Receive a message on a socket. Returns the length of the message.
     int recv(int fd,          // File descriptor for socket
              const void* buf, // Buffer with message to write
              size_t length,   // Length of buffer
              int flags        // Flags
     )
 */

#include <netinet/in.h>
/* Various helper functions for working with networking such as `htons()` that
   converts the port number into its network byte order (regardless of the
   machine's endianness). */

#include <arpa/inet.h>
/* More helper functions for working with networking, especially `inet_addr()`
   which converts an IP address string to its integer representation. */

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

#include <string.h>
/* Strings */

#include <errno.h>
#include <stdlib.h>
/* A few quality-of-life operations such as `exit()` and `strtol`. */

#include <threads.h>
/* C11 threads interface. Most importantly are the `thrd_create` and
 * `thrd_join` functions.
 */
