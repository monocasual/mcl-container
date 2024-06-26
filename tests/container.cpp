#include "src/container.hpp"
#include "src/id.hpp"
#include <catch2/catch_test_macros.hpp>
#include <iostream>

using namespace mcl;

TEST_CASE("ID")
{
	ID::reset();

	SECTION("generation")
	{
		ID id = ID::generate();

		REQUIRE(id == ID(1));
	}

	SECTION("generation with value")
	{
		const int value = 45;

		ID id = ID::generate(value);

		REQUIRE(id == ID(value));

		SECTION("using old value")
		{
			ID id2 = ID::generate(value);

			REQUIRE(id2 == ID(value + 1)); // 'value' is old, expected a new one
		}
	}
}

TEST_CASE("Container")
{
	using Item      = Item</*Identifiable=*/true, /*Sortable=*/true>;
	using Container = Container<Item>;

	Container container;

	SECTION("test allocation")
	{
		REQUIRE(container.size() == 0);
	}

	SECTION("test add")
	{
		Item item1;
		Item item2;
		item1.id = 1;
		item2.id = 2;

		container.add(std::move(item1));
		container.add(std::move(item2));

		REQUIRE(container.size() == 2);
		REQUIRE(container.getById(1).id == ID(1));
		REQUIRE(container.getById(1).index == 0);
		REQUIRE(container.getById(2).id == ID(2));
		REQUIRE(container.getById(2).index == 1);
		REQUIRE(container.contains(2) == true);
		REQUIRE(container.contains(4) == false);

		SECTION("test iterators")
		{
			for (const Item& item : container)
				;
			for (Item& item : container)
				;
		}
	}
}

TEST_CASE("Container, recursive mode")
{
	struct Channel : public Container<Channel, /*Identifiable=*/true>
	{
	};

	struct Column : public Container<Channel>
	{
	};

	Channel channel1;
	Channel channel2;
	Channel channel3;
	Channel subChannel4;
	Channel subChannel5;
	Channel subChannel6;
	channel1.id    = 1;
	channel2.id    = 2;
	channel3.id    = 3;
	subChannel4.id = 4;
	subChannel5.id = 5;
	subChannel6.id = 6;
	channel1.add(std::move(subChannel4));
	channel1.add(std::move(subChannel5));
	channel1.add(std::move(subChannel6));

	Column column;
	column.add(std::move(channel1));
	column.add(std::move(channel2));
	column.add(std::move(channel3));

	REQUIRE(column.size() == 3);
	REQUIRE(column.findById(4) == nullptr);              // sub-channel should not be found in shallow find...
	REQUIRE(column.deepFindById<Channel>(4) != nullptr); // ...but should be in deep find

	SECTION("test shallow deletion by id")
	{
		column.removeById(1);

		REQUIRE(column.size() == 2);
		REQUIRE(column.findById(1) == nullptr);
	}

	SECTION("test deep deletion by id")
	{
		column.deepRemoveById<Channel>(6); // channel with ID=6 was inside channel with ID=1

		REQUIRE(column.size() == 3);
		REQUIRE(column.getById(1).size() == 2);
		REQUIRE(column.deepFindById<Channel>(6) == nullptr);
	}
}