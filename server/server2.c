#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>

#define PORT 1234
#define BUF_SIZE 1024

int main() {
    int sock1, sock2, received, child_pid;
    struct sockaddr_in server;
    char total_buf[BUF_SIZE];
    char buf[BUF_SIZE];
    char username[255];
    int username_length;

    server.sin_family       = AF_INET;      // Protokollfamilie
    server.sin_addr.s_addr  = htonl(INADDR_ANY);   // Interface
    server.sin_port         = htons(PORT);  // Port

    /* Stream Socket AF_INET erzeugen */
    sock1 = socket(AF_INET, SOCK_STREAM, 0);
    if (sock1 < 0) {
        perror("Socket wurde nicht erzeugt!");
        exit(1);
    }
    /* Socket konfigurieren */
    if (bind(sock1, (struct sockaddr*) &server, sizeof(struct sockaddr_in))) {
        perror("Socket konnte nicht konfiguriert werden!");
        exit(1);
    }
    /* Socket in den listen-Zustand versetzen */
    listen(sock1, 5);
    printf("Socket-Server bereit\n");

    while(1) {
        printf("Erwarte eingehende Verbindung\n");
        /* Verbindung akzeptieren */
        sock2 = accept(sock1, 0, 0);    // BLOCKIERT!

        if (sock2 < 0) {
            perror("Fehler bei accept()");
            continue;
        } else {
            if (username_length = read(sock2, username, 255)) {
                if (received < 0)
                    perror("Fehler beim Lesen");
                else {
                    username[username_length] = '\0';
                    //send(sock2, "yes", 3, 0);
                }
            }
        }

        if((child_pid = fork()) == 0) {
            int total_received = 0;
            while (received = read(sock2, buf, BUF_SIZE)) {
                // Kindprozess
                if (received < 0)
                    perror("Fehler beim Lesen");
                else {
                    strncpy(&total_buf[total_received], buf, received);
                    total_received += received;
                    write(1, "Aff1:\0", 6);
                    write(1, buf, received);
                    write(1, "Aff2:\0", 6);
                    write(1, total_buf, total_received);

                    printf("Charakter: %c\n", buf[received-2]);
                    if (buf[received-1] == 'H') {
                        printf("Hey, was geht\n");
                        write(1, username, username_length);
                        write(1, ": ", 2);
                        write(1, total_buf, total_received);
                        total_buf[total_received] = '\0';
                        //printf("<%s>: %s", username, total_buf);
                        total_received = 0;
                        //send(sock2, total_buf, total_received+1, 0);
                    }
                }
            }
            printf("Ende der Kommunikation\n");
            close(sock2);
        } else if(child_pid > 0) {
            // Elternprozess
        } else {
            perror("Fork fehlgeschlagen!");
            close(sock1); close(sock2);
        }
    }
    close(sock1);
    return 0;
}
