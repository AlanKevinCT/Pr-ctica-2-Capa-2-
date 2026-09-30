#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/if_packet.h>

#define INTERFAZ_RED "eth0"
#define ETHERTYPE_EXP 0x88B5
#define TAM_MAX_BUFFER 1514

// Variable global para capturar la señal de interrupción (Ctrl+C)
volatile sig_atomic_t ejecutando = 1;

// Esto es para evitar que se cicle sin cerrarse el programa.
void manejar_sigint(int sig) {
    (void)sig;
    ejecutando = 0;
}

// Método para crear el socket receptor
int crear_socket(uint16_t ethertype) {
    int descriptor_socket = socket(AF_PACKET, SOCK_RAW, htons(ethertype));
    if (descriptor_socket < 0) {
        perror("Error al crear el socket");
    }
    return descriptor_socket;
}

// Método para procesar y desempaquetar la trama recibida
void procesar_trama(const unsigned char *buffer, ssize_t tam_trama) {
    if (tam_trama < 14) return;

    unsigned char mac_dst[6];
    unsigned char mac_src[6];
    uint16_t ethertype;

    // Desempaquetado manual
    memcpy(mac_dst, buffer, 6);
    memcpy(mac_src, buffer + 6, 6);
    memcpy(&ethertype, buffer + 12, 2);

    uint16_t ethertype_host = ntohs(ethertype);

    // Filtrar únicamente tramas con EtherType experimental (0x88B5)
    if (ethertype_host == ETHERTYPE_EXP) {
        printf("\n=== ¡TRAMA CAPA 2 RECIBIDA! ===\n");
        printf("Iniciando procesamiento pesado de 5 segundos...\n");
        sleep(5);

        printf("MAC Origen:  %02X:%02X:%02X:%02X:%02X:%02X\n",
               mac_src[0], mac_src[1], mac_src[2], mac_src[3], mac_src[4], mac_src[5]);

        printf("MAC Destino: %02X:%02X:%02X:%02X:%02X:%02X (Broadcast)\n",
               mac_dst[0], mac_dst[1], mac_dst[2], mac_dst[3], mac_dst[4], mac_dst[5]);

        printf("EtherType:   0x%04X (Experimental)\n", ethertype_host);

        // Extracción del Payload
        int tam_payload = tam_trama - 14;
        char mensaje[TAM_MAX_BUFFER];
        memcpy(mensaje, buffer + 14, tam_payload);
        mensaje[tam_payload] = '\0';

        printf("Mensaje:     %s\n", mensaje);
        printf("=== Procesamiento completado con éxito ===\n\n");
    }
}

// Bucle principal de escucha
void escuchar_tramas(int descriptor_socket) {
    unsigned char buffer[TAM_MAX_BUFFER];

    printf("Escuchando de forma cruda en %s esperando tramas experimentales...\n", INTERFAZ_RED);

    while (ejecutando) {
        ssize_t bytes_recibidos = recvfrom(descriptor_socket, buffer, sizeof(buffer), 0, NULL, NULL);
        if (bytes_recibidos < 0) {
            if(!ejecutando) break; // Salir si se recibió la señal de interrupción 
            perror("Error al recibir trama");
            continue;
        }

        procesar_trama(buffer, bytes_recibidos);
    }
    printf("\nDeteniendo el receptor de forma limpia...\n");
}

int main(void) {
    signal(SIGINT, manejar_sigint);
    int descriptor_socket = crear_socket(ETHERTYPE_EXP);
    if (descriptor_socket < 0) return EXIT_FAILURE;
    escuchar_tramas(descriptor_socket);
    close(descriptor_socket);
    return EXIT_SUCCESS;
}