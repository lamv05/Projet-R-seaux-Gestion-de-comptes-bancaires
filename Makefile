CC = gcc
CFLAGS = -std=c99
LDFLAGS = -lsqlite3
TCP_CLIENT_SRC = tcp_src/client.c
TCP_SERVER_SRC = tcp_src/server.c
UDP_CLIENT_SRC = udp_src/client.c
UDP_SERVER_SRC = udp_src/server.c
TCP_CLIENT_BIN = tcp_client
TCP_SERVER_BIN = tcp_server
UDP_CLIENT_BIN = udp_client
UDP_SERVER_BIN = udp_server

all: tcp udp

tcp: $(TCP_CLIENT_BIN) $(TCP_SERVER_BIN)

udp: $(UDP_CLIENT_BIN) $(UDP_SERVER_BIN)

$(TCP_CLIENT_BIN): $(TCP_CLIENT_SRC)
	$(CC) $(CFLAGS) $(TCP_CLIENT_SRC) -o $@ $(LDFLAGS)

$(TCP_SERVER_BIN): $(TCP_SERVER_SRC)
	$(CC) $(CFLAGS) $(TCP_SERVER_SRC) -o $@ $(LDFLAGS)

$(UDP_CLIENT_BIN): $(UDP_CLIENT_SRC)
	$(CC) $(CFLAGS) $(UDP_CLIENT_SRC) -o $@ $(LDFLAGS)

$(UDP_SERVER_BIN): $(UDP_SERVER_SRC)
	$(CC) $(CFLAGS) $(UDP_SERVER_SRC) -o $@ $(LDFLAGS)

clean:
	rm -f $(TCP_CLIENT_BIN) $(TCP_SERVER_BIN) $(UDP_CLIENT_BIN) $(UDP_SERVER_BIN)

