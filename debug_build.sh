mkdir -p bin
g++ main.cpp -std=c++17 -fsanitize=undefined,address -fno-omit-frame-pointer -Ipngio -lpng -g -o bin/program