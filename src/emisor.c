#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <net/ethernet.h>
#include <linux/if_packet.h>

// Constantes
#define INTERFAZ_RED "eth0"
#define ETHERTYPE_EXP 0x88B5
#define TAM_MAX_BUFFER 1514

// Método para crear un socket
int crear_socket(uint16_t ethertype) {
    int descriptor_socket = socket(AF_PACKET, SOCK_RAW, htons(ethertype));
    if (descriptor_socket < 0) {
        perror("Error al crear el socket");
    }
    return descriptor_socket;
}

// Método para obtener el índice de la interfaz de red
int obtener_indice_interfaz(int descriptor_socket, const char *nombre_interface) {
    struct ifreq request_interface;
    memset(&request_interface, 0, sizeof(request_interface));

    // Copia el nombre de la interfaz
    strncpy(request_interface.ifr_name, nombre_interface, IFNAMSIZ - 1);

    // Consulta el índice de la interfaz
    if (ioctl(descriptor_socket, SIOCGIFINDEX, &request_interface) < 0) {
        perror("Error al obtener índice de interfaz");
        return -1;
    }
    return request_interface.ifr_ifindex;
}

// Método para obtener la dirección MAC de origen de la interfaz local
int obtener_mac_local(int descriptor_socket, const char *nombre_interface, unsigned char *mac_out) {
    struct ifreq request_interface;
    memset(&request_interface, 0, sizeof(request_interface));
    strncpy(request_interface.ifr_name, nombre_interface, IFNAMSIZ - 1);

    // Obtiene la dirección MAC de la interfaz
    if (ioctl(descriptor_socket, SIOCGIFHWADDR, &request_interface) < 0) {
        perror("Error al obtener la dirección MAC local");
        return -1;
    }

    memcpy(mac_out, request_interface.ifr_hwaddr.sa_data, 6);
    return 0;
}


// Método para construir una trama de la capa 2 en el búfer byte por byte
int construir_trama_capa2(unsigned char *buffer, const unsigned char *mac_destino, const unsigned char *mac_origen, uint16_t ethertype, const char *payload) {
    int desplazamiento = 0;

    // Copia de la dirección MAC de destino
    memcpy(buffer + desplazamiento, mac_destino, 6);
    desplazamiento += 6;

    // Copia de la dirección MAC de origen
    memcpy(buffer + desplazamiento, mac_origen, 6);
    desplazamiento += 6;

    // Copia del Ethertype
    uint16_t proto_net = htons(ethertype);
    memcpy(buffer + desplazamiento, &proto_net, 2);
    desplazamiento += 2;

    // Copia del payload
    size_t len_payload = strlen(payload);
    memcpy(buffer + desplazamiento, payload, len_payload);
    desplazamiento += len_payload;

    return desplazamiento;
}

// Método para transmitir la trama a través del socket
int transmitir_trama(int descriptor_socket, int indice_interface, const unsigned char *mac_destino, const unsigned char *trama, int tam_trama) {
    struct sockaddr_ll direccion_red;
    memset(&direccion_red, 0, sizeof(direccion_red));
    direccion_red.sll_family = AF_PACKET;
    direccion_red.sll_ifindex = indice_interface;
    direccion_red.sll_halen = 6;
    memcpy(direccion_red.sll_addr, mac_destino, 6);

    // Se envía la trama a través del socket
    ssize_t enviados = sendto(descriptor_socket, trama, tam_trama, 0, (struct sockaddr *)&direccion_red, sizeof(direccion_red));

    return (enviados > 0) ? 0 : -1;
}



// Método principal para ejecutar al emisor
int main(void) {
    unsigned char mac_origen[6];
    unsigned char mac_destino[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    unsigned char paquete[TAM_MAX_BUFFER];
    const char *mensaje = "Hola Redes2027-1";

    int descriptor_socket = crear_socket(ETHERTYPE_EXP);
    if (descriptor_socket < 0) return EXIT_FAILURE;

    int indice_interface = obtener_indice_interfaz(descriptor_socket, INTERFAZ_RED);
    if (indice_interface < 0) { close(descriptor_socket); return EXIT_FAILURE; }

    if (obtener_mac_local(descriptor_socket, INTERFAZ_RED, mac_origen) < 0) {
        close(descriptor_socket);
        return EXIT_FAILURE;
    }

    int tam_trama = construir_trama_capa2(paquete, mac_destino, mac_origen, ETHERTYPE_EXP, mensaje);

    if (transmitir_trama(descriptor_socket, indice_interface, mac_destino, paquete, tam_trama) == 0) {
        printf("El Mensaje enviado es: %s en %s.\n", mensaje, INTERFAZ_RED);
    } else {
        perror("Error en la transmisión de la trama");
    }

    close(descriptor_socket);
    return EXIT_SUCCESS;
}