#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>

#define BUFFSIZE 512
#define CONNECTED_PORT 9999
#define IP_ADDRESS "0.0.0.0"
void Die(char *mess) { perror(mess); exit(1); }



int main(int argc, char *argv[]) {
  int sock;
  struct sockaddr_in echoserver;
  char buffer[BUFFSIZE];
  unsigned int echolen;
  int received = 0;

  /*
  if (argc != 4) {
    fprintf(stderr, "USAGE: TCPecho <server_ip> <word> <port>\n");
    exit(1);
  }
  */

  /* Create the TCP socket */
  if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
    Die("Failed to create socket");
  }

/* Construct the server sockaddr_in structure */
  memset(&echoserver, 0, sizeof(echoserver));       /* Clear struct */
  echoserver.sin_family = AF_INET;                  /* Internet/IP */
  //echoserver.sin_addr.s_addr = inet_addr(argv[1]);  /* IP address */
  echoserver.sin_addr.s_addr = inet_addr(IP_ADDRESS); 
  //echoserver.sin_port = htons(atoi(argv[3]));       /* server port */
  echoserver.sin_port = htons(CONNECTED_PORT);
  /* Establish connection */
  if (connect(sock,
              (struct sockaddr *) &echoserver,
              sizeof(echoserver)) < 0) {
    Die("Failed to connect with server");
  }

  fprintf(stdout,"Session ouverte - type \"help\" for help\n");

/* Send the word to the server */
while (1) {
    char buffer[BUFFSIZE];   // Tampon pour la réponse du serveur
    char commande[BUFFSIZE]; // Tampon pour la commande complète

    // Demander une commande
    fprintf(stdout, "> ");
    fgets(commande, BUFFSIZE, stdin); // Lire la commande complète avec arguments
    commande[strcspn(commande, "\n")] = '\0'; // Supprimer le caractère '\n'

    // Envoyer la commande au serveur
    size_t echolen = strlen(commande);
    if (send(sock, commande, echolen, 0) != echolen) {
        Die("Mismatch in number of sent bytes");
    }

    // Vérifier si l'utilisateur souhaite quitter
        if (strncmp(commande, "exit", 4) == 0 || strncmp(commande, "quit", 4) == 0) {
            printf("Session closed\n");
            break;
        }

    // Recevoir la réponse du serveur
    int bytes = recv(sock, buffer, BUFFSIZE - 1, 0);
    if (bytes < 1) {
        Die("Failed to receive bytes from server");
    }
    
    

    // Assurer une chaîne terminée par \0
    buffer[bytes] = '\0';
    fprintf(stdout, "Response: \n%s\n", buffer);

    
}

// Fermer la connexion et quitter
fprintf(stdout, "\n");
close(sock);
exit(0);

}