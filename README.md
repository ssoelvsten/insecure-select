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

- [64809](https://bz.apache.org/bugzilla/show_bug.cgi?id=64809): connections
  are not reset after closing.
- [55615](https://bz.apache.org/bugzilla/show_bug.cgi?id=55615): thread
  starvation when number of clients exceeds a certain threshold.
