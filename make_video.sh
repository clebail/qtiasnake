#!/usr/bin/env bash
# Assemble les frames PNG capturées (voir README section « Vidéo ») en MP4.
#
# Usage : ./make_video.sh [dossier_frames] [sortie.mp4] [fps]
#   dossier_frames : défaut "frames"
#   sortie         : défaut "video.mp4"
#   fps            : défaut 30
#
# Les frames sont nommées genXXX_frameYYYYY.png : le tri lexicographique du glob
# donne l'ordre chronologique (gén 1 → gén 100, pas 0 → fin de partie).

set -euo pipefail

FRAMES_DIR="${1:-frames}"
OUTPUT="${2:-video.mp4}"
FPS="${3:-30}"

if ! command -v ffmpeg >/dev/null 2>&1; then
    echo "Erreur : ffmpeg introuvable. Installe-le (ex. apt install ffmpeg)." >&2
    exit 1
fi

if [ -z "$(ls -A "$FRAMES_DIR"/*.png 2>/dev/null)" ]; then
    echo "Erreur : aucune frame *.png dans '$FRAMES_DIR'." >&2
    exit 1
fi

echo "Assemblage de $(ls "$FRAMES_DIR"/*.png | wc -l) frames à ${FPS} fps -> $OUTPUT"

ffmpeg -y -framerate "$FPS" -pattern_type glob -i "$FRAMES_DIR/*.png" \
       -c:v libx264 -pix_fmt yuv420p "$OUTPUT"

echo "Vidéo générée : $OUTPUT"
