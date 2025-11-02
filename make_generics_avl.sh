#!/usr/bin/env bash
set -euo pipefail
IFS=$'\n\t'

readonly function_decls=($(grep -Po "^(void |int |tree_node_t \*)[a-zA-Z][a-zA-Z_]*\(.*\)" avl.c))
readonly functions=($(grep -Po "^(void |int |tree_node_t \*)[a-zA-Z][a-zA-Z_]*\(.*\)" avl.c | sed -E "s/^(void |int |tree_node_t \*)([a-zA-Z][a-zA-Z_]*)\(.*\)/\2/"))
readonly files_with_defs=($(grep -l "^TREE_NODE_DEF" ./{**/,}*.{c,h} | sort | uniq))

# TODO: this clearly isn't POSIX
readonly tree_node_names=($(/usr/bin/grep -hP "^TREE_NODE_DEF" ./{**/,}*.{c,h} | rg -oP "(?<=,).*(?=\))" | tr -d " " | sort | uniq))
readonly OUTDIR="${1:-out}"
readonly SRC_AVL="${2:-./avl.c}"

readonly TYPE_TO_REPLACE="${3:-tree_node_t}"
readonly REPLACEMENT_TYPE_PREFIX="${4:-tree_node_}"

# for name in ${tree_node_names[@]}; do
for i in $(seq 0 $((${#tree_node_names[@]}-1))); do
  name="${tree_node_names[$i]}"
  avl_name="avl_$name"
  outfile_header="$OUTDIR/$avl_name.h"
  outfile="$OUTDIR/$avl_name.c"

  sed "$SRC_AVL" -E -e "s/$TYPE_TO_REPLACE/$REPLACEMENT_TYPE_PREFIX${name}_t/g" \
    -e "s/avl_cmp_t/avl_cmp_$name/g" \
    -e "s/avl_print_t/avl_print_$name/g" \
    -e "s/^#include.*$//" \
    > "$outfile"

  for f in ${functions[@]}; do
    echo "$f"
    sed -i "s/$f(/${name}_$f(/g" "$outfile"
  done

  echo "#include \"${outfile_header##*/}\"" | cat - "$outfile" > "$outfile.new"
  mv "$outfile.new" "$outfile"

  header_guard_macron="${avl_name^^}_H"
  echo -e "#ifndef $header_guard_macron\n#define $header_guard_macron\n#include <stdbool.h>\n" > "$outfile_header"
  echo "#include \"../${files_with_defs[$i]}\"" >> "$outfile_header"
  for (( i=0; i < ${#function_decls[@]}; i++ )); do
    f="${function_decls[$i]}"
    echo -e "${f//${functions[$i]}/"${name}_${functions[$i]}"};\n" >> "$outfile_header"
  done

  sed -i "$outfile_header" -e "s/$TYPE_TO_REPLACE/$REPLACEMENT_TYPE_PREFIX${name}_t/g" \
    -e "s/avl_cmp_t/avl_cmp_$name/g" \
    -e "s/avl_print_t/avl_print_$name/g"

  echo "#endif" >> "$outfile_header"
done
