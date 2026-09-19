#!/usr/bin/env bash

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD="$ROOT/build"

if cmake -S "$ROOT" -B "$BUILD" && cmake --build "$BUILD"; then
    exec "$BUILD/span"
else
    echo
    echo "Build failed."
    exit 1
fi
