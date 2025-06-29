#include "TcpSocket.h"

#include <WinSock2.h>

namespace Halo
{
	Halo::TcpSocket::TcpSocket(const std::string& ip, const std::uint16_t port) noexcept
		: Socket(ip, port)
	{
	}

	Halo::TcpSocket::TcpSocket(const std::uint16_t port) noexcept
		: Socket(port)
	{
	}

	std::int32_t TcpSocket::Listen(std::int32_t backlog)
	{
		return listen(sockfd_, backlog);
	}

	// TODO: IPv6‘Î‰ž

	/// @brief 
	/// @param client_socket 
	/// @return 
	std::int32_t TcpSocket::Accept() const
	{
		::sockaddr_in addr{};
		std::int32_t addrlen = sizeof(addr);
		return accept(sockfd_, reinterpret_cast<sockaddr*>(&addr), &addrlen);
	}
}


