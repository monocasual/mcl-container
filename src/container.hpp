#ifndef MCL_CONTAINER_H
#define MCL_CONTAINER_H

#include "id.hpp"
#include <cassert>
#include <functional>
#include <iostream>
#include <utility>
#include <vector>

namespace mcl
{
// clang-format off
template <typename T>
concept HasId = requires(T t) {
	{ T::id } -> std::convertible_to<uint64_t>;
};

template <typename T>
concept HasIndex = requires(T t) {
	{ T::index } -> std::convertible_to<std::size_t>;
};
// clang-format on

/* -------------------------------------------------------------------------- */

struct Empty
{
};

/* -------------------------------------------------------------------------- */

template <typename T, bool Identifiable = false, bool Sortable = false>
class Container
{
public:
	using iterator       = typename std::vector<T>::iterator;
	using const_iterator = typename std::vector<T>::const_iterator;

	Container() = default;

	Container(ID id)
	    requires HasId<T>
	: id(id)
	{
	}

	Container(ID id, std::initializer_list<T> l)
	    requires HasId<T>
	: id(id)
	, m_items(l)
	{
	}

	Container(std::initializer_list<T> l)
	: m_items(l)
	{
	}

	/* Iterators */

	iterator       begin() { return m_items.begin(); }
	const_iterator begin() const { return m_items.begin(); }
	const_iterator cbegin() const { return m_items.cbegin(); }
	iterator       end() { return m_items.end(); }
	const_iterator end() const { return m_items.end(); }
	const_iterator cend() const { return m_items.cend(); }

	const int size() const
	{
		return m_items.size();
	}

	/* deepFindById (1)
	Finds an element with the given ID in the whole hierarchy, recursively.
	Returns nullptr if not found. */

	template <typename U>
	const U* deepFindById(ID id) const
	{
		const U* found = nullptr;
		deepFindById(id, found);
		return found;
	}

	/* deepFindById (2)
	Alternate version of deepFindById() (1) that returns a non-const pointer. */

	template <typename U>
	U* deepFindById(ID id)
	{
		return const_cast<U*>(std::as_const(*this).template deepFindById<U>(id));
	}

	/* deepFindById (3)
	Internal recursive function used by deepFindById (1). Do not call this directly. */

	template <typename U>
	bool deepFindById(ID id, const U*& found) const
	{
		for (const T& item : m_items)
		{
			if constexpr (HasId<T>)
			{
				if (item.id == id)
				{
					found = &item;
					return true;
				}
			}
			if (item.template deepFindById<U>(id, found))
				return true;
		}
		return false;
	}

	/* deepGetById (1)
	Returns a const reference of item with given ID, by looking through the whole
	hierarchy, recursively. Assumes that the item is present in the container
	(raises assertion otherwise). */

	template <typename U>
	const U& deepGetById(ID id) const
	{
		const U* item = deepFindById<U>(id);
		assert(item != nullptr);
		return *item;
	}

	/* deepGetById (2)
	Alternate version of deepGetById() (1) that returns a non-const reference. */

	template <typename U>
	U& deepGetById(ID id)
	{
		return const_cast<U&>(std::as_const(*this).template deepGetById<U>(id));
	}

	/* FindById (1)
	Finds an element with the given ID in the current container. Returns nullptr
	if not found. */

	const T* findById(ID id) const
	    requires HasId<T>
	{
		const auto it = findIf([id](const T& item)
		    { return item.id == id; });
		return it != m_items.end() ? &*it : nullptr;
	}

	/* FindById (2)
	Alternate version of FindById() (1) that returns a non-const pointer. */

	T* findById(ID id)
	    requires HasId<T>
	{
		return const_cast<T*>(std::as_const(*this).findById(id));
	}

	/* contains
	Returns whether the container contains item with the given id. */

	bool contains(ID id)
	    requires HasId<T>
	{
		return findById(id) != nullptr;
	}

	/* getById (1)
	Returns a const reference of item with given ID. Assumes that the item is
	present in the container (raises assertion otherwise). */

	const T& getById(ID id) const
	    requires HasId<T>
	{
		const T* item = findById(id);
		assert(item != nullptr);
		return *item;
	}

	/* getById (2)
	Alternate version of getById() (1) that returns a non-const reference. */

	T& getById(ID id)
	    requires HasId<T>
	{
		return const_cast<T&>(std::as_const(*this).getById(id));
	}

	/* getByIndex (1)
	Returns a const reference of item at index 'index'. */

	const T& getByIndex(std::size_t index) const
	{
		assert(index < m_items.size());
		if constexpr (HasIndex<T>)
			assert(static_cast<std::size_t>(m_items[index].index) == index); // Make sure indexing is right

		return m_items[index];
	}

	/* getByIndex (2)
	Alternate version of getByIndex() (1) that returns a non-const reference. */

	T& getByIndex(std::size_t index)
	{
		return const_cast<T&>(std::as_const(*this).getByIndex(index));
	}

	/* anyOf
	Returns true if any item satisfies the callback 'f'. */

	bool anyOf(std::function<bool(const T&)> f) const
	{
		return std::any_of(m_items.begin(), m_items.end(), f);
	}

	/* getIndex
	Return the index of the item with the given int. Assumes that the item is
	present in the container (raises assertion otherwise). */

	const int getIndex(ID id) const
	    requires HasId<T>
	{
		if constexpr (HasIndex<T>)
		{
			return getById(id).index;
		}
		else
		{
			for (int i = 0; const T& item : m_items)
			{
				if (item.id == id)
					return i;
				i++;
			}
			assert(false);
			return -1;
		}
	}

	/* getLast (1)
	Return a const reference to the last item in the container. Raises an
	assertion if the container is empty. */

	const T& getLast() const
	{
		assert(m_items.size() > 0);
		return m_items.back();
	}

	/* getLast (2)
	Alternate version of getLast() (1) that returns a non-const reference. */

	T& getLast() { return const_cast<T&>(std::as_const(*this).getLast()); }

	/* getIf
	Returns a vector of item pointers that satisfy the callback 'f'. */

	std::vector<T*> getIf(std::function<bool(const T&)> f)
	{
		std::vector<T*> out;
		for (T& item : m_items)
			if (f(item))
				out.push_back(&item);
		return out;
	}

	/* add
	Adds a new item to this container by moving it, and returns a reference. */

	T& add(T&& item)
	{
		if constexpr (HasId<T>)
			assert(isUniqueId(item.id));

		m_items.push_back(std::move(item));

		if constexpr (HasIndex<T>)
			rebuildIndexes();

		return m_items.back();
	}

	/* insert
	Inserts a new item to this container at the given index by moving it, and
	returns a reference. If the container is empty, index is ignored and the
	item will be added as first element. */

	T& insert(T&& item, std::size_t index)
	    requires HasIndex<T>
	{
		if constexpr (HasId<T>)
			assert(isUniqueId(item.id));

		if (m_items.empty())
			return add(std::move(item));

		assert(index <= m_items.size());

		m_items.insert(m_items.begin() + index, std::move(item));
		rebuildIndexes();
		return m_items[index];
	}

	/* moveByIndex
	Moves the element at 'oldIndex' to 'newIndex'. 'newIndex' can be greater than
	the last element index. */

	void moveByIndex(std::size_t oldIndex, std::size_t newIndex)
	    requires HasIndex<T>
	{
		if (oldIndex == newIndex)
			return;

		assert(oldIndex < m_items.size());

		newIndex = std::min(newIndex, m_items.size() - 1);
		auto it  = m_items.begin() + oldIndex;

		if (oldIndex < newIndex) // Forward
			std::rotate(it, it + 1, m_items.begin() + newIndex + 1);
		else // Backward
			std::rotate(m_items.begin() + newIndex, it, it + 1);

		rebuildIndexes();
	}

	/* moveById
	Moves the element with the given id to 'newIndex'. */

	void moveById(ID id, std::size_t newIndex)
	    requires HasId<T> && HasIndex<T>
	{
		moveByIndex(getById(id).index, newIndex);
	}

	void removeById(ID id)
	    requires HasId<T>
	{
		const auto f = [id](const T& item)
		{ return item.id == id; };
		m_items.erase(std::remove_if(m_items.begin(), m_items.end(), f),
		    m_items.end());

		if constexpr (HasIndex<T>)
			rebuildIndexes();
	}

	void removeByIndex(std::size_t index)
	{
		assert(index < m_items.size());

		m_items.erase(m_items.begin() + index);

		if constexpr (HasIndex<T>)
			rebuildIndexes();
	}

	void clear()
	{
		m_items.clear();
	}

	std::conditional_t<Identifiable, ID, Empty>      id    = {};
	std::conditional_t<Sortable, std::size_t, Empty> index = {};

private:
	template <typename P>
	const auto findIf(P p) const
	{
		return std::find_if(m_items.begin(), m_items.end(), p);
	}

	bool isUniqueId(ID id) const
	    requires HasId<T>
	{
		return findIf([id](const T& item)
		           { return item.id == id; }) == m_items.end();
	}

	void rebuildIndexes()
	    requires HasIndex<T>
	{
		for (std::size_t i = 0; T & item : m_items)
			item.index = i++;
	}

	std::conditional_t<std::same_as<T, Empty>, Empty, std::vector<T>> m_items;
};

/* -------------------------------------------------------------------------- */

/* Item
Specialization of Container for 'leaf' items, that should not contain any
other container. */

template <bool Identifiable = false, bool Sortable = false>
class Item : public Container<Empty, Identifiable, Sortable>
{
};
} // namespace mcl

#endif
