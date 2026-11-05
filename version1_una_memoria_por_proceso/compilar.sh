#!/bin/bash
# Compila el padre y los 5 programas de operaciones.
for p in padre suma resta multiplicacion division modulo; do
    if gcc -Wall -Wextra -o "$p" "$p.c"; then
        echo "compilado: $p"
    else
        echo "ERROR al compilar $p"
        exit 1
    fi
done
echo "Listo. Ejecuta: ./padre"
