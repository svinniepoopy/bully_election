#!/usr/bin/env bash

EXEC='main'

rm $EXEC

g++ -pthread -o $EXEC main.cpp election_sim.cpp net_helper.cpp process.cpp
