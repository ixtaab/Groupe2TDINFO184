mkdir -p bin
g++ main.cpp -Ipngio -fopenmp -lpng -O3 -o bin/program

# -fsanitize=undefined,address