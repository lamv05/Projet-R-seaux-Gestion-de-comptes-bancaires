all : tcp_client tcp_server


tcp_client : client.c
	gcc client.c -o client

tcp_server : server.c
	gcc server.c -o server

clear :
	rm -rf server client

.PHONY: kill-port

kill-port:
	@echo "Killing process on port 9999..."
	@kill -9 $$(lsof -t -i:9999) 2>/dev/null || echo "No process running on port 9999"


new : 
	make clear
	make all
	make kill-port
