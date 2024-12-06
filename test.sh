#!/bin/bash
echo "[1/7] creating output directory..."
mkdir -p ./test_out

echo "[2/7] clearing output directory..."
rm -rf ./test_out/*

echo "[3/7] building..."
./build.sh

echo "[4/7] testing fondu_noir configuration..."
./bin/program ./test_ressources/configs/fondu_noir.txt

echo "[5/7] testing fondu_noir configuration..."
./bin/program ./test_ressources/configs/fondu_niveaux_gris.txt

echo "[6/7] testing fondu_bruitage configuration..."
./bin/program ./test_ressources/configs/fondu_bruitage.txt

echo "[7/7] testing fondu_flou configuration..."
./bin/program ./test_ressources/configs/fondu_flou.txt

echo "[7/7] testing fondu_flou configuration..."
./bin/program ./test_ressources/configs/fondu_flou.txt

echo "all tests completed"