#ifndef MCL_ID_H
#define MCL_ID_H

#include <cstdint>

namespace mcl
{
class ID
{
public:
	ID() = default; // Invalid ID

	ID(uint64_t value)
	: m_value(value)
	{
	}

	/* generate
	Generates a new unique ID. If a value passed in is valid, it generates an ID
	with that value. Useful when loading things from the model that already have
	their own ID. */

	static ID generate(uint64_t value = 0)
	{
		if (value > m_gen)
		{
			m_gen = value;
			return {m_gen};
		}
		return {++m_gen};
	}

	/* reset
	Resets the internal generator to initial value. */

	static ID reset()
	{
		return m_gen = 0;
	}

	bool operator==(const ID& other) const
	{
		return m_value == other.m_value;
	}

	bool operator!=(const ID& other) const
	{
		return m_value != other.m_value;
	}

	bool operator==(uint64_t value) const
	{
		return m_value == value;
	}

	bool operator!=(uint64_t value) const
	{
		return m_value != value;
	}

	operator uint64_t() const // convertible to uint64_t
	{
		return m_value;
	}

	bool isValid()
	{
		return m_value > 0;
	}

	uint64_t get() const
	{
		return m_value;
	}

private:
	uint64_t               m_value = {0};
	inline static uint64_t m_gen   = {0};
};
} // namespace mcl

#endif
