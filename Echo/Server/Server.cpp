/*
	Server.cpp
*/

#include <iostream>
#include <algorithm>
#include <array>
#include <string>
#include <winsock2.h>
#include <WS2tcpip.h>

#pragma comment(lib,"ws2_32.lib")

int main()
{
	const std::uint32_t BUFFER_SIZE = 512;
	const std::uint32_t PORT_NO = 30000;
	const std::string IP_ADDRESS = "127.0.0.1";

	WSADATA wsaData;

	std::uint16_t sockfd;
	struct sockaddr_in sockAddr;

	struct timeval timeout = { 0, 1000 };
	int select_ret;

	char buf[BUFFER_SIZE];
	fd_set fds, readfds;
	std::array<int, FD_SETSIZE> acceptList;
	int accept_num = 0;

	//------------------------------------------------------
	// acceptList 初期化
	std::fill(acceptList.begin(), acceptList.end(), INVALID_SOCKET);

	std::memset(&sockAddr, 0, sizeof(struct sockaddr_in));
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(PORT_NO);
	inet_pton(AF_INET, IP_ADDRESS.c_str(), &sockAddr.sin_addr.S_un.S_addr);

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		std::cout << "Failed to startup WSA >> " << WSAGetLastError() << std::endl;
		return 1;
	}


	//------------------------------------------------------
	//
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET ||
		bind(sockfd, (struct sockaddr*)&sockAddr, sizeof(sockAddr)) == SOCKET_ERROR ||
		listen(sockfd, FD_SETSIZE) == SOCKET_ERROR)
	{
		std::cout << "socket error >> " << WSAGetLastError() << std::endl;
		return 1;
	}
	//FD_ZERO(&readfds);
	//FD_SET(sockfd, &readfds);


	//------------------------------------------------------
	//
	std::cout << "[Info] " << std::endl;
	std::cout << "Port No >> " << std::to_string(PORT_NO) << std::endl;
	std::cout << "IP Address >> " << IP_ADDRESS << std::endl;

	std::cout << std::endl << "Start Listening... " << std::endl;

	FD_ZERO(&readfds);
	FD_SET(sockfd, &readfds);

	while (true)
	{
		std::memcpy(&fds, &readfds, sizeof(fd_set));
		select_ret = select(FD_SETSIZE, &fds, NULL, NULL, &timeout);

		//std::cout << "ret select >> " << select_ret << std::endl;

		// timeoutでない場合
		if (select_ret != 0)
		{
			// 待ちうけソケットにデータがある
			// 通信時にはacceptしたsocketを使うはずなので、待ちうけに来るものは必ずaccept対象
			if (FD_ISSET(sockfd, &fds))
			{
				struct sockaddr_in client;
				int len = sizeof(client);
				int client_sockfd = accept(sockfd, (struct sockaddr*)&client, &len);
				if (client_sockfd != INVALID_SOCKET) {
					// 空いているところから登録
					int i = 0;
					while (i < FD_SETSIZE && acceptList.at(i) != INVALID_SOCKET) i++;
					if (i != FD_SETSIZE)
					{
						FD_SET(client_sockfd, &readfds);
						acceptList.at(i) = client_sockfd;
						std::cout << "accept >> " << std::to_string(client_sockfd) << std::endl;
					}
					else
					{
						std::cout << "空きがありません" << std::endl;
					}
				}
				else
				{
					std::cout << "accept error" << std::endl;
				}
			}
			else
			{
				std::cout << "Err >>" << WSAGetLastError() << std::endl;
			}

			// 各ソケットの状況チェック
			for (int i = 0; i < FD_SETSIZE; ++i)
			{
				if (acceptList.at(i) != -1 && FD_ISSET(acceptList.at(i), &fds))
				{
					int recv_ret;
					std::memset(buf, 0, BUFFER_SIZE);

					recv_ret = recv(acceptList.at(i), buf, BUFFER_SIZE, 0);
					if (recv_ret != SOCKET_ERROR)
					{
						// 受信データ処理
						buf[BUFFER_SIZE - 1] = '\0';
						std::cout << "recv[" << i << "][" << acceptList.at(i) << "]: " << buf << " [status: " << recv_ret << "]" << std::endl;
					}
					else
					{
						// 通信異常（相手からいきなり切断されると常にここが呼び出されたので。）
						std::cout << "disconnect?[" << i << "][" << acceptList.at(i) << "]" << std::endl;
						std::cout << WSAGetLastError() << std::endl;

						shutdown(acceptList.at(i), SD_SEND);
						closesocket(acceptList.at(i));

						FD_CLR(acceptList.at(i), &fds);

						acceptList.at(i) = -1;

					}
				}
			}

		}
	}

	shutdown(sockfd, SD_BOTH);
	closesocket(sockfd);

	WSACleanup();

	return 0;
}