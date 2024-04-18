#include "src/container.hpp"
#include <catch2/catch_test_macros.hpp>
#include <iostream>

using namespace mcl;

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
		const int id = 1;

		Item item;
		item.id = id;

		container.add(std::move(item));

		REQUIRE(container.size() == 1);
		REQUIRE(container.getById(id).id == id);
		REQUIRE(container.getById(id).index == 0);

		SECTION("test iterators")
		{
			for (const Item& item : container)
				std::cout << item.id << "\n";
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
}