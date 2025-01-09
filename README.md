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
## Fonctionnement 

Executer les fichiers dans des terminaux difféerent

Pour tcp :

tcp_client
tcp_server

Pour udp :

udp_client
udp_server

tcp utilise le port 9998 et udp le port 9999