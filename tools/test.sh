#/usr/bin/env bash

DIR="$(dirname "$(realpath "$0")")"

pushd "${DIR}/../build-tests" > /dev/null
cmake --build . --parallel --target hotcart-tests

./hotcart-tests

lcov --capture -q --directory CMakeFiles/hotcart-tests.dir --output-file coverage.info
lcov -q --no-external -r coverage.info "/nix/store/*" --output-file coverage.info
lcov -q --no-external -r coverage.info "/usr/include/*" --output-file coverage.info
lcov -q --no-external -r coverage.info "*doctest*" --output-file coverage.info

bash $DIR/print-coverage.sh coverage.info

echo "  html report: file://$(realpath coverage/index.html)"
popd > /dev/null

