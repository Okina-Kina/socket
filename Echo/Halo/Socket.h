#pragma once

#include <cstdint>
#include <array>
#include <string>

#include "ITransmitter.h"

namespace Halo
{
	/// @brief 
	class Socket : public ITransmitter
	{
	public:
		explicit Socket(const std::string& ip, const std::uint16_t port) noexcept;
		explicit Socket(const std::uint16_t port) noexcept;
		Socket() = delete;

	public:
		[[nodiscard]] std::int32_t Open(const std::int32_t address_family, const std::int32_t socktype, const std::int32_t protocol);
		[[nodiscard]] std::int32_t Close();
		[[nodiscard]] std::int32_t Shutdown(const std::int32_t how);
		[[nodiscard]] std::int32_t Bind();
		[[nodiscard]] std::int32_t Connect(const std::string& ip, const std::uint16_t port);

	public:
		// sendto, recvfrom
		[[nodiscard]] std::int32_t Send(const void* data, std::int32_t size) const override final;
		[[nodiscard]] std::int32_t Recv(void* data, std::int32_t size) const override final;

	protected:
		std::int32_t sockfd_;
		std::int32_t addressfamily_;
		std::int32_t protocol_;
		std::int32_t socktype_;
		std::uint16_t port_;
		std::string ip_;
		bool is_blocking_;
	};
}