#!/bin/bash

if [ -z "$1" ]; then
    echo "Uso: $0 <PID_DEL_PROGRAMA>"
    exit 1
fi

pid_raiz=$1

if ! kill -0 "$pid_raiz" 2>/dev/null; then
    echo "Error: el PID $pid_raiz no existe o no es válido."
    exit 1
fi

INTERVALO=2
DURACION=30
iteraciones=$((DURACION / INTERVALO))

echo "Monitoreando hilos del proceso principal PID=$pid_raiz"
echo ""

for ((i=1; i<=iteraciones; i++)); do

    if ! kill -0 "$pid_raiz" 2>/dev/null; then
        echo "El proceso principal $pid_raiz ya terminó. Deteniendo monitoreo."
        break
    fi

    echo "=========================================================="
    echo "Hora: $(date '+%H:%M:%S')"
    echo "----------------------------------------------------------"
    # Cabecera para identificar PID, LWP (ID de hilo), CPU y comando
    printf "%-8s %-8s %-6s %-6s %-15s %s\n" "PID" "LWP" "CPU%" "MEM%" "ESTADO" "COMANDO"
    
    # El comando 'ps -T' muestra los hilos asociados al proceso
    ps -T -o pid=,lwp=,%cpu=,%mem=,stat=,comm= -p "$pid_raiz" 2>/dev/null

    echo "=========================================================="
    echo ""

    sleep "$INTERVALO"
done

echo "Monitoreo de hilos finalizado."