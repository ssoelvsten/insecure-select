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
