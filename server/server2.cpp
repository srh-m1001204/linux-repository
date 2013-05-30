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

    int fd[2];
    int nbytes;
    char client_mngr_buf[BUF_SIZE];

    pipe(fd);

    vector<int> client_sock;

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
    int client_mngr_pid = fork();

    if (client_mngr_pid == 0) {
        close(fd[1]);
        cout << "Client Manager wartet auf Anweisung...\n";
        while(1) {
            if ((nbytes = read(fd[0], client_mngr_buf, BUF_SIZE)) > 0) {
                write(1, "bekomme: ", 16);
                write(1, client_mngr_buf, nbytes);
                write(1, "\n", 1);
            }
        }
    } else if (client_mngr_pid < 0) {
        perror("Client Manager konnte nicht erstellt werden!");
        exit(1);
    }
    close(fd[0]);

    while(1) {
        printf("Erwarte eingehende Verbindung\n");
        /* Verbindung akzeptieren */
        sock2 = accept(sock1, 0, 0);    // BLOCKIERT!
        client_sock.push_back(sock2);
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
                write(fd[1], "Befehl wurde gesendet...", 32);
            }
            printf("Ende der Kommunikation\n");
            close(sock2);
        } else if(child_pid > 0) {
            // Elternprozess
            cout << "Hi, ich bin der Vater\n";
            for (int i=0; i<client_sock.size(); i++) {
                cout << "sock " << i << ": " << client_sock[i] << endl;
                write(client_sock[i], "fick dich, du Socke", 32);
            }
        } else {
            perror("Fork fehlgeschlagen!");
            close(sock2);
        }
    }
    close(sock1);
    return 0;
}
