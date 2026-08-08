#!/usr/bin/env bash

set -euxo pipefail

IMAGE="big-endian-tests:s390x"

docker run --privileged --rm tonistiigi/binfmt --install s390x

if ! docker image inspect "$IMAGE" >/dev/null 2>&1; then
   docker build \
      --platform linux/s390x \
      -f .github/workflows/scripts/Dockerfile.big-endian \
      -t "$IMAGE" .
fi

docker run --rm \
   --platform linux/s390x \
   -v "$PWD:/src" \
   -w /src \
   "$IMAGE" \
   bash -euxo pipefail -c '
      # rm -rf build_test_s390x
      cmake -S . -B build_test_s390x -D IS_BIG_ENDIAN=ON -D DO_NOT_TEST_GMP_LEAKS=ON
      cmake --build build_test_s390x --target Tests
      ctest --test-dir build_test_s390x -V
   '

