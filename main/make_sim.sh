#!/usr/bin/env bash

EXEC='main'

rm -f $EXEC

g++ -pthread -o $EXEC main.cpp election_sim.cpp net_helper.cpp process.cpp
