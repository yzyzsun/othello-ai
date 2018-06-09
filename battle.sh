#!/bin/bash -x

if [[ $# -lt 1 ]]; then
  echo "Missing session number"
  exit 1
fi

readonly BASE_URL="http://47.89.179.202:5000"
readonly SLEEP_TIME=1
readonly TIMEOUT=60
readonly MIN_DEPTH=6
readonly MAX_DEPTH=10

# For macOS: install `coreutils` via Homebrew to use `timeout`.
export PATH="${PATH}:/usr/local/opt/coreutils/libexec/gnubin"

self=$(curl -s "${BASE_URL}/create_session/$1")
while :; do
  turn=$(curl -s "${BASE_URL}/turn/$1")
  if [[ ${turn} != ${self} ]]; then
    sleep ${SLEEP_TIME}
    continue
  fi
  board=$(curl -s "${BASE_URL}/board_string/$1")
  ./api <<< "0 ${board}"
  move=$(./api ${MIN_DEPTH} <<< "${turn} ${board}")
  move_prime=$(timeout ${TIMEOUT} ./api ${MAX_DEPTH} <<< "${turn} ${board}")
  if [[ -n ${move_prime} ]]; then
    move=${move_prime}
  fi
  if [[ -z ${move} ]]; then
    read -p "Cannot play. Press enter to pass..."
    curl -s "${BASE_URL}/change_color/$1"
  fi
  result=$(curl -s "${BASE_URL}/move/$1/${move/ //}/${self}")
  if [[ ${result} != "SUCCESS" ]]; then
    read
  fi
  board=$(curl -s "${BASE_URL}/board_string/$1")
  ./api <<< "0 ${board}"
done
