/*
	server.cpp
*/

#include <iostream>
#include <algorithm>
#include <array>

#include <winsock2.h>
#include <WS2tcpip.h>
//Winsockを使用するアプリケーションはWs2_32.lib ライブラリ ファイルとリンクする必要がある。 
//#pragma コメントは、 Ws2_32.lib ファイルが必要であることをリンカーに示す。
#pragma comment(lib,"ws2_32.lib")

// socket
int main() {
	const std::uint32_t BUFFER_SIZE = 512;

	WSADATA wsaData;
	SOCKET sock;
	struct sockaddr_in addr;

	struct timeval t_val = { 0, 1000 };
	int select_ret;

	char buf[BUFFER_SIZE];
	fd_set fds, readfds;
	std::array<int, FD_SETSIZE> acceptList;
	int accept_num = 0;
	int i;

	// accept_list 初期化
	std::fill(acceptList.begin(), acceptList.end(), INVALID_SOCKET);

	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(30000);
	inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr.S_un.S_addr);

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		return 1;
	}

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET ||
		bind(sock, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR ||
		listen(sock, FD_SETSIZE) == SOCKET_ERROR)
	{
		std::cout << "socket error >> " << WSAGetLastError() << std::endl;
		return 1;
	}
	FD_ZERO(&readfds);
	FD_SET(sock, &readfds);


	while (true)
	{
		memcpy(&fds, &readfds, sizeof(fd_set));
		select_ret = select(0, &fds, NULL, NULL, &t_val);

		// timeoutでない場合
		if (select_ret != 0)
		{
			// 待ちうけソケットにデータがある
			// 通信時にはacceptしたsocketを使うはずなので、待ちうけに来るものは必ずaccept対象
			if (FD_ISSET(sock, &fds))
			{
				struct sockaddr_in client;
				int len = sizeof(client);
				int client_sock = accept(sock, (struct sockaddr*)&client, &len);
				if (client_sock != INVALID_SOCKET) {
					// 空いているところから登録
					int i = 0;
					while (i < FD_SETSIZE && acceptList.at(i) != INVALID_SOCKET) i++;
					if (i != FD_SETSIZE)
					{
						FD_SET(client_sock, &readfds);
						acceptList.at(i) = client_sock;
						std::cout << "accept" << std::endl;
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

			// 各ソケットの状況チェック
			for (i = 0; i < FD_SETSIZE; i++)
			{
				if (acceptList.at(i) != -1 && FD_ISSET(acceptList.at(i), &fds))
				{
					int recv_ret;
					memset(buf, 0, BUFFER_SIZE);

					recv_ret = recv(acceptList.at(i), buf, BUFFER_SIZE, 0);
					if (0 < recv_ret)
					{
						// 受信データ処理
						buf[BUFFER_SIZE - 1] = '\0';
						std::cout << "recv[" << i << "][" << acceptList.at(i) << "]: " << buf << " [status: " << recv_ret << "]" << std::endl;
					}
					else
					{
						// 通信異常（相手からいきなり切断されると常にここが呼び出されたので。）
						std::cout << "disconnect?[" << i << "][" << acceptList.at(i) << "]" << std::endl;
						closesocket(acceptList.at(i));
						acceptList.at(i) = -1;
					}
				}
			}
		}
	}
	closesocket(sock);

	WSACleanup();

	return 0;
}