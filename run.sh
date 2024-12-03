#!/bin/bash
# sudo apt-get install -y freeglut3-dev libglfw3-dev libglfw3 > /dev/null 2>&1

# install dependable libraries
# -y say yes to all prompt messages
# /dev/null doesn't show installation process
sudo apt-get install -y libssl-dev > /dev/null 2>&1

cc -Wall main.c logic.c peer.c sha256.c -o run -lssl -lcrypto

# execute compiled file program
./run

# Clean up the compiled file
rm run