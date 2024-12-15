echo "[1/14] creating output directory..."
mkdir -p ./test_out

echo "[2/14] clearing output directory..."
rm -rf ./test_out/*

echo "[3/14] building..."
./build.sh

echo "[4/14] testing fondu_noir configuration..."
time ./bin/program ./test_ressources/configs/fondu_noir.txt
echo ""

echo "[5/14] testing fondu_noir configuration..."
time ./bin/program ./test_ressources/configs/fondu_niveaux_gris.txt
echo ""

echo "[6/14] testing fondu_bruitage_alteration configuration..."
time ./bin/program ./test_ressources/configs/fondu_bruitage_alteration.txt
echo ""

echo "[7/14] testing fondu_bruitage_remplacement configuration..."
time ./bin/program ./test_ressources/configs/fondu_bruitage_remplacement.txt
echo ""

echo "[8/14] testing fondu_flou configuration..."
time ./bin/program ./test_ressources/configs/fondu_flou.txt
echo ""

echo "[9/14] testing fondu_noir_retrecir configuration..."
time ./bin/program ./test_ressources/configs/animation_retrecir.txt
echo ""

echo "[10/14] testing animation_masquage_rideau configuration..."
time ./bin/program ./test_ressources/configs/animation_masquage_rideau.txt
echo ""

echo "[11/14] testing animation_masquage_persiennes configuration..."
time ./bin/program ./test_ressources/configs/animation_masquage_persiennes.txt
echo ""

echo "[12/14] testing transition_noir configuration..."
time ./bin/program ./test_ressources/configs/transition_noir.txt
echo ""

echo "[13/14] testing transition_fondu configuration..."
time ./bin/program ./test_ressources/configs/transition_fondu.txt
echo ""

echo "[14/14] testing transition_glissement configuration..."
time ./bin/program ./test_ressources/configs/transition_glissement.txt
echo ""

echo "all tests completed"