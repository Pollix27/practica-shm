#!/bin/bash
# Uso: ./monitorear.sh [PID_PADRE]
# Si no se da el PID, busca un proceso llamado "padre".
# Muestra cada INTERVALO segundos: procesos (padre + descendientes), arbol y memoria compartida (ipcs -m).

INTERVALO=3

if [ -n "$1" ]; then
    pid_padre=$1
else
    pid_padre=$(pgrep -x padre | head -n 1)
fi

if [ -z "$pid_padre" ] || ! kill -0 "$pid_padre" 2>/dev/null; then
    echo "Error: no se encontro un proceso padre valido (PID='$pid_padre')."
    echo "Uso: $0 <PID_PADRE>   (primero ejecuta ./padre en otra terminal)"
    exit 1
fi

# Imprime los descendientes de un PID de forma recursiva (hijos, nietos, ...)
descendientes() {
    local h
    for h in $(pgrep -P "$1"); do
        echo "$h"
        descendientes "$h"
    done
}

# Dibuja el arbol con sangria
arbol() {
    local pid=$1 pref=$2 nombre h
    nombre=$(ps -o comm= -p "$pid" 2>/dev/null)
    echo "${pref}${nombre} (${pid})"
    for h in $(pgrep -P "$pid"); do
        arbol "$h" "${pref}    "
    done
}

echo "Monitoreando al proceso padre PID=$pid_padre"
muestra=0
while kill -0 "$pid_padre" 2>/dev/null; do
    muestra=$((muestra + 1))
    todos="$pid_padre $(descendientes "$pid_padre")"
    total=$(echo $todos | wc -w | tr -d ' ')
    lista=$(echo $todos | tr ' ' ',')

    echo "=========================================================="
    echo "Muestra #$muestra   Hora: $(date '+%H:%M:%S')"
    echo "Total de procesos (padre + hijos): $total"
    echo "----------------------------------------------------------"
    ps -o pid,ppid,user,stat,%cpu,%mem,comm,command -p "$lista"
    echo "----------------------------------------------------------"
    echo "Arbol de procesos:"
    arbol "$pid_padre" ""
    echo "----------------------------------------------------------"
    echo "Memoria compartida (ipcs -m):"
    if [ "$(uname)" = "Darwin" ]; then
        ipcs -m -a
    else
        ipcs -m
    fi
    echo ""
    sleep "$INTERVALO"
done

echo "El proceso padre $pid_padre ya termino. Fin del monitoreo."
