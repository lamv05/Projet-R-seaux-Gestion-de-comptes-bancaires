DROP TABLE IF EXISTS account;
DROP TABLE IF EXISTS client;
DROP TABLE IF EXISTS operation;


CREATE TABLE client(
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    nom TEXT NOT NULL,
    prenom TEXT NOT NULL,
    cli_password TEXT NOT NULL,
    adresse TEXT,
    phone_num TEXT
);

CREATE TABLE account(
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    solde FLOAT NOT NULL,
    client_id INTEGER,
    FOREIGN KEY (client_id) REFERENCES client(id)
);

CREATE TABLE operation(
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    montant FLOAT NOT NULL,
    date_operation TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    op TEXT NOT NULL,
    account_id INTEGER,
    client_id INTEGER,
    FOREIGN KEY (account_id) REFERENCES account(id),
    FOREIGN KEY (client_id) REFERENCES client(id)
);

INSERT INTO client (nom,prenom,cli_password) VALUES (
    "Jean",
    "Jacques",
    "123"
);

INSERT INTO client (nom,prenom,cli_password) VALUES (
    "Bertrand",
    "Bernard",
    "456"
);

INSERT INTO client(nom,prenom,cli_password) VALUES (
    "Jean",
    "Marie",
    "789"
);

INSERT INTO account(solde,client_id) VALUES (
    100,
    1
);

INSERT INTO account(solde,client_id) VALUES (
    200,
    2
);

INSERT INTO account(solde,client_id) VALUES (
    500,
    3
);