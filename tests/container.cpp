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

		container.add({id});

		REQUIRE(container.size() == 1);
		REQUIRE(container.getById(id).id == id);
		REQUIRE(container.getById(id).index == 0);
	}
}
