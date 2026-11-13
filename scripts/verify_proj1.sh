#!/usr/bin/env bash
set -euo pipefail
export NO_COLOR=1
mkdir -p test-results
exec > >(tee test-results/verification.log) 2>&1
baseline=83024ffe49b8ecf1f76ed2ec2c5db7d6fea5b88c
git diff --exit-code "$baseline" -- src/game.h src/unit_tests.c src/asserts.c src/snake_utils.c src/interactive_snake.c tests tools Makefile .provenance-manifest
make clean
make all
./unit-tests | tee test-results/unit.log
# Original unit main returns zero even on failure; inspect all ten groups.
! grep -q 'Not all' test-results/unit.log
test "$(grep -c '^All .* tests passed!' test-results/unit.log)" -eq 10
./custom-tests | tee test-results/custom.log
grep -q 'All custom tests passed!' test-results/custom.log
make run-integration-tests | tee test-results/integration.log
test "$(grep -c '^Passed ' test-results/integration.log)" -eq 22
valgrind --error-exitcode=91 --leak-check=full --show-leak-kinds=all --errors-for-leak-kinds=all ./unit-tests >test-results/valgrind-unit.log 2>&1
valgrind --error-exitcode=91 --leak-check=full --show-leak-kinds=all --errors-for-leak-kinds=all ./custom-tests >test-results/valgrind-custom.log 2>&1
for input in tests/*-in.snk; do
  name=${input##*/}; name=${name%-in.snk}
  valgrind --error-exitcode=91 --leak-check=full --errors-for-leak-kinds=all ./snake --stdin -o "test-results/$name-stdin.snk" < "$input" >"test-results/valgrind-$name.log" 2>&1
  cmp "tests/$name-ref.snk" "test-results/$name-stdin.snk"
done
valgrind --error-exitcode=91 --leak-check=full --errors-for-leak-kinds=all ./unit-tests -m >test-results/valgrind-free.log 2>&1
bash scripts/check_proj1.sh | tee test-results/extra.log
# Protected original files and fixtures still match after all tests.
git diff --exit-code "$baseline" -- src/game.h src/unit_tests.c src/asserts.c src/snake_utils.c src/interactive_snake.c tests tools Makefile .provenance-manifest
