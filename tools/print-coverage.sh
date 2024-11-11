#!/bin/bash

DIR="$(dirname "$(realpath "$0")")"

if [ -z "$1" ]; then
  echo "Usage: $0 <lcov_file.info>"
  exit 1
fi

echo ""
echo "Coverage report:"

lcov_file="$1"
current_file=""
lines_found="0"
lines_hit="0"
cat $lcov_file | while read -r line; do
  file_match=$(echo $line | grep -E "^SF:.*" | awk -F':' '{print $2}')
  if [[ -n "$file_match" ]]; then
    current_file=$file_match
  fi

  lines_found_match=$(echo $line | grep -E "^LF:.*" | awk -F':' '{print $2}')

  if [[ -n "$lines_found_match" ]]; then
    lines_found=$lines_found_match
  fi

  lines_hit_match=$(echo $line | grep -E "^LH:.*" | awk -F':' '{print $2}')
  if [[ -n "$lines_hit_match" ]]; then
    lines_hit=$lines_hit_match
  fi

  end_record=$(echo $line | grep -E "^end_of_record")
  if [[ -n "$end_record" ]]; then
    percent=$(python -c "print('{:.2f}'.format(float($lines_hit) / float($lines_found) * 100.0))")

    echo "  $(realpath --relative-to="$DIR/.." $current_file) $percent%"
  fi
done
echo ""
