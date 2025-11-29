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
BIN_EDITOR="./build/nfs2d_editor"

INSTALL_BIN_DIR="/usr/bin"
INSTALL_VAR_DIR="/var/${PROJECT_NAME}"

DEPENDENCIAS=(
   cmake
   make
   g++
   libopus-dev
   libopusfile-dev
   libxmp-dev
   libfluidsynth-dev
   fluidsynth
   libwavpack1
   libwavpack-dev
   libfreetype-dev
   wavpack
   libyaml-cpp-dev
)

instalar_dependencias() {
   echo "== Verificando dependencias =="

   # Detectar gestor de paquetes
   if command -v apt >/dev/null 2>&1; then
       PKG_MANAGER="apt"
       INSTALL_CMD="apt install -y"
       QUERY_CMD="dpkg -s"
   elif command -v dnf >/dev/null 2>&1; then
       PKG_MANAGER="dnf"
       INSTALL_CMD="dnf install -y"
       QUERY_CMD="dnf list installed"
   elif command -v pacman >/dev/null 2>&1; then
       PKG_MANAGER="pacman"
       INSTALL_CMD="pacman -S --noconfirm"
       QUERY_CMD="pacman -Qi"
   else
       echo "[ERROR] No se pudo detectar un gestor de paquetes compatible."
       exit 1
   fi

   echo "→ Gestor detectado: $PKG_MANAGER"
   echo

   for pkg in "${DEPENDENCIAS[@]}"; do
       echo -n "Comprobando $pkg... "
       if $QUERY_CMD "$pkg" >/dev/null 2>&1; then
           echo "OK"
       else
           echo "faltante → instalando..."
           $INSTALL_CMD "$pkg"
       fi
   done
}

instalar_dependencias

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

if [[ -f "$BIN_EDITOR" ]]; then
    cp "$BIN_EDITOR" "$INSTALL_BIN_DIR/"
    chmod +x "${INSTALL_BIN_DIR}/nfs2d_editor"
    echo "✔ nfs2d_editor instalado"
else
    echo "✖ No se encontró $BIN_EDITOR"
fi

./build/nfs2d_tests

echo
echo "=========================================="
echo "   Instalación completada correctamente"
echo "=========================================="
echo
