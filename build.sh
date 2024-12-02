mkdir -p bin
g++ main.cpp -Ipngio -lpng -o bin/program

# -fsanitize=undefined,address