echo "[1/17] creating output directory..."
mkdir -p ./test_out

echo "[2/17] clearing output directory..."
rm -rf ./test_out/*

echo "[3/17] building..."
./build.sh

echo "[4/17] testing fondu_noir configuration..."
time ./bin/program ./test_ressources/configs/fondu_noir.txt
echo ""

echo "[5/17] testing fondu_noir configuration..."
time ./bin/program ./test_ressources/configs/fondu_niveaux_gris.txt
echo ""

echo "[6/17] testing fondu_bruitage_alteration configuration..."
time ./bin/program ./test_ressources/configs/fondu_bruitage_alteration.txt
echo ""

echo "[7/17] testing fondu_bruitage_remplacement configuration..."
time ./bin/program ./test_ressources/configs/fondu_bruitage_remplacement.txt
echo ""

echo "[8/17] testing fondu_flou configuration..."
time ./bin/program ./test_ressources/configs/fondu_flou.txt
echo ""

echo "[9/17] testing fondu_noir_retrecir configuration..."
time ./bin/program ./test_ressources/configs/animation_retrecir.txt
echo ""

echo "[10/17] testing animation_masquage_rideau configuration..."
time ./bin/program ./test_ressources/configs/animation_masquage_rideau.txt
echo ""

echo "[11/17] testing animation_masquage_persiennes configuration..."
time ./bin/program ./test_ressources/configs/animation_masquage_persiennes.txt
echo ""

echo "[12/17] testing transition_noir configuration..."
time ./bin/program ./test_ressources/configs/transition_noir.txt
echo ""

echo "[13/17] testing transition_fondu configuration..."
time ./bin/program ./test_ressources/configs/transition_fondu.txt
echo ""

echo "[14/17] testing transition_glissement configuration..."
time ./bin/program ./test_ressources/configs/transition_glissement.txt
echo ""

echo "[15/17] testing transition_remplacement_aleatoire configuration..."
time ./bin/program ./test_ressources/configs/transition_remplacement_aleatoire.txt
echo ""

echo "[16/17] testing transition_remplacement_persiennes configuration..."
time ./bin/program ./test_ressources/configs/transition_remplacement_persiennes.txt
echo ""

echo "[17/17] testing transition_remplacement_rideau configuration..."
time ./bin/program ./test_ressources/configs/transition_remplacement_rideau.txt
echo ""

echo "all tests completed"