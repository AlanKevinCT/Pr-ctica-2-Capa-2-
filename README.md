# Práctica 2 - Redes de Computadoras (Capa 2)

## Creación y ejecución con Docker

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
docker run -it --rm --name receptor --network practica_2 \
  --cap-add=NET_RAW --cap-add=NET_ADMIN capa_2 ./receptor
```

### 4. Activar el nodo Emisor (envía por medoi de eth0)
```bash
docker run -it --rm --name emisor --network practica_2 \
  --cap-add=NET_RAW --cap-add=NET_ADMIN capa_2 ./emisor
```