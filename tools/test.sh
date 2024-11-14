#/usr/bin/env bash
set -e
DIR="$(dirname "$(realpath "$0")")"

pushd "${DIR}/../build" > /dev/null
cmake --build . --parallel

./hotcart-tests

lcov --capture -q --directory CMakeFiles/hotcart-tests.dir --output-file coverage.info
lcov -q --no-external -r coverage.info "/nix/store/*" --output-file coverage.info
lcov -q --no-external -r coverage.info "/usr/include/*" --output-file coverage.info
lcov -q --no-external -r coverage.info "*doctest*" --output-file coverage.info
lcov -q --no-external -r coverage.info "*_deps*" --output-file coverage.info

bash $DIR/print-coverage.sh coverage.info

genhtml -q coverage.info --output-directory coverage
echo "  html report: file://$(realpath coverage/index.html)"
popd > /dev/null

