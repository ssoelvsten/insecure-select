# Echo Server

You can `make build/<example>` and `make run/<example>` to compile and run the
program with GCC.

The `client` provides a simple implementation of a client. This client throttles
its messages. This makes it easier for us slow humans to play around with these
examples.

## Safe Reference

For reference, we provide two safe implementations of an echo server.

- `safe`: A simple echo server without any bells and whistles.

- `buffer.safe`: A safe implementation of an echo server which buffers the
  users' messages until they send a `\n` or `\0`.

## Control Flow Leaks

The following two examples provide a timing channel by full or partial
starvation of one connection due to another.

- `first-served`: An echo server, that in each round (2s) only replies to
  the first connection with content. Effectively, a *starvation* of a later
  connection provides a side-channel. This lifts the `empty.socket` demo into an
  echo server.

- `throttle`: An echo server, that in each (2s) round only responds with 16
    bytes greedily favouring the earliest connected clients. This lifts the
    `secret.2` example into an echo server.

The `drop_<N>` servers lift the `empty.socket` [MVP](../mvp) example without
making it a timing side channel. The two servers differ in the number of
`select` statements each round.

- `drop_<1>`: Only the first is served each round (still potentially starving
  other connections). Yet, each connection not only gets its own message back
  but also the value of an incrementing counter, i.e. an abstract clock.

- `drop_<2>`: Every second use of `select` only drops messages from the first
  connection rather than sending it back; each connection can see in their
  response whether they (or a prior connection) had their data dropped.

## Buffer Leaks

Similar to `buffer.safe`. But, these server implementations mismanage the buffer
to leak information about one client to another.

- `buffer.bad_close`: The buffer is not properly reset when disconnecting
  with half a message left in the buffer. This is inspired by Apache's
  security issue
    [CVE-2010-0434](https://www.cve.org/CVERecord?id=CVE-2010-0434).

- `buffer.share`: The same buffer is used for all connections, meaning
  one can read buffered partial messages from others. This is inspired by
  Apache's bug [52701](https://bz.apache.org/bugzilla/show_bug.cgi?id=42701).
