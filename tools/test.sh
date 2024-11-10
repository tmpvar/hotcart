#/usr/bin/env bash

DIR="$(dirname "$(realpath "$0")")"

pushd "${DIR}/../build" > /dev/null
cmake --build . --parallel --target hotcart-tests

LLVM_PROFILE_FILE=hotcart.profraw ./hotcart-tests
llvm-profdata merge -sparse hotcart.profraw -o hotcart.profdata
llvm-cov report hotcart-tests -instr-profile=hotcart.profdata --ignore-filename-regex=doctest

popd > /dev/null