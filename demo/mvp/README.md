# Minimal Viable Problems (MVP)

<!-- markdown-toc start - Don't edit this section. Run M-x markdown-toc-refresh-toc -->
**Table of Contents**

- [Control Flow](#control-flow)
  - [`empty_<variant>`](#empty_variant)
  - [`public`](#public)
  - [`secret_<N>`](#secret_n)

<!-- markdown-toc end -->


## Control Flow

The following subfolders includes demonstrations of varying complexities for how
to leak information through `send`/`recv` and `select`. These are all based on
asymmetric control flow which favours one socket over another can leak secrets.

### `empty_<variant>`

These examples leak whether a secret channel has content or not. This provides a
1-bit side channel.

- `<stdin>`: Simple offline example using `stdin` and `stdout`.

- `<socket>`: The same but with two Unix sockets (with information sent with
  the `connect.c` program).

### `public`

This leaks of a secret value by dropping secret number of messages from a
socket with public messages.

### `secret_<N>`

These leak of a secret value by reading a (public) constant number of 256
messages from a set of streams.

- `1`: On a single socket, the secret number of messages is sent first
  followed by 256 public messages.

- `2`: The secret number is once more sent as a certain number of messages,
  but this time on a separate *secret* socket. Yet, since it is given
  priority when reading, it leaks anew.
