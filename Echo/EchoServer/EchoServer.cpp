#include <iostream>
#include <windows.h>
#include <winsock2.h>
#include <WS2tcpip.h>

// socket
#define FD_SETSIZE 32
#define BUFFER 512


int main() {
	WSADATA wsaData;
	SOCKET sock;
	struct sockaddr_in addr;
	struct timeval t_val = { 0, 1000 };
	int select_ret;
	char buf[BUFFER];
	fd_set fds, readfds;
	int accept_list[FD_SETSIZE];
	int accept_num = 0;
	int i;

	// accept_list 初期化
	for (i = 0; i < FD_SETSIZE; i++) {
		accept_list[i] = INVALID_SOCKET;
	}

	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(30000);
	addr.sin_addr.S_un.S_addr = INADDR_ANY;

	WSAStartup(MAKEWORD(2, 0), &wsaData);

	if (
		(sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET ||
		bind(sock, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR ||
		listen(sock, FD_SETSIZE) == SOCKET_ERROR
		) {
		//printf("socket error\n");
		return 1;
	}
	FD_ZERO(&readfds);
	FD_SET(sock, &readfds);

	while (1) {
		memcpy(&fds, &readfds, sizeof(fd_set));
		select_ret = select(0, &fds, NULL, NULL, &t_val);
		// timeoutでない場合
		if (select_ret != 0) {
			// 待ちうけソケットにデータがある
			// 通信時にはacceptしたsocketを使うはずなので、待ちうけに来るものは必ずaccept対象
			if (FD_ISSET(sock, &fds)) {
				struct sockaddr_in client;
				int len = sizeof(client);
				int client_sock = accept(sock, (struct sockaddr*)&client, &len);
				if (client_sock != INVALID_SOCKET) {
					// 空いているところから登録
					int i = 0;
					while (i < FD_SETSIZE && accept_list[i] != INVALID_SOCKET) i++;
					if (i != FD_SETSIZE) {
						FD_SET(client_sock, &readfds);
						accept_list[i] = client_sock;
						printf("accept\n");
					}
					else {
						printf("空きがありません\n");
					}
				}
				else {
					printf("accept error\n");
				}
			}

			// 各ソケットの状況チェック
			for (i = 0; i < FD_SETSIZE; i++) {
				if (accept_list[i] != -1 && FD_ISSET(accept_list[i], &fds)) {
					int x;
					memset(buf, 0, BUFFER);
					x = recv(accept_list[i], buf, BUFFER, 0);
					if (x != 0) {
						// 受信データ処理
						buf[BUFFER - 1] = '\0';
						//printf("recv[%d][%d]: %s [status: %d]\n", i, accept_list[i], buf, x);
					}
					else {
						// 通信異常（相手からいきなり切断されると常にここが呼び出されたので。）
						//printf("disconnect?[%d][%d]\n", i, accept_list[i]);
						closesocket(accept_list[i]);
						accept_list[i] = -1;
					}
				}
			}
		}
	}
	closesocket(sock);

	WSACleanup();

	return 0;
}