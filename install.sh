#!/bin/bash

if [[ $EUID -ne 0 ]]; then
    echo "[ERROR] Este script debe ejecutarse con sudo."
    echo "Usá:"
    echo "    sudo $0"
    exit 1
fi

PROJECT_NAME="nfs2d"
BIN_SERVER="./build/nfs2d_server"
BIN_CLIENT="./build/nfs2d_client"

INSTALL_BIN_DIR="/usr/bin"
INSTALL_VAR_DIR="/var/${PROJECT_NAME}"

set -e

echo "== Compilando proyecto =="
make

echo "== Preparando directorio de assets =="
mkdir -p "${INSTALL_VAR_DIR}"

if [[ -d "${INSTALL_VAR_DIR}/assets" ]]; then
    echo "→ Eliminando assets anteriores..."
    rm -rf "${INSTALL_VAR_DIR}/assets"
    rm -rf "${INSTALL_VAR_DIR}/tours"
    rm -rf "${INSTALL_VAR_DIR}/tracks"
    rm -rf "${INSTALL_VAR_DIR}/cities"
fi

echo "→ Copiando nuevos assets..."
cp -r ./assets "${INSTALL_VAR_DIR}/"
cp -r ./tours "${INSTALL_VAR_DIR}/"
cp -r ./tracks "${INSTALL_VAR_DIR}/"
cp -r ./cities "${INSTALL_VAR_DIR}/"

echo "== Instalando binarios en ${INSTALL_BIN_DIR} =="

if [[ -f "$BIN_SERVER" ]]; then
    cp "$BIN_SERVER" "$INSTALL_BIN_DIR/"
    chmod +x "${INSTALL_BIN_DIR}/nfs2d_server"
    echo "✔ nfs2d_server instalado"
else
    echo "✖ No se encontró $BIN_SERVER"
fi

if [[ -f "$BIN_CLIENT" ]]; then
    cp "$BIN_CLIENT" "$INSTALL_BIN_DIR/"
    chmod +x "${INSTALL_BIN_DIR}/nfs2d_client"
    echo "✔ nfs2d_client instalado"
else
    echo "✖ No se encontró $BIN_CLIENT"
fi

echo
echo "======================================"
echo "   Instalación completada correctamente"
echo "======================================"
echo
