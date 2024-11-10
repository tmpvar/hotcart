#!/usr/bin/env bash
DIR="$(dirname "$(realpath "$0")")"

pushd "${DIR}/.." > /dev/null
watchexec -w src -w include -w CMakeLists.txt -c -r "$DIR/test.sh"
popd > /dev/null
