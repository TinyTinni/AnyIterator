#include <catch.hpp>
#include <any_iterator.hpp>

// containers
#include <vector>
#include <list>

TEST_CASE("basic inc-/decrement", "[basic]")
{

    std::vector<int> vc;// = { 5,10,20 };
    vc.push_back(5);
    vc.push_back(10);
    vc.push_back(20);
    std::list<int> vl;// = { 6, 11, 21, 33 };
    vl.push_back(6);
    vl.push_back(11);
    vl.push_back(21);
    vl.push_back(33);
    auto i = vc.begin();
    i = vc.end();
    SECTION("normal increment")
    {
        tyti::any_iterator<int> it(vc.begin());
        auto itc = vc.begin();
        for (;it != vc.end(); ++it, ++itc)
            REQUIRE(*it == *itc);
    }
    SECTION("increment with conatiner switch")
    {
        tyti::any_iterator<int> it(vc.begin());
        auto itc = vc.begin();
        for (; it != vc.end(); ++it, ++itc)
            REQUIRE(*it == *itc);
        it = vl.begin();
        auto itl = vl.begin();
        for (; it != vl.end(); ++it, ++itl)
            REQUIRE(*it == *itl);
    }
    
    SECTION("decrement from end")
    {
        tyti::any_iterator<int> it(vc.end());
        auto itc = vc.end();
        do
        {
            --it;
            --itc;
            REQUIRE(*it == *itc);
        }while (it != vc.begin());
    }

    SECTION("decrement from end with conatiner switch")
    {
        tyti::any_iterator<int> it(vc.end());
        auto itc = vc.end(); 
        do
        {
            --it;
            --itc;
            REQUIRE(*it == *itc);
        } while (it != vc.begin());
        
        
        it = vl.end();
        auto itl = vl.end();
        do
        {
            --it;
            --itl;
            REQUIRE(*it == *itl);
        } while (it != vl.begin());
    }

    SECTION("assign from any_vector")
    {
        tyti::any_iterator<int> it_src(vc.begin());
        tyti::any_iterator<int> it(it_src);
        REQUIRE(it_src == it);
        REQUIRE(*it_src == *it);
        it_src = it;
        REQUIRE(it_src == it);
        REQUIRE(*it_src == *it);
    }

    SECTION("compare with non-any")
    {
        tyti::any_iterator<int> it_src(vc.begin());
        REQUIRE(it_src == vc.begin());
        REQUIRE(it_src != vc.end());
    }
}

