#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#include "gravedata.h"
#include "hashmap.h"
#include <functional>
#include <iostream>
#include <regex>
#include <sstream>
#include <vector>

using gint = ian::GraveData;
using gimap = Hashmap<int, gint>;

struct pair {
    int key;
    int value;
};

#ifdef DEBUG
void forceResize(gimap& map) {
    map.resize();
}
int getBucketCount(gimap& map) {
    return map._bucket_count;
}
#endif

TEST_SUITE("constructors") {
    TEST_CASE("test default constructor") {
        gint::init();

        gimap map;

        CHECK_EQ(0, map.size());
        CHECK_EQ(0, gint::count());
    }

    TEST_CASE("test copy constructor with empty map") {
        gint::init();

        gimap map;

        gimap newMap(map);

        CHECK_EQ(map.size(), newMap.size());
        CHECK_EQ(0, gint::count());
    }
    TEST_CASE("test copy constructor with non-empty map") {
        gint::init();

        gimap map;
        map.add(1, 9867);

        gimap newMap(map);

        CHECK_EQ(map.size(), newMap.size());
        CHECK_EQ(map.get(1), newMap.get(1));
        CHECK_EQ(2, gint::count());
    }

    TEST_CASE("test move constructor with empty map") {
        gint::init();

        gimap map;

        gimap newMap(std::move(map));

        CHECK_EQ(0, newMap.size());
        CHECK_EQ(0, gint::count());
    }
    TEST_CASE("test move constructor with non-empty map") {
        gint::init();

        gimap map;
        map.add(1, 9867);

        gimap newMap(std::move(map));

        CHECK_EQ(1, newMap.size());
        CHECK_EQ(9867, newMap.get(1));
        CHECK_EQ(1, gint::count());
    }

    TEST_CASE("test destructor with empty map") {
        gint::init();

        gimap *map = new gimap;

        delete map;

        auto changes = gint::changes();

        CHECK_EQ(0, changes.increments);
        CHECK_EQ(0, changes.decrements);
        CHECK_EQ(0, gint::count());
    }
    TEST_CASE("test destructor with non-empty map") {
        gint::init();

        gimap *map = new gimap;

        map->add(1, 9867);

        delete map;

        auto changes = gint::changes();

        CHECK_EQ(2, changes.increments);
        CHECK_EQ(2, changes.decrements);
        CHECK_EQ(0, gint::count());
    }
}

TEST_SUITE("methods") {
    TEST_CASE("test add with empty map") {
        gint::init();

        gimap map;
        CHECK(map.add(1, 9867));

        auto changes = gint::changes();

        CHECK_EQ(1, gint::count());
        CHECK_EQ(1, map.size());
        CHECK_EQ(9867, map.get(1));
    }
    TEST_CASE("test add with non-empty map") {
        gint::init();

        gimap map;
        map.add(0, 1);

        CHECK(map.add(1, 9867));

        CHECK_EQ(2, gint::count());
        CHECK_EQ(2, map.size());
        CHECK_EQ(9867, map.get(1));
    }
    TEST_CASE("test add with duplicate") {
        gint::init();

        gimap map;
        map.add(1, 0);

        CHECK_FALSE(map.add(1, 9867));

        CHECK_EQ(1, map.size());
        CHECK_EQ(0, map.get(1));
        CHECK_EQ(1, gint::count());
    }

    TEST_CASE("test put with empty map") {
        gint::init();

        gimap map;
        map.put(1, 9867);

        CHECK_EQ(1, gint::count());
        CHECK_EQ(1, map.size());
        CHECK_EQ(9867, map.get(1));
    }
    TEST_CASE("test put with non-empty map") {
        gint::init();

        gimap map;
        map.add(0, 1);

        map.put(1, 9867);

        CHECK_EQ(2, gint::count());
        CHECK_EQ(2, map.size());
        CHECK_EQ(9867, map.get(1));
    }
    TEST_CASE("test put with duplicate key") {
        gint::init();

        gimap map;
        map.add(1, 0);

        map.put(1, 9867);

        CHECK_EQ(1, gint::count());
        CHECK_EQ(1, map.size());
        CHECK_EQ(9867, map.get(1));
    }

    TEST_CASE("test remove with invalid key") {
        gint::init();

        gimap map;

        CHECK_THROWS(map.remove(1));
        CHECK_EQ(0, gint::count());
        CHECK_EQ(0, map.size());
    }
    TEST_CASE("test remove with valid key") {
        gint::init();

        gimap map;
        map.add(1, 9867);

        CHECK_EQ(9867, map.remove(1));
        CHECK_EQ(0, gint::count());
        CHECK_EQ(0, map.size());
    }

    TEST_CASE("test contains with invalid key") {
        gint::init();

        gimap map;

        CHECK_FALSE(map.contains(1));
        CHECK_EQ(0, gint::count());
        CHECK_EQ(0, map.size());
    }
    TEST_CASE("test contains with valid key") {
        gint::init();

        gimap map;
        map.add(1, 9867);

        CHECK(map.contains(1));
        CHECK_EQ(1, gint::count());
        CHECK_EQ(1, map.size());
    }

    TEST_CASE("test get with invalid key") {
        gint::init();

        gimap map;

        CHECK_THROWS(map.get(1));
        CHECK_EQ(0, gint::count());
        CHECK_EQ(0, map.size());
    }
    TEST_CASE("test get with valid key") {
        gint::init();

        gimap map;
        map.add(1, 9867);

        CHECK_EQ(9867, map.get(1));
        CHECK_EQ(1, gint::count());
        CHECK_EQ(1, map.size());
    }

    TEST_CASE("test size") {
        gint::init();

        gimap map;

        CHECK_EQ(0, map.size());

        map.add(0, 1);

        CHECK_EQ(1, map.size());

        map.add(1, 2);

        CHECK_EQ(2, map.size());
    }

    TEST_CASE("test clear with empty map") {
        gint::init();

        gimap map;

        map.clear();

        CHECK_EQ(0, map.size());
        CHECK_EQ(0, gint::count());
    }

    TEST_CASE("test clear with non-empty map") {
        gint::init();

        gimap map;
        map.add(1, 9867);

        map.clear();

        CHECK_EQ(0, map.size());
        CHECK_EQ(0, gint::count());
    }

    TEST_CASE("test optimize with empty map") {
        gint::init();

        gimap map;

        CHECK_FALSE(map.optimize());

        CHECK_EQ(16, getBucketCount(map));
        CHECK_EQ(0, gint::count());
        CHECK_EQ(0, map.size());
    }
    TEST_CASE("test optimize false with non-empty map") {
        gint::init();

        gimap map;
        map.add(1, 9999);

        CHECK_FALSE(map.optimize());

        CHECK_EQ(16, getBucketCount(map));
        CHECK_EQ(1, gint::count());
        CHECK_EQ(1, map.size());
        CHECK_EQ(9999, map.get(1));
    }
    TEST_CASE("test optimize true") {
        gint::init();

        gimap map;
        map.add(1, 9999);

        forceResize(map);

        CHECK(map.optimize());
        
        CHECK_EQ(16, getBucketCount(map));
        CHECK_EQ(1, gint::count());
        CHECK_EQ(1, map.size());
        CHECK_EQ(9999, map.get(1));
    }
    TEST_CASE("test optimize true with large map") {
        gint::init();

        gimap map;
        for (int i = 0; i < 64; ++i) {
            map.add(i, i+1000);
        }

        forceResize(map);

        CHECK(map.optimize());

        CHECK_EQ(64, gint::count());
        CHECK_EQ(64, map.size());
        for (int i = 0; i < 64; ++i) {
            REQUIRE_EQ(i+1000, map.get(i));
        }
        CHECK_EQ(16, getBucketCount(map));
    }
}

TEST_SUITE("operators") {
    TEST_CASE("test copy operator with empty map") {
        gint::init();

        gimap map;

        gimap newMap = map;

        CHECK_EQ(0, gint::count());
        CHECK_EQ(0, map.size());
        CHECK_EQ(0, newMap.size());
    }
    TEST_CASE("test copy operator with non-empty map") {
        gint::init();

        gimap map;
        map.add(1, 9867);

        gimap newMap = map;

        CHECK_EQ(2, gint::count());
        CHECK_EQ(1, map.size());
        CHECK_EQ(1, newMap.size());
        CHECK_EQ(9867, map.get(1));
        CHECK_EQ(9867, newMap.get(1));
    }

    TEST_CASE("test move operator with empty map") {
        gint::init();

        gimap map;

        gimap newMap = std::move(map);

        CHECK_EQ(0, gint::count());
        CHECK_EQ(0, newMap.size());
    }
    TEST_CASE("test move opertor with non-empty map") {
        gint::init();

        gimap map;
        map.add(1, 9867);

        gimap newMap = std::move(map);

        CHECK_EQ(1, gint::count());
        CHECK_EQ(1, newMap.size());
        CHECK_EQ(9867, newMap.get(1));
    }

    TEST_CASE("test append operator with empty maps") {
        gint::init();

        gimap map;
        gimap otherMap;

        gimap newMap = map + otherMap;

        CHECK_EQ(0, gint::count());
        CHECK_EQ(0, newMap.size());
    }
    TEST_CASE("test append operator with non duplicate map") {
        gint::init();

        gimap map;
        map.add(1, 9867);
        map.add(2, 9999);
        gimap otherMap;
        otherMap.add(3, 2146);
        otherMap.add(4, 20);

        gimap newMap = map + otherMap;

        CHECK_EQ(8, gint::count());
        CHECK_EQ(4, newMap.size());
        CHECK_EQ(9867, newMap.get(1));
        CHECK_EQ(9999, newMap.get(2));
        CHECK_EQ(2146, newMap.get(3));
        CHECK_EQ(20, newMap.get(4));
    }
    TEST_CASE("test append operator with duplicate map") {
        gint::init();

        gimap map;
        map.add(1, 9867);
        map.add(2, 9999);
        gimap otherMap;
        otherMap.add(1, 3);
        otherMap.add(2, 1);

        gimap newMap = map + otherMap;

        CHECK_EQ(6, gint::count());
        CHECK_EQ(2, newMap.size());
        CHECK_EQ(9867, newMap.get(1));
        CHECK_EQ(9999, newMap.get(2));
    }
    TEST_CASE("test append operator with empty left map") {
        gint::init();

        gimap map;
        gimap otherMap;
        otherMap.add(1, 9867);

        gimap newMap = map + otherMap;

        CHECK_EQ(2, gint::count());
        CHECK_EQ(1, newMap.size());
        CHECK_EQ(9867, newMap.get(1));
    }
    TEST_CASE("test append operator with empty right map") {
        gint::init();

        gimap map;
        map.add(1, 9867);
        gimap otherMap;

        gimap newMap = map + otherMap;

        CHECK_EQ(2, gint::count());
        CHECK_EQ(1, newMap.size());
        CHECK_EQ(9867, newMap.get(1));
    }

    TEST_CASE("test append equals operator with empty map") {
        gint::init();

        gimap map;
        gimap otherMap;

        map += otherMap;

        CHECK_EQ(0, gint::count());
        CHECK_EQ(0, map.size());
    }
    TEST_CASE("test append equals operator with non duplicate map") {
        gint::init();

        gimap map;
        map.add(1, 9867);
        map.add(2, 9999);
        gimap otherMap;
        otherMap.add(3, 2146);
        otherMap.add(4, 20);

        map += otherMap;

        CHECK_EQ(6, gint::count());
        CHECK_EQ(4, map.size());
        CHECK_EQ(9867, map.get(1));
        CHECK_EQ(9999, map.get(2));
        CHECK_EQ(2146, map.get(3));
        CHECK_EQ(20, map.get(4));
    }
    TEST_CASE("test append equals operator with duplicate map") {
        gint::init();

        gimap map;
        map.add(1, 9867);
        map.add(2, 9999);
        gimap otherMap;
        otherMap.add(1, 3);
        otherMap.add(2, 1);

        map += otherMap;

        CHECK_EQ(4, gint::count());
        CHECK_EQ(2, map.size());
        CHECK_EQ(9867, map.get(1));
        CHECK_EQ(9999, map.get(2));
    }
    TEST_CASE("test append equals operator with empty left map") {
        gint::init();

        gimap map;
        gimap otherMap;
        otherMap.add(1, 9867);

        map += otherMap;

        CHECK_EQ(2, gint::count());
        CHECK_EQ(1, map.size());
        CHECK_EQ(9867, map.get(1));
    }
    TEST_CASE("test append equals operator with empty right map") {
        gint::init();

        gimap map;
        map.add(1, 9867);
        gimap otherMap;

        map += otherMap;

        CHECK_EQ(1, gint::count());
        CHECK_EQ(1, map.size());
        CHECK_EQ(9867, map.get(1));
    }

    TEST_CASE("test equals operator with empty map") {
        gimap map;
        gimap otherMap;

        CHECK(map == otherMap);

        map.add(1, 2);

        CHECK_FALSE(map == otherMap);
    }
    TEST_CASE("test equals operator with non-empty map") {
        gimap map;
        map.add(2, 9999);
        map.add(1, 9867);
        gimap otherMap;
        otherMap.add(1, 9867);
        otherMap.add(2, 9999);

        CHECK(map == otherMap);

        map.add(3, 0);

        CHECK_FALSE(map == otherMap);
    }

    TEST_CASE("test !equals operator with empty map") {
        gimap map;
        gimap otherMap;

        CHECK_FALSE(map != otherMap);

        map.add(1, 0);

        CHECK(map != otherMap);
    }
    TEST_CASE("test !equals operator with non-empty map") {
        gimap map;
        map.add(2, 9999);
        map.add(1, 9867);
        gimap otherMap;
        otherMap.add(1, 9867);
        otherMap.add(2, 9999);

        CHECK_FALSE(map != otherMap);

        map.add(3, 0);

        CHECK(map != otherMap);
    }

    TEST_CASE("test stream insertion operator with empty map") {
        gimap map;

        std::stringstream stream;

        stream << map;

        CHECK_EQ("{ }", stream.str());
    }
    TEST_CASE("test stream insertion operator with non-empty map") {
        gimap map;
        int keys[] = {1, 2, 3, 4};
        int values[] = {9967, 9999, 1, 2379};
        for (int i = 0; i < 4; ++i) {
            map.add(keys[i], values[i]);
        }

        std::stringstream stream;

        stream << map;

        std::string s = stream.str();

        std::regex full("\\{ (\\(\\w+, \\w+\\) ?)*\\}");

        INFO("s = ", s);

        std::cmatch full_match;

        bool full_success = std::regex_match(s.c_str(), full_match, full);
        REQUIRE(full_success);

        REQUIRE_GE(s.find("(3, 1)"), 0);
        REQUIRE_GE(s.find("(4, 2379)"), 0);
        REQUIRE_GE(s.find("(1, 9967)"), 0);
        REQUIRE_GE(s.find("(2, 9999)"), 0);
    }
}

TEST_CASE("test resize with empty map") {
    gint::init();
    gimap map;

    #ifndef DEBUG
    INFO("DEBUG macro not defined");
    REQUIRE(false);
    #else
        forceResize(map);
    #endif

    CHECK_EQ(0, gint::count());
    CHECK_EQ(0, map.size());
}
TEST_CASE("test resize with non-empty map") {
    gint::init();
    gimap map;

    int keys[] = {1, 2, 3, 4};
    int values[] = {9967, 9999, 1, 2379};
    for (int i = 0; i < 4; ++i) {
        map.add(keys[i], values[i]);
    }
    
    #ifndef DEBUG
    #error "DEBUG macro not defined"
    REQUIRE(false);
    #else
        forceResize(map);
    #endif

    CHECK_EQ(4, gint::count());
    CHECK_EQ(4, map.size());
    CHECK_EQ(9967, map.get(1));
    CHECK_EQ(9999, map.get(2));
    CHECK_EQ(1, map.get(3));
}
