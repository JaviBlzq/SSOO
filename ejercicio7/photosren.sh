#!/bin/sh


if [ "$#" -ne 1 ]; then
  echo "Uso: $0 <directorio>"
  exit 1
fi

dir="$1"

if [ ! -d "$dir" ]; then
  echo "Error: '$dir' no es un directorio válido."
  exit 1
fi

current_date=$(date +%d_%b_%Y)

date_dir="$dir/$current_date"
mkdir -p "$date_dir"

counter=0

for file in "$dir"/*; do
  if [ -f "$file" ] && file "$file" | grep -qE 'image|bitmap'; then
    ext="$(echo "$file" | awk -F . '{print tolower($NF)}')"
    new_name=$(printf "%03d.%s" "$counter" "$ext")
    
    mv "$file" "$date_dir/$new_name"
    
    counter=$((counter + 1))
  fi
done
