#ifndef MCL_ID_H
#define MCL_ID_H

namespace mcl
{
class ID
{
public:
	ID() = default; // Invalid ID

	ID(int value)
	: m_value(value)
	{
	}

	/* generate
	Generates a new unique ID. If a value passed in is valid, it generates an ID
	with that value. Useful when loading things from the model that already have
	their own ID. */

	static ID generate(int value = 0)
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

	bool operator==(int value) const
	{
		return m_value == value;
	}

	bool operator!=(int value) const
	{
		return m_value != value;
	}

	operator int() const // convertible to int
	{
		return m_value;
	}

	bool isValid()
	{
		return m_value > 0;
	}

	int get() const
	{
		return m_value;
	}

private:
	int               m_value = {0};
	inline static int m_gen   = {0};
};
} // namespace mcl

#endif
