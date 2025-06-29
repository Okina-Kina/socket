#pragma once

#include <cstdint>

class ITransmitter
{
public:
	virtual ~ITransmitter() = default;

public:
	[[nodiscard]] virtual std::int32_t Send(const void* data, std::int32_t size) const = 0;
	[[nodiscard]] virtual std::int32_t Recv(void* data, std::int32_t size) const = 0;
};

