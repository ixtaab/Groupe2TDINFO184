mkdir -p bin
g++ src/*.cpp src/animations/*.cpp pngio/png_io.cpp -I./headers -std=c++17 -Wall -fsanitize=undefined,address -Ipngio -lpng -g -o bin/program