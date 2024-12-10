mkdir -p bin
g++ main.cpp -std=c++17 -Wall -fsanitize=undefined,address -Ipngio -lpng -g -o bin/program