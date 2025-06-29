#pragma once

#include <string>

#include "ITransmitter.h"

namespace Halo
{
	/// @brief 
	class IUdpTransmitter : public ITransmitter
	{
	public:
		virtual ~IUdpTransmitter() = default;

	public:
		[[nodiscard]] virtual std::int32_t SendTo(const void* data, std::int32_t size, const std::string& ip, std::uint16_t port) const = 0;
		[[nodiscard]] virtual std::int32_t RecvFrom(void* data, std::int32_t size, std::string& ip, std::uint16_t& port) const = 0;
	};
}