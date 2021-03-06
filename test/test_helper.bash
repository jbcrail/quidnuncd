PORT=9999

setup() {
  nohup $QND --port $PORT &
}

send_and_quit() {
  local cmd="$1"
  if [ -z "$cmd" ]; then
    cmd="quit\r\n"
  else
    cmd="$cmd\r\nquit\r\n"
  fi
  run bash -c "echo -e '$cmd' | nc -C localhost $PORT | tr -d '\r'"
}

teardown() {
  eval $(kill $(ps -ef | grep qnd | grep "port $PORT" | awk '{print $2}'))
  true
}

flunk() {
  { if [ "$#" -eq 0 ]; then cat -
    else echo "$@"
    fi
  } | sed "s:${TMP}:\${TMP}:g" >&2
  return 1
}

assert_success() {
  if [ "$status" -ne 0 ]; then
  { echo "command failed with exit status $status"
    echo "output: $output"
  } | flunk
  elif [ "$#" -gt 0 ]; then
    assert_output "$1"
  fi
}

assert_failure() {
  if [ "$status" -eq 0 ]; then
    flunk "expected failed exit status"
  elif [ "$#" -gt 0 ]; then
    assert_output "$1"
  fi
}

assert_equal() {
  if [ "$1" != "$2" ]; then
  { echo "expected: $1"
    echo "actual:   $2"
  } | flunk
  fi
}

assert_output() {
  local expected
  if [ $# -eq 0 ]; then expected="$(cat -)"
  else expected="$1"
  fi
  assert_equal "$expected" "$output"
}

assert_output_contains() {
  local expected="$1"
  if [ -z "$expected" ]; then
    echo "assert_output_contains needs an argument" >&2
    return 1
  fi
  [[ "$output" = *"$expected"* ]] || {
    { echo "expected output to contain $expected"
      echo "actual: $output"
    } | flunk
  }
}
