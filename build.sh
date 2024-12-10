mkdir -p bin
g++ main.cpp -std=c++17 -Wall -Ipngio -fopenmp -lpng -O3 -o bin/program