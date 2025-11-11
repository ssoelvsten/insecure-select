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

## Notes

A set of notes on the subject, e.g. real-world instances and possible solutions.
