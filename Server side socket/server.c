#include <stdio.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 8081
#define BUFFER_SIZE 1024


int main() {
	WSADATA wsa;
	SOCKET server_socket, client_socket;
	struct sockaddr_in server_addr, client_addr;
	char buffer[BUFFER_SIZE];
	int client_addr_length = sizeof(client_addr);
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		printf("error occured while starting winsocked");
		return 1;
	};

	// IPv4 : 127.0.0.1 octet 32-bit 0-255-8bit, IPv6,1.1.1.1.1.1 6 octet 128-bit 16-bit, 
	server_socket = socket(AF_INET, SOCK_STREAM, 0);

	if (server_socket == INVALID_SOCKET) {
		printf("error %d", WSAGetLastError());
		WSACleanup();
		return 1;
	};

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {

		printf("error %d", WSAGetLastError());
		closesocket(server_socket);
		WSACleanup();
		return 1;
	};

	listen(server_socket, 3);
	printf("Server started listening: \n");

	client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_length);

	if (client_socket == INVALID_SOCKET) {
		printf("error %d \n", WSAGetLastError());
		closesocket(server_socket);
		WSACleanup();
		return 1;
	};

	char* header = "HTTP/1.1 200 OK\r\n"
		"Content-Type: text/html\r\n"
		"\r\n";

	FILE* html_file;

	errno_t err = fopen_s(&html_file, "index.html", "r");

	if (err != 0) {
		printf("Error occured while opening the file");
		closesocket(client_socket);
		closesocket(server_socket);
		WSACleanup();
		return 1;

	};

	send(client_socket, header, strlen(header), 0);

	while (fgets(buffer, BUFFER_SIZE, html_file) != NULL) {
		send(client_socket, buffer, strlen(buffer), 0);

	}

	shutdown(client_socket, SD_SEND);
	fclose(html_file);

	recv(client_socket, buffer, BUFFER_SIZE, 0);

	printf("client's response: %s", buffer);

	closesocket(client_socket);

	closesocket(server_socket);
	WSACleanup();

	return 0;

}