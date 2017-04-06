#include <catch.hpp>
#include <AnyIterator.hpp>

// containers
#include <vector>
#include <list>

TEST_CASE("basic_increment", "[basic]")
{

	std::vector<int> vc = { 5,10,20 };
	std::list<int> vl = { 6, 11, 21, 33 };
	SECTION("normal increment")
	{
		tyti::AnyIteratorT<int> it(vc.begin());
		auto itc = vc.begin();
		for (;it != vc.end(); ++it, ++itc)
			REQUIRE(*it == *itc);
	}
	SECTION("increment with conatiner switch")
	{
		tyti::AnyIteratorT<int> it(vc.begin());
		auto itc = vc.begin();
		for (; it != vc.end(); ++it, ++itc)
			REQUIRE(*it == *itc);
		it = vl.begin();
		auto itl = vl.begin();
		for (; it != vl.end(); ++it, ++itl)
			REQUIRE(*it == *itl);
	}
}