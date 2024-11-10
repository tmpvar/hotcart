#/usr/bin/env bash

DIR="$(dirname "$(realpath "$0")")"

pushd "${DIR}/../build" > /dev/null
cmake --build . --parallel --target hotcart
popd > /dev/null