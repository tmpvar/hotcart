# Hotcart

[![codecov](https://codecov.io/github/tmpvar/hotcart/graph/badge.svg?token=D140ZH0YD3)](https://codecov.io/github/tmpvar/hotcart)

hot-reloading rapid prototyping platform for c++


## Development

```shell
nix-shell
cmake -B build -S . --G Ninja

# watch build and run hotcart
tools/watch-build.sh build path/to/file.cpp

# or watch tests
tools/watch-tests.sh
```
