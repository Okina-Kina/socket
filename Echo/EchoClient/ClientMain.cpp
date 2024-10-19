#include <iostream>
#include <cstring>
#include <string>
#include <winsock2.h>
#include <string.h>
#include <WS2tcpip.h>

#define BUFFER 512

int main() {
	WSADATA wsaData;

	SOCKET sock;
	struct sockaddr_in addr;
	struct timeval t_val = { 0, 1000 };
	fd_set fds, readfds;
	int select_ret;
	char buf[BUFFER];

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) <= 0)
	{
		exit(1);
	}


	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(30000);
	inet_pton(AF_INET, "127.0.0.1", &addr);

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET ||
		connect(sock, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		std::cout << "socket error" << std::endl;
		return 1;
	}

	FD_ZERO(&readfds);
	FD_SET(sock, &readfds);

	while (1) {
		memcpy(&fds, &readfds, sizeof(fd_set));
		select_ret = select(0, &fds, NULL, NULL, &t_val);

		if (select_ret != 0) {
			// ソケットにデータがある
			if (FD_ISSET(sock, &fds)) {
				// 受信データ処理
				memset(buf, 0, BUFFER);
				recv(sock, buf, BUFFER, 0);
				buf[BUFFER - 1] = '\0';
				printf("%s\n", buf);
			}
		}
		else {

			std::string line;
			std::getline(std::cin, line);

			// データ送信
			std::strcpy(buf, line.c_str());
			sendto(sock, buf, strlen(buf), 0, (struct sockaddr*)&addr, sizeof(addr));

		}
	}

	closesocket(sock);

	WSACleanup();

	return 0;
}