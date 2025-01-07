#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>

#define MAXPENDING 5    /* Max connection requests */
#define BUFFSIZE 512
#define CONNECTED_PORT 9999
void Die(char *mess) { perror(mess); exit(1); }

void HandleClient(int sock) {
    char buffer[BUFFSIZE];
    int received;

    while (1) {
        char message[BUFFSIZE] = "idle"; // Message de réponse par défaut

        // Réception des commandes
        if ((received = recv(sock, buffer, BUFFSIZE - 1, 0)) < 0) {
            Die("Failed to receive initial bytes from client");
        }

        buffer[received] = '\0'; // Terminaison de la chaîne reçue

        // Analyse de la commande et des arguments
        char command[BUFFSIZE];
        char id_client[BUFFSIZE];
        char id_compte[BUFFSIZE];
        char password[BUFFSIZE];
        float somme = 0.0;

        int args = sscanf(buffer, "%s %s %s %s %f", command, id_client, id_compte, password, &somme);

        if (strcmp(command, "exit") == 0) {
          printf("Client disconnected\n");
          break;
        } 
        else if (strcmp(command, "help") == 0){
          strncpy(message, "\nAJOUT <id_client id_compte password somme>\tAjouter au solde\nRETRAIT <id_client id_compte password somme>\tFaire un retrait\nSOLDE <id_client id_compte password>\tAfficher le solde du compte\nOPERATIONS <id_client id_compte password>\tAffiche les 10 dernières opérations\n\nhelp\tAfficher l'aide\nexit\tQuitter\n", BUFFSIZE);
        }
        else if (strcmp(command, "AJOUT") == 0 ||strcmp(command, "ajout") == 0) {
          if (args==5){
            printf("AJOUT demandé par %s pour le compte %s avec somme %.2f\n", id_client, id_compte, somme);
            strncpy(message, "OK - AJOUT effectué", BUFFSIZE);
          }
          else{
            strncpy(message, "KO - Erreur de paramètre - AJOUT <id_client id_compte password somme>", BUFFSIZE);
          }
        } 
        else if (strcmp(command, "RETRAIT")==0||strcmp(command, "retrait") == 0) {
          if (args==5){
            printf("RETRAIT demandé par %s pour le compte %s avec somme %.2f\n", id_client, id_compte, somme);
            strncpy(message, "OK - RETRAIT effectué", BUFFSIZE);
          }
          else{
            strncpy(message, "KO - Erreur de paramètre - RETRAIT <id_client id_compte password somme>", BUFFSIZE);
          }
        } 
        else if (strcmp(command, "SOLDE")==0||strcmp(command, "solde") == 0) {
          if (args == 4){
            printf("SOLDE demandé par %s pour le compte %s\n", id_client, id_compte);
            snprintf(message, BUFFSIZE, "RES_SOLDE - Solde: %.2f, Dernière opération: 2025-01-07", 1234.56);
          }
          else{
            strncpy(message, "KO - Erreur de paramètre - SOLDE <id_client id_compte password>", BUFFSIZE);
          }
        } 
        else if (strcmp(command, "OPERATIONS")==0||strcmp(command, "operations") == 0) {
          if(args == 4){
            printf("OPERATIONS demandé par %s pour le compte %s\n", id_client, id_compte);
            strncpy(message, "RES_OPERATIONS - Liste des 10 dernières opérations", BUFFSIZE);
          }
          else{
            strncpy(message, "KO - Erreur de paramètre - OPERATIONS <id_client id_compte password>", BUFFSIZE);
          }
        } 
        else {
            strncpy(message, "KO - Commande invalide - help  Pour afficher l'aide", BUFFSIZE);
        }

        // Envoyer la réponse au client
        if (send(sock, message, strlen(message), 0) < 0) {
            Die("Failed to send bytes to client");
        }
    }

    close(sock);
}




int main(int argc, char *argv[]) {
  int serversock, clientsock;
  struct sockaddr_in echoserver, echoclient;
  
  /*
  if (argc != 2) {
    fprintf(stderr, "USAGE: echoserver <port>\n");
    exit(1);
  }
  */

  /* Create the TCP socket */
  // PF_INET = AF_INET = IPV4 protocols = TCP,
  //  SOCK_STREAM for TCP, SOCK_DGRAM for UDP
  // IPPROTO_TCP or 0
  if ((serversock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) { 
    Die("Failed to create socket");
  }

  /* Construct the server sockaddr_in structure */
  memset(&echoserver, 0, sizeof(echoserver));       /* Clear struct */
  echoserver.sin_family = AF_INET;                  /* Internet/IP */
  echoserver.sin_addr.s_addr = htonl(INADDR_ANY);   /* Incoming addr */
  //echoserver.sin_port = htons(atoi(argv[1]));       /* server port */
  echoserver.sin_port = htons(CONNECTED_PORT);  
  

  /* Bind the server socket */ //etablissemernt de la connexion 
if (bind(serversock, (struct sockaddr *) &echoserver,
                              sizeof(echoserver)) < 0) {
  Die("Failed to bind the server socket");
}
/* Listen on the server socket */ // listen attend une connexion
if (listen(serversock, MAXPENDING) < 0) {
  Die("Failed to listen on server socket");
}

fprintf(stdout,"Server listening port %d\n", CONNECTED_PORT);

/* Run until cancelled */
  while (1) {
    unsigned int clientlen = sizeof(echoclient);
    /* Wait for client connection */
    if ((clientsock =
          accept(serversock, (struct sockaddr *) &echoclient,
                &clientlen)) < 0) {
      Die("Failed to accept client connection");
    }
    fprintf(stdout, "Client connected: %s\n",
                    inet_ntoa(echoclient.sin_addr));
    HandleClient(clientsock);
  }
}