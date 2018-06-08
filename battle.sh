#!/bin/bash -x

if [[ $# -lt 1 ]]; then
  echo "Missing session number"
  exit 1
fi

readonly BASE_URL="http://47.89.179.202:5000"
readonly TIMEOUT=60
readonly MIN_DEPTH=6
readonly MAX_DEPTH=10

# For macOS: install `coreutils` via Homebrew to use `timeout`.
export PATH="${PATH}:/usr/local/opt/coreutils/libexec/gnubin"

self=$(curl "${BASE_URL}/create_session/$1" 2> /dev/null)
while :; do
  turn=$(curl "${BASE_URL}/turn/$1" 2> /dev/null)
  if [[ ${turn} != ${self} ]]; then
    sleep 1
    continue
  fi
  board=$(curl "${BASE_URL}/board_string/$1" 2> /dev/null)
  move=$(./api ${MIN_DEPTH} <<< "${turn} ${board}")
  move_prime=$(timeout ${TIMEOUT} ./api ${MAX_DEPTH} <<< "${turn} ${board}")
  if [[ -n ${move_prime} ]]; then
    move=${move_prime}
  fi
  if [[ -z ${move} ]]; then
    read -p "Cannot play..."
  fi
  curl "${BASE_URL}/move/$1/${move/ //}/${self}" 2> /dev/null
done
