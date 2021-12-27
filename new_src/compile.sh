#!/bin/bash

## Compile our two program files
g++ -std=c++11 -Ofast -march=native Board.h Board.cpp minimax_alpha_beta.cpp -o code main.cpp
