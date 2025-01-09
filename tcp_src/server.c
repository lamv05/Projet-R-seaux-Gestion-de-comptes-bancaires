#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sqlite3.h>


#define MAXPENDING 5    /* Max connection requests */
#define BUFFSIZE 512
#define CONNECTED_PORT 9998
#define DATABASE "bank.db"

void Die(char *mess) { perror(mess); exit(1); }

int check_args_validity(char id_client[],char id_compte[],char password[],char message[]){
  sqlite3 *db;
  sqlite3_stmt *stmt1,*stmt2;
  char *err_msg = 0;
  
  int rc = sqlite3_open(DATABASE, &db);
  
  if (rc != SQLITE_OK) {
      
      fprintf(stderr, "Cannot open database: %s\n", 
              sqlite3_errmsg(db));
      sqlite3_close(db);
      
      return 1;
  }
  
  char *sql1 = "SELECT * FROM client WHERE id=?";
      
  rc = sqlite3_prepare_v2(db, sql1, -1, &stmt1, NULL);
  
  if (rc == SQLITE_OK) {
        
    sqlite3_bind_text(stmt1, 1, id_client, -1, SQLITE_STATIC);
  } else {
    fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
  }
  int step1 = sqlite3_step(stmt1);
    
    if (step1 == SQLITE_ROW) {
      
      char cli_password[BUFFSIZE];
      const unsigned char *text1 = sqlite3_column_text(stmt1, 3);
      if (text1 != NULL) {
          strncpy(cli_password, (const char *)text1, sizeof(cli_password) - 1);
          cli_password[sizeof(cli_password) - 1] = '\0'; // Assurez la terminaison de la chaîne
      }      

      if (strcmp(cli_password, password)!=0){
        strncpy(message, "KO - Password incorrect", BUFFSIZE);
        sqlite3_close(db);
        return 1; 
      }   
    }
    else{
      strncpy(message, "KO - L'identifiant entré n'existe pas", BUFFSIZE);
      sqlite3_close(db);
      return 1;
    }

  sqlite3_finalize(stmt1);

  char *sql2 = "SELECT * FROM account WHERE id=?";

  rc = sqlite3_prepare_v2(db, sql2, -1, &stmt2, NULL);

  if (rc == SQLITE_OK) {
        
    sqlite3_bind_text(stmt2, 1, id_compte, -1, SQLITE_STATIC);
  } else {
    fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
  }
  int step2 = sqlite3_step(stmt2);

  if (step2!=SQLITE_ROW){
    strncpy(message, "KO - Le compte entré n'existe pas", BUFFSIZE);
    sqlite3_close(db);
    return 1; 
  }

  char id_cli[BUFFSIZE];
  const unsigned char *text2 = sqlite3_column_text(stmt2, 2);
  if (text2 != NULL) {
    strncpy(id_cli, (const char *)text2, sizeof(id_cli) - 1);
    id_cli[sizeof(id_cli) - 1] = '\0'; // Assurez la terminaison de la chaîne
  }   
  if (strcmp(id_cli,id_client)){
    strncpy(message, "KO - Vous n'êtes titulaire de ce compte", BUFFSIZE);
    sqlite3_close(db);
    return 1; 
  }

  sqlite3_finalize(stmt2);
  sqlite3_close(db);
  
  return 0;
}


void add_operation(char id_client[],char id_compte[],float somme,char op[]){
  sqlite3 *db;
  sqlite3_stmt *stmt;
  char *err_msg = 0;

  int rc = sqlite3_open(DATABASE, &db);
  
  if (rc != SQLITE_OK) {
      
      fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
      sqlite3_close(db);
      
      return;
  }
    
  char *sql = "INSERT INTO operation(montant,op,account_id,client_id) VALUES(?,?,?,?);";

  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  
  if (rc == SQLITE_OK) {
        
    sqlite3_bind_double(stmt, 1, (double)somme);
    sqlite3_bind_text(stmt, 2, op, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, id_compte, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, id_client, -1, SQLITE_STATIC);
  } else {
    fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
  }

  int step = sqlite3_step(stmt);

  sqlite3_finalize(stmt);

  sqlite3_close(db);
}

// pas de test sur le type de somme
int ajout(char id_client[],char id_compte[],float somme,char message[]){
  sqlite3 *db;
  sqlite3_stmt *stmt1,*stmt2;
  double solde;
  char *err_msg = 0;
  
  int rc = sqlite3_open(DATABASE, &db);
  
  if (rc != SQLITE_OK) {
      
      fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
      sqlite3_close(db);
      
      return 1;
  }
    
  char *sql1 = "SELECT solde FROM account WHERE id = ?";

  rc = sqlite3_prepare_v2(db, sql1, -1, &stmt1, NULL);
  
  if (rc == SQLITE_OK) {
        
    sqlite3_bind_text(stmt1, 1, id_compte, -1, SQLITE_STATIC);
  } else {
    fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
  }

  int step1 = sqlite3_step(stmt1);
  
  if (step1 == SQLITE_ROW) {
      solde = sqlite3_column_double(stmt1, 0); 
  } 
  else{
      printf("No data found for the specified account.\n");
  }

  sqlite3_finalize(stmt1);

  solde = solde + somme;  

  char *sql2 = "UPDATE account SET solde=? WHERE id=?";

  rc = sqlite3_prepare_v2(db, sql2, -1, &stmt2, NULL);

  if (rc == SQLITE_OK) {
        
    sqlite3_bind_double(stmt2, 1, solde);
    sqlite3_bind_text(stmt2, 2, id_compte, -1, SQLITE_STATIC);
  } else {
    fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
  }

  int step2 = sqlite3_step(stmt2);

  sqlite3_finalize(stmt2);

  sqlite3_close(db);

  snprintf(message, BUFFSIZE, "OK - Ajout effectué - Nouveau solde du compte %s : %.2f", id_compte, solde);

  add_operation(id_client,id_compte,somme,"ajout");
  
  return 0;
}

// pas de test sur le type de somme
int retrait(char id_client[],char id_compte[],float somme,char message[]){
  sqlite3 *db;
  sqlite3_stmt *stmt1,*stmt2;
  double solde;
  char *err_msg = 0;
  
  int rc = sqlite3_open(DATABASE, &db);
  
  if (rc != SQLITE_OK) {
      
      fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
      sqlite3_close(db);
      
      return 1;
  }
    
  char *sql1 = "SELECT solde FROM account WHERE id = ?";

  rc = sqlite3_prepare_v2(db, sql1, -1, &stmt1, NULL);
  
  if (rc == SQLITE_OK) {
        
    sqlite3_bind_text(stmt1, 1, id_compte, -1, SQLITE_STATIC);
  } else {
    fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
  }

  int step1 = sqlite3_step(stmt1);
  
  if (step1 == SQLITE_ROW) {
      solde = sqlite3_column_double(stmt1, 0); 
  } 
  else{
      printf("No data found for the specified account.\n");
  }

  sqlite3_finalize(stmt1);

  solde = solde - somme;  

  char *sql2 = "UPDATE account SET solde=? WHERE id=?";

  rc = sqlite3_prepare_v2(db, sql2, -1, &stmt2, NULL);

  if (rc == SQLITE_OK) {
        
    sqlite3_bind_double(stmt2, 1, solde);
    sqlite3_bind_text(stmt2, 2, id_compte, -1, SQLITE_STATIC);
  } else {
    fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
  }

  int step2 = sqlite3_step(stmt2);

  sqlite3_finalize(stmt2);

  sqlite3_close(db);

  if (solde<0){
    snprintf(message, BUFFSIZE, "OK - Retrait effectuée - Solde restant du compte %s : %.2f - Attention compte à découvert", id_compte, solde);
  }
  else{
    snprintf(message, BUFFSIZE, "OK - Retrait effectuée - Solde restant du compte %s : %.2f", id_compte, solde);
  }

  add_operation(id_client,id_compte,somme,"retrait");
  
  return 0;
}

int solde(char id_compte[],char message[]){
  sqlite3 *db;
  sqlite3_stmt *stmt1,*stmt2;
  char *err_msg = 0;
  double solde;
  char date_op[31];

  int rc = sqlite3_open(DATABASE, &db);
  
  if (rc != SQLITE_OK) {
      
      fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
      sqlite3_close(db);
      
      return 1;
  }
    
  char *sql1 = "SELECT solde FROM account WHERE id = ?";

  rc = sqlite3_prepare_v2(db, sql1, -1, &stmt1, NULL);
  
  if (rc == SQLITE_OK) {
        
    sqlite3_bind_text(stmt1, 1, id_compte, -1, SQLITE_STATIC);
  } else {
    fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
  }

  int step1 = sqlite3_step(stmt1);
  
  if (step1 == SQLITE_ROW) {
      solde = sqlite3_column_double(stmt1, 0); 
  } 
  else{
      printf("No data found for the specified account.\n");
  }

  sqlite3_finalize(stmt1);

  char *sql2 = "SELECT date_operation FROM operation WHERE account_id = ? ORDER BY date_operation DESC LIMIT 1;";

  rc = sqlite3_prepare_v2(db, sql2, -1, &stmt2, NULL);
  
  if (rc == SQLITE_OK) {
        
    sqlite3_bind_text(stmt2, 1, id_compte, -1, SQLITE_STATIC);
  } else {
    fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
  }

  int step2 = sqlite3_step(stmt2);
  if (step2 == SQLITE_ROW) {
    const unsigned char *text = sqlite3_column_text(stmt2, 0); // Récupère la date sous forme de texte

    if (text != NULL) {
      // Copie en toute sécurité dans date_op (en supposant que la date n'excède pas 30 caractères)
      strncpy(date_op, (const char *)text, sizeof(date_op) - 1);
      date_op[sizeof(date_op) - 1] = '\0'; // Assure une terminaison nulle
    } 
    else{
      strcpy(date_op, "N/A"); // Valeur par défaut si la colonne est NULL
    }
  } 
  else{
    strcpy(date_op, "N/A"); // Valeur par défaut si aucune ligne n'est retournée
  }

  // Finalisation
  sqlite3_finalize(stmt2);

  // Message formaté
  snprintf(message, BUFFSIZE, "RES_SOLDE - Solde du compte %s : %.2f\nDate dernière opération : %s", id_compte, solde, date_op);

  // Fermeture de la base
  sqlite3_close(db);
}

int operation(char id_compte[], char message[]) {
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int rc = sqlite3_open(DATABASE, &db);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    // Requête SQL pour les 10 dernières opérations
    char *sql = "SELECT id, montant, date_operation, op FROM operation WHERE account_id = ? ORDER BY date_operation DESC LIMIT 10;";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    // Lier le paramètre id_compte
    sqlite3_bind_text(stmt, 1, id_compte, -1, SQLITE_STATIC);

    // Initialiser le message pour stocker les résultats
    snprintf(message, BUFFSIZE, "Les 10 dernières opérations pour le compte %s :\nID\tDate\t\t\tOpération\tMontant\n", id_compte);

    // Parcourir les résultats
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        double montant = sqlite3_column_double(stmt, 1);
        const unsigned char *date_op = sqlite3_column_text(stmt, 2);
        const unsigned char *op = sqlite3_column_text(stmt, 3);

        // Ajouter l'opération au message
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "%d\t%s\t%s\t\t%.2f\n", id, date_op, op,montant);
        strncat(message, buffer, BUFFSIZE - strlen(message) - 1);
    }

    // Finaliser la requête et fermer la base de données
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return 0;
}


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

        if (strcmp(command, "exit") == 0 || strcmp(command, "quit") == 0) {
          printf("Client disconnected\n");
          break;
        } 
        else if (strcmp(command, "help") == 0){
          strncpy(message, "\nAJOUT <id_client id_compte password somme>\tAjouter au solde\nRETRAIT <id_client id_compte password somme>\tFaire un retrait\nSOLDE <id_client id_compte password>\t\tAfficher le solde du compte\nOPERATIONS <id_client id_compte password>\tAffiche les 10 dernières opérations\n\nhelp\tAfficher l'aide\nexit\tQuitter\n", BUFFSIZE);
        }
        else if (strcmp(command, "AJOUT") == 0 ||strcmp(command, "ajout") == 0) {
          if (args==5){
            printf("AJOUT effectué par utilisateur %s pour le compte %s avec somme %.2f\n", id_client, id_compte, somme);
            if(check_args_validity(id_client,id_compte,password,message)==0){
              ajout(id_client,id_compte,somme,message);
            }
          }
          else{
            strncpy(message, "KO - Erreur de paramètre - AJOUT <id_client id_compte password somme>", BUFFSIZE);
          }
        } 
        else if (strcmp(command, "RETRAIT")==0||strcmp(command, "retrait") == 0) {
          if (args==5){
            printf("RETRAIT effectué par utilisateur %s pour le compte %s avec somme %.2f\n", id_client, id_compte, somme);
            if(check_args_validity(id_client,id_compte,password,message)==0){
              retrait(id_client,id_compte,somme,message);
            }
          }
          else{
            strncpy(message, "KO - Erreur de paramètre - RETRAIT <id_client id_compte password somme>", BUFFSIZE);
          }
        } 
        else if (strcmp(command, "SOLDE")==0||strcmp(command, "solde") == 0) {
          if (args == 4){
            printf("SOLDE demandé par l'utilisateur %s pour le compte %s\n", id_client, id_compte);
            if(check_args_validity(id_client,id_compte,password,message)==0){
              solde(id_compte,message);
            }
          }
          else{
            strncpy(message, "KO - Erreur de paramètre - SOLDE <id_client id_compte password>", BUFFSIZE);
          }
        } 
        else if (strcmp(command, "OPERATIONS")==0||strcmp(command, "operations") == 0) {
          if(args == 4){
            printf("OPERATIONS demandé par utilisateur %s pour le compte %s\n", id_client, id_compte);
            if(check_args_validity(id_client,id_compte,password,message)==0){
              operation(id_compte,message);
            }
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