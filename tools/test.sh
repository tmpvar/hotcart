#/usr/bin/env bash

DIR="$(dirname "$(realpath "$0")")"

pushd "${DIR}/../build-tests" > /dev/null
cmake --build . --parallel --target hotcart-tests

mkdir -p coverage
pushd coverage > /dev/null
  llvm-cov gcov ../CMakeFiles/hotcart-tests.dir/src/tests.cpp.gcda > /dev/null
popd

# LLVM_PROFILE_FILE=hotcart.profraw ./hotcart-tests
# llvm-profdata merge -sparse hotcart.profraw -o hotcart.profdata
# llvm-cov report hotcart-tests -instr-profile=hotcart.profdata --ignore-filename-regex=doctest

popd > /dev/null

