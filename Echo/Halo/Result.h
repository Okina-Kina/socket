#pragma once

#include <variant>
#include <stdexcept>
#include <utility>

namespace Halo {
	template<typename Val, typename Err>
	class Result
	{
	public:
		explicit Result(const Val& value) : value_(value) {}
		explicit Result(Val&& value) : value_(std::move(value)) {}
		explicit Result(const Err& error) : value_(error) {}
		explicit Result(Err&& error) : value_(std::move(error)) {}

		explicit Result(const Result&) = default;
		explicit Result(Result&&) = default;
		Result& operator=(const Result&) = default;
		Result& operator=(Result&&) = default;


	public:
		bool HasValue() const noexcept
		{
			return std::holds_alternative<Val>(value_);
		}

		explicit operator bool() const noexcept
		{
			return HasValue();
		}

	public:
		const	Val& Value() const noexcept
		{
			if (!HasValue()) throw std::logic_error("bad excpected access: no value");
			return std::get<Val>(value_);
		}

		const Err& Error() noexcept
		{
			if (HasValue()) throw std::logic_error("bad excpected access: has value");
			return std::get<Error>(value_);
		}

		const Err&& Error() const noexcept
		{
			if (HasValue()) throw std::logic_error("bad excpected access: has value");
			return std::get<Error>(std::move(value_));
		}

	public:
		template<typename SuccessFn, typename ErrorFn>
		void Apply(SuccessFn&& on_succes, ErrorFn&& on_error) const
		{
			if (HasValue())
			{
				on_succes(Value());
			}
			else
			{
				on_error(Error());
			}
		}

	private:
		std::variant<Val, Err> value_;
	};
}