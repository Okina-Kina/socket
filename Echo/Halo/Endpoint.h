#pragma once

#include <cstdint>
#include <string>

namespace MyNamespace
{

	class Endpoint
	{
	public:
		explicit Endpoint(const std::int32_t sockfd, const std::string& ip, const std::uint16_t port) noexcept;

	private:
		std::int32_t sockfd_;
		std::string ip_;
		std::uint16_t port_;
	};
}