#include "Socket.h"

#include <iostream>
#include <WS2tcpip.h>
#include <WinSock2.h>

namespace Halo
{
	///-------------------------------------------------------------------------
	/// Socket
	///-------------------------------------------------------------------------
	Socket::Socket(const std::string& ip, const std::uint16_t port) noexcept
		: sockfd_(-1), addressfamily_(AF_INET), protocol_(0), socktype_(SOCK_STREAM), port_(port), ip_(std::move(ip)), is_blocking_(true)
	{
	}

	Socket::Socket(const std::uint16_t port) noexcept
		: sockfd_(-1), addressfamily_(AF_INET), protocol_(0), socktype_(SOCK_STREAM), port_(port), is_blocking_(true)
	{
	}

	///-------------------------------------------------------------------------
	/// Public
	///-------------------------------------------------------------------------
	std::int32_t Socket::Open(const std::int32_t address_family, const std::int32_t socktype, const std::int32_t protocol)
	{
		addressfamily_ = address_family;
		socktype_ = socktype;
		protocol_ = protocol;

		sockfd_ = socket(address_family, socktype, protocol);

		return sockfd_;
	}

	std::int32_t Socket::Shutdown(const std::int32_t how)
	{
		return shutdown(sockfd_, how);
	}

	std::int32_t Socket::Close()
	{
		return closesocket(sockfd_);
	}

	// TODO: IPv6‘Î‰ž
	std::int32_t Socket::Connect(const std::string& ip, const std::uint16_t port)
	{
		sockaddr_in addr{};

		addr.sin_family = addressfamily_;
		addr.sin_port = htons(port);
		inet_pton(addressfamily_, ip.c_str(), &addr.sin_addr);

		socklen_t addrlen = sizeof(sockaddr);

		return connect(sockfd_, reinterpret_cast<sockaddr*>(&addr), addrlen);
	}

	// TODO: IPv6‘Î‰ž
	std::int32_t Socket::Bind()
	{
		sockaddr_in addr{};
		addr.sin_family = addressfamily_;
		addr.sin_port = htons(port_);
		inet_pton(addressfamily_, ip_.c_str(), &addr.sin_addr);

		socklen_t addrlen = sizeof(sockaddr);
		return bind(sockfd_, reinterpret_cast<sockaddr*>(&addr), addrlen);
	}
}