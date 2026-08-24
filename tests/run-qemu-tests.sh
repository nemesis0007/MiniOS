#!/usr/bin/env bash
set -euo pipefail

qemu_binary="$1"
iso_path="$2"
serial_log="$3"

rm -f "$serial_log"
{
  sleep 9
  for key in s e l f t e s t; do
    echo "sendkey $key"
  done
  echo "sendkey ret"
  sleep 8
  echo "quit"
} | timeout 30s "$qemu_binary" \
  -cdrom "$iso_path" \
  -display none \
  -monitor stdio \
  -serial "file:$serial_log" \
  -no-reboot -no-shutdown >/dev/null

grep -q '^@TEST|SUMMARY|13|0' "$serial_log"
echo "MiniOS QEMU integration suite: 13 passed, 0 failed"
