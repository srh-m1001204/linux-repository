#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <iostream>
#include <vector>

using namespace std;

#define PORT 1234
#define BUF_SIZE 1024

int main() {
    int sock1, sock2, sock3, received, child_pid;
    struct sockaddr_in server;
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
    //sock3 = accept(sock1, 0, 0);

    printf("Socket-Server bereit\n");

    while(1) {
        printf("Erwarte eingehende Verbindung\n");
        /* Verbindung akzeptieren */
        sock2 = accept(sock1, 0, 0);    // BLOCKIERT!
        cout << "sock: " << sock2 << endl;
        if((child_pid = fork()) == 0) {
            if (sock2 < 0) {
                perror("Fehler bei accept()");
                continue;
            } else {
                if (username_length = read(sock2, username, 255)) {
                    if (username_length < 0)
                        perror("Fehler beim Lesen");
                    else {
                        username[username_length] = '\0';
                        write(1, username, username_length);
                        write(1, " hat sich angemeldet.\n", 32);
                        //send(sock2, "yes", 3, 0);
                    }
                }
            }
            while (received = read(sock2, buf, BUF_SIZE)) {
                // Kindprozess
                write(1, username, username_length);
                write(1, " sagt: ", 7);
                write(1, buf, received);
                write(1, "\n", 1);
                buf[received] = '\0';
                send(sock2, buf,received+1, 0);
            }
            printf("Ende der Kommunikation\n");
            close(sock2);
        } else if(child_pid > 0) {
            // Elternprozess
            cout << "Hi, ich bin der Vater\n";
        } else {
            perror("Fork fehlgeschlagen!");
            close(sock2);
        }
    }
    close(sock1);
    return 0;
}
