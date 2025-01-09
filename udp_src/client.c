#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>

#define BUFFSIZE 512
#define CONNECTED_PORT 9999
#define SERVER_IP "0.0.0.0" // Remplacez par l'adresse IP du serveur

void Die(char *mess) {
    perror(mess);
    exit(1);
}

int main() {
    int sock;
    struct sockaddr_in echoserver;
    char buffer[BUFFSIZE];

    /* Create the UDP socket */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        Die("Failed to create socket");
    }

    /* Construct the server sockaddr_in structure */
    memset(&echoserver, 0, sizeof(echoserver));        /* Clear struct */
    echoserver.sin_family = AF_INET;                   /* Internet/IP */
    echoserver.sin_addr.s_addr = inet_addr(SERVER_IP); /* Server IP address */
    echoserver.sin_port = htons(CONNECTED_PORT);       /* Server port */

    fprintf(stdout, "Session ouverte - type \"help\" for help\n");

    while (1) {
        char commande[BUFFSIZE]; // Tampon pour la commande complète

        // Demander une commande
        fprintf(stdout, "> ");
        fgets(commande, BUFFSIZE, stdin); // Lire la commande complète avec arguments
        commande[strcspn(commande, "\n")] = '\0'; // Supprimer le caractère '\n'

        // Envoyer la commande au serveur
        size_t echolen = strlen(commande);
        if (sendto(sock, commande, echolen, 0, (struct sockaddr *) &echoserver, sizeof(echoserver)) != echolen) {
            Die("Mismatch in number of sent bytes");
        }

        // Vérifier si l'utilisateur souhaite quitter
        if (strncmp(commande, "exit", 4) == 0 || strncmp(commande, "quit", 4) == 0) {
            printf("Session closed\n");
            break;
        }

        // Recevoir la réponse du serveur
        socklen_t serverlen = sizeof(echoserver);
        int bytes = recvfrom(sock, buffer, BUFFSIZE - 1, 0, (struct sockaddr *) &echoserver, &serverlen);
        if (bytes < 1) {
            Die("Failed to receive bytes from server");
        }

        // Assurer une chaîne terminée par \0
        buffer[bytes] = '\0';
        fprintf(stdout, "Response: \n%s\n", buffer);
    }

    // Fermer la connexion et quitter
    close(sock);
    return 0;
}
