# Secure Select

The [`select`](https://man7.org/linux/man-pages/man2/select.2.html),
[`poll`](https://man7.org/linux/man-pages/man2/poll.2.html), and
[`epoll`](https://man7.org/linux/man-pages/man7/epoll.7.html) operations in Linux can leak
information about the contents of a secret file; see the [`examples`](#examples) for more details.

## Examples

In these examples whether a confidential file, `secret.txt`, is empty or not is leaked due to
blocking behaviour and/or flow of information.

- [x] `select`
- [ ] `poll`
- [ ] `epoll`
- [ ] `inotify`
