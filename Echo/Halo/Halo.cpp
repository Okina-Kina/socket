#include <iostream>
#include <thread>
#include <WinSock2.h>

#include "TcpSocket.h"

#pragma comment(lib, "ws2_32.lib")

int main()
{
	WSADATA wsa_data;

	if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0)
	{
		std::cout << "Failed to startup WSA >> " << WSAGetLastError() << std::endl;
		return 1;
	}

	Halo::TcpSocket host{ "127.0.0.1", 10000 };
	Halo::TcpSocket guest{ 10031 };

	std::thread host_task{
		[&host]() {
			// Host
			decltype(auto) result = host.Open(AF_INET, SOCK_STREAM, 0);
			if (result != INVALID_SOCKET) {
				std::cout << "Host socket opened successfully." << std::endl;
			}
			else {
				std::cerr << "Failed to open host socket >> " << WSAGetLastError() << std::endl;
			}

			result = host.Bind();
			if (result != SOCKET_ERROR) {
				std::cout << "Host socket bind successfully." << std::endl;
			}
			else {
				std::cerr << "Failed to bind host socket >> " << WSAGetLastError() << std::endl;
			}

			while (true)
			{
				if (host.Listen(5) == 0)
				{
					std::cout << "Host socket is now listening." << std::endl;
					break;
				}
				else
				{
					std::cerr << "Listen failed >> " << WSAGetLastError() << std::endl;
					std::this_thread::sleep_for(std::chrono::seconds(1));
				}
			}

			result = host.Accept();
			if (result != INVALID_SOCKET) {
				std::cout << "Host socket accepted a connection successfully." << std::endl;
			}
			else {
				std::cerr << "Failed to accept connection on host socket >> " << WSAGetLastError() << std::endl;
			}

			while (true)
			{
				char send_buffer[1024] = { 0 };
				//if (host.Send(send_buffer, sizeof(send_buffer))) {
				//	std::cout << "Host socket sent data successfully." << std::endl;
				//}
				//else {
				//	std::cerr << "Failed to send data on host socket >> " << WSAGetLastError() << std::endl;
				//}

				char recv_buffer[1024] = { 0 };
				int ret = host.Recv(recv_buffer, sizeof(recv_buffer));
				if (ret > 0) {
					std::cout << "Host socket received data successfully." << std::endl;
				}
				else if (ret == 0) {
					std::cout << "Host socket: peer disconnected." << std::endl;
					break;  // ループ抜ける処理など
				}
				else {
					std::cerr << "Failed to receive data on host socket >> " << WSAGetLastError() << std::endl;
				}

				std::this_thread::sleep_for(std::chrono::seconds(1));
			}

			result = host.Shutdown(SD_BOTH);
			if (result != SOCKET_ERROR) {
				std::cout << "Host socket shutdown successfully." << std::endl;
			}
			else {
				std::cerr << "Failed to shutdown host socket >> " << WSAGetLastError() << std::endl;
			}

			result = host.Close();
			if (result != SOCKET_ERROR) {
				std::cout << "Host socket closed successfully." << std::endl;
			}
			else {
				std::cerr << "Failed to close host socket >> " << WSAGetLastError() << std::endl;
			}
		}
	};

	std::thread guest_task{
		[&guest]() {
			// Guest
			 decltype(auto)	result = guest.Open(AF_INET, SOCK_STREAM, 0);
			if (result != INVALID_SOCKET) {
				std::cout << "Guest socket opened successfully." << std::endl;
			}
			else {
				std::cerr << "Failed to open guest socket >> " << WSAGetLastError() << std::endl;
			}

			result = guest.Connect("127.0.0.1", 10000);
			if (result != SOCKET_ERROR) {
				std::cout << "Guest socket connected successfully." << std::endl;
			}
			else {
				std::cerr << "Failed to connect guest socket >> " << WSAGetLastError() << std::endl;
			}

			while (true)
			{
				std::string send_buffer = "hello";
				if (guest.Send(send_buffer.c_str(), send_buffer.size()) > 0) {
					std::cout << "Guest socket sent data successfully." << std::endl;
				}
				else {
					std::cerr << "Failed to send data on guest socket >> " << WSAGetLastError() << std::endl;
				}

				//char recv_buffer[1024] = { 0 };
				//if (guest.Recv(recv_buffer, sizeof(recv_buffer)) > 0) {
				//	std::cout << "Guest socket received data successfully." << std::endl;
				//}
				//else {
				//	std::cerr << "Failed to receive data on guest socket >> " << WSAGetLastError() << std::endl;
				//}

				std::this_thread::sleep_for(std::chrono::seconds(1));
			}


			result = guest.Shutdown(SD_BOTH);
			if (result != SOCKET_ERROR) {
				std::cout << "Guest socket shutdown successfully." << std::endl;
			}
			else {
				std::cerr << "Failed to shutdown guest socket >> " << WSAGetLastError() << std::endl;
			}

			result = guest.Close();
			if (result != SOCKET_ERROR) {
				std::cout << "Guest socket closed successfully." << std::endl;
			}
			else {
				std::cerr << "Failed to close guest socket >> " << WSAGetLastError() << std::endl;
			}
		}
	};

	host_task.join();
	guest_task.join();


	WSACleanup();
}
