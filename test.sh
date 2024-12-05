#!/bin/bash
mkdir -p ./test_out
echo "[1/7] created output directory."

rm -rf ./test_out/*
echo "[2/7] cleared output directory."

./build.sh
echo "[3/7] build completed."

./bin/program ./test_ressources/configs/fondu_noir.txt
echo "[4/7] test fondu_noir configuration."

./bin/program ./test_ressources/configs/fondu_niveaux_gris.txt
echo "[5/7] test fondu_noir configuration."

./bin/program ./test_ressources/configs/fondu_bruitage.txt
echo "[6/7] test fondu_bruitage configuration."

./bin/program ./test_ressources/configs/fondu_flou.txt
echo "[7/7] test fondu_flou configuration."

echo "all test completed"
