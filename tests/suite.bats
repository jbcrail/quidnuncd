#!/usr/bin/env bash

load test_helper

@test "Server responds to 'help'" {
  send_and_quit "help"
  assert_output <<EOF
HELP
INFO [fs|load|memory|network|page|stats|swap]
PING
QUIT
TIME

EOF
}

@test "Server responds to 'info'" {
  send_and_quit "info"
  assert_success
  assert_output_contains "server.os_name="
}

@test "Server responds to 'info fs'" {
  send_and_quit "info fs"
  assert_success
  assert_output_contains "fs.0.avail="
}

@test "Server responds to 'info load'" {
  send_and_quit "info load"
  assert_success
  assert_output_contains "load.min1="
}

@test "Server responds to 'info memory'" {
  send_and_quit "info memory"
  assert_success
  assert_output_contains "memory.total="
}

@test "Server responds to 'info network'" {
  send_and_quit "info network"
  assert_success
  assert_output_contains "net."
}

@test "Server responds to 'info page'" {
  send_and_quit "info page"
  assert_success
  assert_output_contains "page.in="
}

@test "Server responds to 'info stats'" {
  send_and_quit "info stats"
  assert_success
  assert_output_contains "stats.total_clients="
}

@test "Server responds to 'info swap'" {
  send_and_quit "info swap"
  assert_success
  assert_output_contains "swap.total="
}

@test "Server responds to 'ping'" {
  send_and_quit "ping"
  assert_success
  assert_output "pong"
}

@test "Server responds to 'time'" {
  send_and_quit "time"
  assert_success
  assert_output_contains "time.secs="
}

@test "Server responds to 'quit'" {
  send_and_quit
  assert_success
  assert_output ""
}
