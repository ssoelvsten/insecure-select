# Secure Select

Incorrect multiplexing multiple streams (e.g. by using the
[`select`](https://man7.org/linux/man-pages/man2/select.2.html),
[`poll`](https://man7.org/linux/man-pages/man2/poll.2.html), and
[`epoll`](https://man7.org/linux/man-pages/man7/epoll.7.html) operations in
Linux) can create a side-channel with which one can leak information about the
contents of one stream to another; see the [`Demo`](#demo) for minimal
examples.

## Demo

These *C* programs provide examples of varying complexity on how to leak
information from a confidential stream.

## Related Issues

### Apache

The following [security
vulnerabilities](https://httpd.apache.org/security_report.html) in the Apache
web server are vaguely related to this issue:

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

## Usage (on GitHub)

But, how much is `select` and its siblings used (on GitHub)?

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
