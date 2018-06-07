#!/bin/bash
set -euxo pipefail

readonly BASE_URL="http://47.89.179.202:5000"
readonly DEPTH=10

self=$(curl "${BASE_URL}/create_session/$1" 2> /dev/null)
while :; do
  turn=$(curl "${BASE_URL}/turn/$1" 2> /dev/null)
  if [[ ${turn} != ${self} ]]; then
    sleep 1
    continue
  fi
  board=$(curl "${BASE_URL}/board_string/$1" 2> /dev/null)
  move=$(./api ${DEPTH} <<< "${turn} ${board}")
  curl "${BASE_URL}/move/$1/${move/ //}/${self}" 2> /dev/null
done
