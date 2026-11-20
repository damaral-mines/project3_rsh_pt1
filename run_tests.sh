#!/usr/bin/env bash
set -u
ROOT="$(cd "$(dirname "$0")" && pwd)"
cd "$ROOT" || exit 1

make || { echo "Build failed"; exit 1; }

pass=0; fail=0
for input in test_cases/test*.in; do
    name="$(basename "$input" .in)"
    expected="test_cases/$name.out"
    tmp="$(mktemp -d)"
    actual="$(mktemp)"
    cp test_cases/* "$tmp/"
    cp "$ROOT/rsh" "$tmp/"
    if ( cd "$tmp" && "$ROOT/rsh" < "$name.in" > "$actual" 2>/dev/null ) \
        && diff -u "$expected" "$actual"; then
        echo "PASS $name"; pass=$((pass+1))
    else
        echo "FAIL $name"; fail=$((fail+1))
    fi
    rm -rf "$tmp" "$actual"
done

echo "$pass passed, $fail failed"
[ "$fail" -eq 0 ]
