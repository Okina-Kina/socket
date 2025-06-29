#pragma once

#include "Socket.h"

namespace Halo
{
	/// @brief 
	class TcpSocket : public Socket
	{
	public:
		explicit TcpSocket(const std::string& ip, const std::uint16_t port) noexcept;
		explicit TcpSocket(const std::uint16_t port) noexcept;

	public:
		[[nodiscard]] std::int32_t Listen(std::int32_t backlog);
		[[nodiscard]] std::int32_t Accept() const;
	};
}
