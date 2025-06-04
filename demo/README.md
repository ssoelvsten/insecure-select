# Demonstrations

The following subfolders includes demonstrations of varying complexities for
how to leak information through `send`/`recv` and `select`.

- `empty.<variant>`: Leak whether a secret channel has content or not. This
  provides a 1-bit side channel.
  - `<stdin>`: Simple offline example using `stdin` and `stdout`.
  - `<socket>`: The same but with two Unix sockets (with information sent with
    the `connect.c` program).

- `public`: Leak of a secret value by dropping secret number of messages from a
  socket with public messages.

- `secret.<sockets>`: Leak of a secret value by reading a (public) constant
  number of 256 messages from a set of streams.
  - `<1>`: On a single socket, the secret number of messages is sent first
    followed by 256 public messages.
  - `<2>`: The secret number is once more sent as a certain number of messages,
    but this time on a separate *secret* socket. Yet, since it is given
    priority when reading, it leaks anew.

Based on these minimal examples, we have implemented an `echo` server which
leaks information about one user's inputs to another.

- `echo.safe`: Is a secure reference implementation that does not leak any
  information across channels due to a misuse of `select`.

- `echo.throttle`: An echo server, that in each (1s) round only responds with
  16 bytes greedily favouring the earliest connected clients. This lifts the
  `secret.2` example into an echo server.
