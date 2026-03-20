# Configurer + compiler
cmake -B build
cmake --build build
 
# Lancer les tests
ctest --test-dir build --output-on-failure
 
# Générer la documentation HTML
cmake --build build --target doc
# → ouvrir docs/html/index.html