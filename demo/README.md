# Demonstrations

Each subfolder includes demonstrations of varying complexities for how to leak
information through `select`, `poll`, and `epoll`.

- `select.stdin.1`: Leak of 1-bit information about the content of `stdin`,
  i.e. whether it is empty or not.
