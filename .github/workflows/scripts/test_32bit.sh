#!/usr/bin/env bash

set -euxo pipefail

IMAGE="32-bit-tests:i386"

docker run --privileged --rm tonistiigi/binfmt --install 386

if ! docker image inspect "$IMAGE" >/dev/null 2>&1; then
   docker build \
      --platform linux/386 \
      -f .github/workflows/scripts/Dockerfile.32-bit \
      -t "$IMAGE" .
fi

docker run --rm \
   --platform linux/386 \
   -v "$PWD:/src" \
   -w /src \
   "$IMAGE" \
   bash -euxo pipefail -c '
      # rm -rf build_test_i386
      cmake -S . -B build_test_i386 -D CONFIRM_32BIT=ON -D DO_NOT_TEST_GMP_LEAKS=ON
      cmake --build build_test_i386 --target Tests
      ctest --test-dir build_test_i386 -V
   '

