#!/bin/bash

tmux new-session -d -s cppserver

tmux send-keys -t cppserver "./build/server" C-m
tmux split-window -h -t cppserver
tmux send-keys -t cppserver "./build/client" C-m

tmux attach -t cppserver
