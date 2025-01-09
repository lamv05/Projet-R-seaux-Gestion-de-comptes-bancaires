# Projet-Reseaux-Gestion-de-comptes-bancaires

## Compilation 

Dépendance : libsqlite3-dev

Pour compiler
```
make all
```

## Créer la base de donnée

Executer le fichier bank.sql avec sqlite3 par exemple

```
sqlite3
```
```
.read bank.sql
```

## Contenu base de donné

Table client

id  nom       prenom   cli_password  adresse  phone_num
--  --------  -------  ------------  -------  ---------
1   Jean      Jacques  123                             
2   Bertrand  Bernard  123                             
3   Jean      Marie    123

Table account

id  solde  client_id
--  -----  ---------
1   100.0  1        
2   200.0  2        
3   500.0  3   

## Fonctionnement 

Executer les fichiers dans des terminaux difféerent

Pour tcp :

tcp_client
tcp_server

Pour udp :

udp_client
udp_server

tcp utilise le port 9998 et udp le port 9999