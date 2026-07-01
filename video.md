# Brief — Génération d'une vidéo de l'évolution (projet qtiasnake)

## Objectif
Produire une **vidéo montrant la progression de l'apprentissage sur 100 générations**. Pour chaque génération, rejouer la partie du **meilleur individu** de cette génération sous forme d'une **séquence d'images**, avec un **overlay de stats** dessiné sur le plateau. Puis assembler le tout en vidéo.

L'intérêt narratif : partir d'un run **neuf (from 0)** → gén 1 le serpent meurt vite, gén 100 il remplit le plateau. La vidéo doit rendre visible cette montée en compétence.

## Contexte technique du projet
- **Qt5 / C++**, build via `qmake qtiasnake.pro && make`. App GUI (`MainWindow`).
- Fichiers clés :
  - `game.cpp/.h` : simulation. `Game::step()` avance d'un pas et retourne `false` en fin de partie. `Game::getResult()` → `GameResult{ Reseau::Poids poids; int scoreVisite; int scorePasteque; bool perdu; }`. `Game::getSnake()`, `getPasteque()`, `getSensors()`, `getCaseVisite()`, `getTotMouvement()`.
  - `snakewidget.cpp` : **`drawGame(QPainter*)`** — c'est LE rendu du plateau (grille, serpent, pastèque). C'est ce visuel qu'on veut dans les frames.
  - `mainwindow.cpp` : boucle d'entraînement. `iterate()` = une itération (un pas, ou bascule de partie), `onTimer()` = paquet d'itérations. Fin de génération dans le `else` de `iterate()` : après `std::sort(generation.begin(), generation.end(), sorterVisite)`, **`generation.first().poids`** = poids du meilleur individu, ses scores dans `.scoreVisite/.scorePasteque`. `idxGeneration` = numéro de génération.
- **Déterminisme important** : les positions de pastèques sont un cache statique dans `game.cpp`, vidé à chaque génération via `Game::resetPasteques()`, et `NB_EVAL=1`. Donc une partie est **entièrement déterministe** (poids + cache pastèques + départ fixe).

## Spécification
1. **Sélection** : sur les 100 premières générations, récupérer les poids du meilleur individu de chaque génération.
2. **Rejeu → frames** : rejouer ce meilleur pas à pas, et à chaque pas rendre une image du plateau.
3. **Overlay** (sur le plateau) avec les stats *courantes du rejeu en cours* : n° de génération, pastèques mangées, longueur du serpent, nb de mouvements, et le meilleur score de la génération. Bandeau semi-transparent, lisible.
4. **Sortie** : séquence de PNG numérotés + assemblage en vidéo (ffmpeg) — ou encodage direct.

## Approche recommandée
- **Rejeu fidèle** : capturer `generation.first().poids` en fin de génération **avant** l'appel à `Game::resetPasteques()`, et rejouer un `Game(22, 22, bestPoids)` sur le cache pastèques encore intact → reproduit exactement la partie qui a donné le score (déterministe). *Fallback plus simple si besoin* : rejouer sur un plateau neuf (montre le champion, mais score potentiellement différent de celui affiché en training).
- **Rendu offscreen** : refactorer `drawGame` pour qu'il peigne sur **n'importe quel `QPainter` d'une taille donnée** (extraire la logique dans une fonction réutilisable prenant `QPainter*` + dimensions). Rendre alors sur une `QImage` (ex. 800×800) sans dépendre de la fenêtre visible → `image.save("frames/genXXX_frameYYYY.png")`. Éviter `widget->grab()` (impose la fenêtre affichée).
- **Overlay** : `QPainter::drawText` par-dessus le plateau, dans la même fonction de rendu offscreen (paramètre optionnel « infos overlay »).
- **Assemblage** : fournir une commande ffmpeg type
  `ffmpeg -framerate 30 -pattern_type glob -i 'frames/*.png' -c:v libx264 -pix_fmt yuv420p video.mp4`.

## Points de vigilance (à traiter explicitement)
- **Volume de frames** : un bon serpent joue des milliers de pas ; 100 gén × milliers = potentiellement 100k+ images. Prévoir des paramètres : sous-échantillonnage (1 frame / N pas), cap de frames par génération, ou durée max par gén. Les 1res gén sont courtes (mort rapide), la charge est en fin de run.
- **Rythme narratif** : envisager une **carte-titre / pause** entre générations (« Génération 42 — 31 pastèques ») pour la lisibilité, et éventuellement ne garder qu'1 génération sur k (ex. gén 1, 5, 10, 20, …) si 100 clips complets font une vidéo trop longue.
- **Non-intrusif** : déclencher la capture via un **flag/argument ou un bouton dédié**, séparé de l'entraînement normal, pour ne pas ralentir/altérer les runs classiques.
- **Run from 0** : lancer un entraînement neuf pour la vidéo (montre la progression), pas une population chargée.

## Livrables attendus
1. Mécanisme de capture intégré proprement (derrière un flag/bouton), avec `drawGame` refactoré en rendu réutilisable offscreen.
2. Génération des PNG + script/commande ffmpeg d'assemblage (ou encodage direct).
3. Petite doc : comment lancer la capture, où sortent les frames/vidéo, params réglables (fps, sous-échantillonnage, nb de générations).

## Décisions à confirmer avec l'utilisateur avant/pendant
- Rejeu **fidèle** (avant resetPasteques) vs **plateau neuf** ?
- **Toutes** les 100 générations, ou un échantillon (ex. tous les 5) ?
- Format **MP4** vs **GIF**, résolution, fps.
- Contenu exact et style de l'overlay.
