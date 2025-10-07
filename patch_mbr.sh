#!/bin/bash
set -euo pipefail

if [ $# -eq 0 ]; then 
  echo "$0 <boot.bin> <part.img>"
  exit 0
fi

readonly OUTFILE="${1}.mbr"
readonly SECTORS_OF_PART="$(bc <<< "obase=16; $(du -b $2 | cut -f1) / 512 + 1")"

cp $1 $OUTFILE

# echo "001BE: 8000 0000 0000 0000 $(printf "%04x" "$(bc <<< "obase=16; $(du -b $1 | cut -f1) + 1")") $(printf "%04x" "$SECTORS_OF_PART")"
echo "001BE: 8000 0100 0000 0100 $(printf "%04x" "$(bc <<< "obase=16; $(du -b $1 | cut -f1) + 1")") $(printf "%04x" "$SECTORS_OF_PART")" | xxd -r - $OUTFILE
