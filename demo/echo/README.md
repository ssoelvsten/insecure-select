# Echo Server

You can `make build/<example>` and `make run/<example>` to compile and run the
program with GCC.

The `client` provides a simple implementation of a client. This client throttles
its messages. This makes it easier for us slow humans to play around with these
examples.

<!-- markdown-toc start - Don't edit this section. Run M-x markdown-toc-refresh-toc -->
**Table of Contents**

- [Safe Reference](#safe-reference)
  - [`safe`](#safe)
  - [`buffer.safe`](#buffersafe)
- [Control Flow Leaks](#control-flow-leaks)
  - [`cf.first-served`](#cffirst-served)
  - [`cf.throttle`](#cfthrottle)
  - [`cf.drop_<N>`](#cfdrop_n)
- [Buffer Leaks](#buffer-leaks)
  - [`buffer.close`](#cfclose)
  - [`buffer.share`](#cfshare)

<!-- markdown-toc end -->


## Safe Reference

For reference, we provide two safe implementations of an echo server.

### `safe`

This is a simple echo server without any bells and whistles.

### `buffer.safe`

This is a safe implementation of an echo server which buffers the users'
messages until they send a `\n` or `\0`.

## Control Flow Leaks

The following two examples provide a timing channel by full or partial
starvation of one connection due to another.

### `cf.first-served`

This is an echo server, that in each round (2s) only replies to the first
connection with content. Effectively, a *starvation* of a later connection
provides a side-channel. This lifts the `empty.socket` [mvp](../mvp) demo into
an echo server.

### `cf.throttle`

This is an echo server, that in each (2s) round only responds with 16 bytes
greedily favouring the earliest connected clients. This lifts the `secret.2`
[mvp](../mvp) demo into an echo server.

### `cf.drop_<N>`

These lift the `empty.socket` [mvp](../mvp) example without making it a timing
side channel. The two servers differ in the number of `select` statements each
round.

- `1`: Only the first is served each round (still potentially starving
  other connections). Yet, each connection not only gets its own message back
  but also the value of an incrementing counter, i.e. an abstract clock.

- `2`: Every second use of `select` only drops messages from the first
  connection rather than sending it back; each connection can see in their
  response whether they (or a prior connection) had their data dropped.

## Buffer Leaks

Similar to `buffer.safe`. But, these server implementations mismanage the buffer
to leak information about one client to another.

### `buffer.close`

In this server, the buffer is not properly reset when disconnecting with half a
message left in the buffer. This is inspired by Apache's security issue
[CVE-2010-0434](https://www.cve.org/CVERecord?id=CVE-2010-0434).

### `buffer.share`

Here, the same buffer is used for all connections, meaning one can read buffered
partial messages from others. This is inspired by Apache's bug
[52701](https://bz.apache.org/bugzilla/show_bug.cgi?id=42701).
