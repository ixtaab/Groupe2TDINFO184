#!/bin/bash
echo "[1/8] creating output directory..."
mkdir -p ./test_out

echo "[2/8] clearing output directory..."
rm -rf ./test_out/*

echo "[3/8] building..."
./build.sh

echo "[4/8] testing fondu_noir configuration..."
time ./bin/program ./test_ressources/configs/fondu_noir.txt
echo ""

echo "[5/8] testing fondu_noir configuration..."
time ./bin/program ./test_ressources/configs/fondu_niveaux_gris.txt
echo ""

echo "[6/8] testing fondu_bruitage_alteration configuration..."
time ./bin/program ./test_ressources/configs/fondu_bruitage_alteration.txt
echo ""

echo "[7/8] testing fondu_bruitage_remplacement configuration..."
time ./bin/program ./test_ressources/configs/fondu_bruitage_remplacement.txt
echo ""

echo "[8/8] testing fondu_flou configuration..."
time ./bin/program ./test_ressources/configs/fondu_flou.txt
echo ""

echo "[9/8] testing fondu_noir_retrecir configuration..."
time ./bin/program ./test_ressources/configs/animation_retrecir.txt
echo ""

echo "[10/8] testing animation_masquage_rideau configuration..."
time ./bin/program ./test_ressources/configs/animation_masquage_rideau.txt
echo ""

echo "[11/8] testing animation_masquage_persiennes configuration..."
time ./bin/program ./test_ressources/configs/animation_masquage_persiennes.txt
echo ""

echo "all tests completed"