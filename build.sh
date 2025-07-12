#!/bin/bash

# Script para limpiar y compilar el proyecto PasswordGenerator

# Limpiar archivos generados previamente
make clean

# Eliminar el Makefile
rm -f Makefile

# Generar un nuevo Makefile con qmake6
qmake6

# Compilar el proyecto
make

./PasswordGenerator

# Verificar si la compilación fue exitosa
if [ $? -eq 0 ]; then
    echo "Compilación exitosa. El ejecutable 'PasswordGenerator' ha sido generado."
else
    echo "Error durante la compilación. Revisa los mensajes de error anteriores."
    exit 1
fi
