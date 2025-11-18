#!/bin/bash

if [[ $EUID -ne 0 ]]; then
    echo "[ERROR] Este script debe ejecutarse con sudo."
    echo "Usá:"
    echo "    sudo $0"
    exit 1
fi

PROJECT_NAME="nfs2d"
INSTALL_BIN_DIR="/usr/bin"
INSTALL_VAR_DIR="/var/${PROJECT_NAME}"

echo "======================================"
echo "     DESINSTALADOR DE NFS2D"
echo "======================================"
echo

read -p "¿Seguro que querés desinstalar NFS2D? (y/N): " confirm

case "$confirm" in
    y|Y|yes|YES)
        echo "Desinstalando..."
        ;;
    *)
        echo "Cancelado."
        exit 0
        ;;
esac

echo "== Eliminando binarios =="

if [[ -f "${INSTALL_BIN_DIR}/nfs2d_server" ]]; then
    sudo rm -f "${INSTALL_BIN_DIR}/nfs2d_server"
    echo "✔ nfs2d_server eliminado"
fi

if [[ -f "${INSTALL_BIN_DIR}/nfs2d_client" ]]; then
    sudo rm -f "${INSTALL_BIN_DIR}/nfs2d_client"
    echo "✔ nfs2d_client eliminado"
fi

echo "== Eliminando assets en ${INSTALL_VAR_DIR} =="
if [[ -d "$INSTALL_VAR_DIR" ]]; then
    sudo rm -rf "$INSTALL_VAR_DIR"
    echo "✔ Directorio /var/nfs2d eliminado"
fi

echo
echo "======================================"
echo "   Desinstalación completada"
echo "======================================"
echo
