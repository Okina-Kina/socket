/*
	Client.cpp
*/

#include <iostream>
#include <string>
#include <winsock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")


int main() 
{
	const std::uint32_t BUFFER_SIZE = 512;

	WSADATA wsaData;

	SOCKET sockfd;
	struct sockaddr_in addr;
	struct timeval timeout = { 0, 1000 };
	fd_set fds, readfds;
	int select_ret;
	char buf[BUFFER_SIZE];

	//------------------------------------------------------
	//
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		std::cout << "Failed to startup WSA >> " << WSAGetLastError() << std::endl;
		return 1;
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	ZeroMemory(&addr, sizeof(addr));

	addr.sin_family = AF_INET;
	addr.sin_port = htons(30000);

	inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr.S_un.S_addr);


	//------------------------------------------------------
	//
	if (sockfd == INVALID_SOCKET ||
		connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		std::cout << "socket error >>" << WSAGetLastError() << std::endl;
		return 1;
	}
	FD_ZERO(&readfds);
	FD_SET(sockfd, &readfds);

	std::cout << "connect succeeded! " << std::endl;


	//------------------------------------------------------
	//
	while (1)
	{
		std::memcpy(&fds, &readfds, sizeof(fd_set));
		select_ret = select(0, &fds, NULL, NULL, &timeout);

		if (select_ret != 0)
		{
			// ソケットにデータがある
			if (FD_ISSET(sockfd, &fds))
			{
				// 受信データ処理
				std::memset(buf, 0, BUFFER_SIZE);
				recv(sockfd, buf, BUFFER_SIZE, 0);
				buf[BUFFER_SIZE - 1] = '\0';
				std::cout << buf << std::endl;
			}
		}
		else
		{
			std::cout << "[input your message] " << std::endl;

			std::string msg;
			std::getline(std::cin, msg);

			std::copy(msg.begin(), msg.end(), buf);
			buf[msg.size()] = '\0';

			// データ送信
			sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr*)&addr, sizeof(addr));
		}
	}

	shutdown(sockfd, SD_SEND);
	closesocket(sockfd);

	WSACleanup();

	return 0;
}