#!/usr/bin/env bash
set -e
DIR="$(dirname "$(realpath "$0")")"

pushd "${DIR}/.." > /dev/null

cmake -S . -B build-tests -G Ninja \
 -DCMAKE_BUILD_TYPE=Debug \
 -DHOTCART_ENABLE_TESTS=1 \
 -DHOTCART_TEST_COVERAGE_GCOV=1
watchexec -w src -w include -w CMakeLists.txt -w $DIR -c -r "$DIR/test.sh"

popd > /dev/null
