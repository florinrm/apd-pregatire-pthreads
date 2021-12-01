#!/bin/bash

make

./testapd 10 3
./testapd 25 4
./testapd 3 8

make clear