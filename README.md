# Insecure Select

Incorrect multiplexing of multiple streams (e.g. by using the
[`select`](https://man7.org/linux/man-pages/man2/select.2.html),
[`poll`](https://man7.org/linux/man-pages/man2/poll.2.html), and
[`epoll`](https://man7.org/linux/man-pages/man7/epoll.7.html) operations in
Linux) can create side-channels. This can leak information about the contents of
one stream to another.

## Demo

These *C* programs provide examples of varying complexity on how to leak
information from a confidential stream.

These demos can be divided into two types

1. Asymmetric control flow which favours one connection over another.
2. Mismanagement of buffers allows one connection to read information left from
   another.

See the description of each particular example for more details.


## Relevance

But, how much is `select` and its siblings used? Here is a quick preliminary
search on GitHub.

- [Sourcegraph search for
  `select`](https://sourcegraph.com/search?q=context:global+language:C+type:file+%3D%5Cs*select%5C%28.*%2C.*%2C.*%2C.*%2C.*%5C%29&patternType=regexp&sm=0):
  More than a thousand usages; some of which are not related to the Unix
  `select`.
- [Sourcegraph search for
  `poll`](https://sourcegraph.com/search?q=context:global+language:C+type:file+%3D%5Cs*poll%5C%28.*%2C.*%2C.*%5C%29&patternType=regexp&sm=0):
  Also more than a thousand hits! Some of these, of course, are not related to
  the Unix `poll`.
- [Sourcegraph search for `epoll_wait`](https://sourcegraph.com/search?q=context:global+language:C+type:file+%3D%5Cs*epoll_wait%5C%28.*%2C.*%2C.*%2C.*%5C%29&patternType=regexp&sm=0):
  Again, more than a thousand hits!


## Related Material

### Real-world Issues

#### Apache

The following [security
vulnerabilities](https://httpd.apache.org/security_report.html) in the Apache
web server are related to the `select` multiplexing:

- [CVE-2009-2699](https://www.cve.org/CVERecord?id=CVE-2009-2699): `poll`
  denial of service
- [CVE-2010-0434](https://www.cve.org/CVERecord?id=CVE-2010-0434): reading
  memory left behind after serving an earlier request.
- [CVE-2010-2791](https://www.cve.org/CVERecord?id=CVE-2010-2791): obtaining a
  response meant for another client.

The [bug database](https://bz.apache.org/bugzilla/) also has the following
interesting issues:

- [40660](https://bz.apache.org/bugzilla/show_bug.cgi?id=40660): Small typo
  error changes precedence of operators which affected the wrapper for the
  *epoll* system call.
- [52701](https://bz.apache.org/bugzilla/show_bug.cgi?id=42701): Incorrect
  completion of partial requests due to mixing up the sockets, leads to garbled
  requests that may result in crashes or incorrect (secret?) data returned.
- [59897](https://bz.apache.org/bugzilla/show_bug.cgi?id=59897): Using `select`
  with more than `FD_SETSIZE` (usually 1024) sockets leads to a crash.
- [64809](https://bz.apache.org/bugzilla/show_bug.cgi?id=64809): connections
  are not reset after closing.
- [55615](https://bz.apache.org/bugzilla/show_bug.cgi?id=55615): thread
  starvation when number of clients exceeds a certain threshold.

### Papers

- Systems like [Flume](https://dl.acm.org/doi/abs/10.1145/1323293.1294293),
  [HiStar](https://dl.acm.org/doi/abs/10.1145/2018396.2018419), and
  [Asbestos](https://dl.acm.org/doi/abs/10.1145/1095809.1095813) provide
  information-flow control at a too coarse-grained level. They consider each
  program a black-box with certain privileges; whether the program can read
  from and write to a socket depends on the labels of the socket's other
  endpoint and of the program.

  On the other hand, the leaks in `demo/` are due to sharing of memory and/or
  control flow influencing what is sent. With these systems, either the safe
  `echo` example would be invalid (it is tainted by all connections and cannot
  respond) or the unsafe ones would be permitted (they can declassify/endorse
  information regardless of the bugs).
