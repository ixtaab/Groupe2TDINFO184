#!/bin/bash
mkdir -p ./test_out
echo "[1/6] created output directory."

rm -rf ./test_out/*
echo "[2/6] cleared output directory."

./build.sh
echo "[3/6] build completed."

./bin/program ./test_ressources/configs/fondu_noir.txt
echo "[4/6] test fondu_noir configuration."

./bin/program ./test_ressources/configs/fondu_bruitage.txt
echo "[5/6] test fondu_bruitage configuration."

./bin/program ./test_ressources/configs/fondu_flou.txt
echo "[6/6] test fondu_flou configuration."

echo "all test completed"
