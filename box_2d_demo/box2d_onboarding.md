# Onboarding – Box2D  
**Proyecto:** Need for Speed 2D  
**Materia:** Taller de Programación I – Cátedra Veiga (FIUBA)  
**Integrante responsable:** Maximo Giovanettoni  
**Rol:** Física del juego (motor Box2D)

---

## Objetivo

El propósito de este onboarding es **comprender el funcionamiento de Box2D** y realizar una **prueba de concepto (PoC)** para validar su integración dentro del TP *Need for Speed 2D*.  
Box2D será utilizado como **motor físico** para manejar los choques, colisiones, masas, velocidades y aceleraciones de los autos y NPCs, evitando implementar la física desde cero.

---

## 1. ¿Qué es Box2D?

**Box2D** es un motor de simulación física en 2D, ampliamente usado en videojuegos (por ejemplo, Angry Birds o Limbo).  
Permite representar el mundo físico mediante:

- **`b2World`** → El mundo físico donde se simulan las interacciones.  
- **`b2Body`** → Cuerpos físicos (autos, NPCs, muros, etc.).  
- **`b2Fixture`** → Define la forma y propiedades físicas de cada cuerpo.  
- **`b2Shape`** → Representa la geometría (rectángulo, círculo, etc.).  
- **`b2ContactListener`** → Detecta colisiones y eventos entre cuerpos.  

Box2D se encargará de **actualizar posiciones, velocidades y colisiones** cada vez que el servidor avance un “frame” del juego.

---

## 2. Instalación

Para Ubuntu 24.04 (entorno usado en el TP):

```bash
sudo apt update
sudo apt install -y libbox2d-dev
