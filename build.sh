mkdir -p bin
g++ main.cpp -std=c++17 -Ipngio -fopenmp -lpng -O3 -o bin/program

# -fsanitize=undefined,address