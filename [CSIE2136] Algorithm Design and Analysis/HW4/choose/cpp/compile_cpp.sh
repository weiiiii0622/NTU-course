#!/bin/bash

problem=choose

g++ -O2 -std=c++17 grader.cpp $problem.cpp -o $problem
