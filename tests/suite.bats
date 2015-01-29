#!/usr/bin/env bash

load test_helper

@test "Server responds to 'help'" {
  run bash -c "printf 'help\r\nquit\r' | nc -C localhost $PORT | tr -d '\r'"
  assert_output <<EOF
HELP
INFO [fs|load|memory|network|page|stats|swap]
PING
QUIT
TIME

EOF
}

@test "Server responds to 'info'" {
  run bash -c "printf 'info\r\nquit\r' | nc -C localhost $PORT | tr -d '\r'"
  assert_success
  assert_output_contains "server.os_name="
}

@test "Server responds to 'info fs'" {
  run bash -c "printf 'info fs\r\nquit\r' | nc -C localhost $PORT | tr -d '\r'"
  assert_success
  assert_output_contains "fs.0.avail="
}

@test "Server responds to 'info load'" {
  run bash -c "printf 'info load\r\nquit\r' | nc -C localhost $PORT | tr -d '\r'"
  assert_success
  assert_output_contains "load.min1="
}

@test "Server responds to 'info memory'" {
  run bash -c "printf 'info memory\r\nquit\r' | nc -C localhost $PORT | tr -d '\r'"
  assert_success
  assert_output_contains "memory.total="
}

@test "Server responds to 'info network'" {
  run bash -c "printf 'info network\r\nquit\r' | nc -C localhost $PORT | tr -d '\r'"
  assert_success
  assert_output_contains "net."
}

@test "Server responds to 'info page'" {
  run bash -c "printf 'info page\r\nquit\r' | nc -C localhost $PORT | tr -d '\r'"
  assert_success
  assert_output_contains "page.in="
}

@test "Server responds to 'info stats'" {
  run bash -c "printf 'info stats\r\nquit\r' | nc -C localhost $PORT | tr -d '\r'"
  assert_success
  assert_output_contains "stats.total_clients="
}

@test "Server responds to 'info swap'" {
  run bash -c "printf 'info swap\r\nquit\r' | nc -C localhost $PORT | tr -d '\r'"
  assert_success
  assert_output_contains "swap.total="
}

@test "Server responds to 'ping'" {
  run bash -c "printf 'ping\r\nquit\r' | nc -C localhost $PORT | tr -d '\r'"
  assert_success
  assert_output "pong"
}

@test "Server responds to 'time'" {
  run bash -c "printf 'time\r\nquit\r' | nc -C localhost $PORT | tr -d '\r'"
  assert_success
  assert_output_contains "time.secs="
}

@test "Server responds to 'quit'" {
  run bash -c "printf 'quit\r' | nc -C localhost $PORT | tr -d '\r'"
  assert_success
  assert_output ""
}
