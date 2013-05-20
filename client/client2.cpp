#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <iostream>

using namespace std;

int main() {
    int sock;
    struct sockaddr_in server;
    char message[1024], response[1024];

    /* Socket erzeugen */
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket konnte nicht erzeugt werden!");
        return 1;
    }

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(1234);

    /* Verbindung aufbauen */
    if (connect(sock, (struct sockaddr*)&server, sizeof(struct sockaddr_in)) < 0) {
        perror("Verbindung fehlgeschlagen!");
        return 1;
    } printf("Verbindung hergestellt!\n");

    while(1) {
        printf("> ");
        scanf("%s", message);

        /* Sende Daten */
        if (write(sock, message, strlen(message), 0) < 0) {
            perror("Senden fehlgeschlagen!");
            return 1;
        }
        /* Daten empfangen */
        if (read(sock, response, 1024, 0) < 0) {
            perror("Lesen fehlgeschlagen!");
            return 1;
        }
        printf("Reply: %s\n", response);
    }
    close(sock);
    return 0;
}

