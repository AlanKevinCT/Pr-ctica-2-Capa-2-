# Práctica 2 (Capa 2) - Redes de Computadoras

Alumno: Alan Kevin Cano Tenorio (321259967)

## Descripción:

Para realizar la práctica se optópor escribir dos programas en C (receptor.c y emisor.c), esto para que se pueda simular el funcionamiento de la capa 2.

## Ejecución del programa con Docker

Se debe estar en el directorio raiz (donde está el Dockerfile)

### 1. Construir la imagen de Docker
```bash
docker build -t capa_2 .
```

### 2. Crear la red virtual aislada
```bash
docker network create practica_2
```

### 3. Activar el nodo Receptor (se inicia en la interfaz eth0)
```bash
docker run -it --rm --name receptor --network practica_2 --cap-add=NET_RAW --cap-add=NET_ADMIN capa_2 ./receptor
```

### 4. Activar el nodo Emisor (envía por medio de eth0) (se usa en otra terminal)
```bash
docker run -it --rm --name emisor --network practica_2 --cap-add=NET_RAW --cap-add=NET_ADMIN capa_2 ./emisor
```