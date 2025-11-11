# Information Flow Control

If applied correctly, then information flow control should be able to solve the
issues identified in the [../demo](`demo/`) folder.

## Flume, HiStar, Asbestos

Systems like [Flume](https://dl.acm.org/doi/abs/10.1145/1323293.1294293),
[HiStar](https://dl.acm.org/doi/abs/10.1145/2018396.2018419), and
[Asbestos](https://dl.acm.org/doi/abs/10.1145/1095809.1095813) provide
information-flow control at a too coarse-grained level. They consider each
program a black-box with certain privileges; whether the program can read from
and write to a socket depends on the labels of the socket's other endpoint and
of the program.

On the other hand, the leaks in [../demo](`demo/`) are due to sharing of memory
and/or control flow influencing what is sent. With these systems, either the
[../demo/echo/safe](safe echo server) example would be invalid (it is tainted by
all connections and cannot respond) or the unsafe ones would be permitted (they
can declassify/endorse information regardless of the bugs).
