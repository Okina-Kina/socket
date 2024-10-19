#include <iostream>
#include <winsock2.h>
#include <string>
#include <string.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")


int main() {
	const std::uint32_t BUFFER_SIZE = 512;

	WSADATA wsaData;

	SOCKET sock;
	struct sockaddr_in addr;
	struct timeval t_val = { 0, 1000 };
	fd_set fds, readfds;
	int select_ret;
	char buf[BUFFER_SIZE];

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		return 1;
	}

	sock = socket(AF_INET, SOCK_STREAM, 0);
	ZeroMemory(&addr, sizeof(addr));
	//memset(&addr, 0, sizeof(struct sockaddr_in));

	addr.sin_family = AF_INET;
	addr.sin_port = htons(30000);

	inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr.S_un.S_addr);


	if (sock == INVALID_SOCKET ||
		connect(sock, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		std::cout << "socket error >>" << WSAGetLastError() << std::endl;
		return 1;
	}


	FD_ZERO(&readfds);
	FD_SET(sock, &readfds);

	while (1)
	{
		memcpy(&fds, &readfds, sizeof(fd_set));
		select_ret = select(0, &fds, NULL, NULL, &t_val);

		if (select_ret != 0)
		{
			// ソケットにデータがある
			if (FD_ISSET(sock, &fds))
			{
				// 受信データ処理
				memset(buf, 0, BUFFER_SIZE);
				recv(sock, buf, BUFFER_SIZE, 0);
				buf[BUFFER_SIZE - 1] = '\0';
				std::cout << buf << std::endl;
			}
		}
		else
		{
			std::cout << "input message: " << std::endl;

			std::string msg;
			std::getline(std::cin, msg);

			std::copy(msg.begin(), msg.end(), buf);
			buf[msg.size()] = '\0';

			// データ送信
			sendto(sock, buf, strlen(buf), 0, (struct sockaddr*)&addr, sizeof(addr));
		}
	}

	closesocket(sock);

	WSACleanup();

	return 0;
}