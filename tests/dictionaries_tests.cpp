#define BOOST_TEST_MODULE RadProtoDictionariesTests

#include "radproto/dictionaries.h"
#include "radproto/error.h"
#include <string>
#include <map>
#include <cstdint> //uint8_t, uint32_t
#include <stdexcept>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wparentheses"
#include <boost/test/unit_test.hpp>
#pragma GCC diagnostic pop

BOOST_AUTO_TEST_SUITE(DictionariesTests)

BOOST_AUTO_TEST_SUITE(BasicDictionaryTests)

BOOST_AUTO_TEST_CASE(FunctionAdd)
{
    RadProto::BasicDictionary b;

    b.add(1, "User-Name");
    b.add(1, "User");
    b.add(1, "abc");
    BOOST_CHECK_THROW(b.add(2, "abc"), RadProto::Exception);
    b.add(3, "def");

    BOOST_CHECK_EQUAL(b.name(1), "abc");
    BOOST_CHECK_EQUAL(b.name(3), "def");
    BOOST_CHECK_EQUAL(b.code("User-Name"), 1);
    BOOST_CHECK_EQUAL(b.code("User"), 1);
    BOOST_CHECK_EQUAL(b.code("abc"), 1);
    BOOST_CHECK_EQUAL(b.code("def"), 3);

    BOOST_CHECK_THROW(b.name(2), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(FunctionAppend)
{
    RadProto::BasicDictionary a;

    a.add(2, "def");
    a.add(3, "ghi");
    a.add(4, "User-Name");

    BOOST_CHECK_EQUAL(a.name(2), "def");
    BOOST_CHECK_EQUAL(a.name(3), "ghi");
    BOOST_CHECK_EQUAL(a.name(4), "User-Name");
    BOOST_CHECK_EQUAL(a.code("def"), 2);
    BOOST_CHECK_EQUAL(a.code("ghi"), 3);
    BOOST_CHECK_EQUAL(a.code("User-Name"), 4);

    RadProto::BasicDictionary b;

    b.add(1, "User-Name");

    BOOST_CHECK_EQUAL(b.name(1), "User-Name");
    BOOST_CHECK_EQUAL(b.code("User-Name"), 1);

    BOOST_CHECK_THROW(a.append(b), RadProto::Exception);

    BOOST_CHECK_THROW(a.name(1), std::out_of_range);

    RadProto::BasicDictionary c;

    c.add(4, "User");

    BOOST_CHECK_EQUAL(c.name(4), "User");
    BOOST_CHECK_EQUAL(c.code("User"), 4);

    a.append(c);

    BOOST_CHECK_EQUAL(a.name(2), "def");
    BOOST_CHECK_EQUAL(a.name(3), "ghi");
    BOOST_CHECK_EQUAL(a.name(4), "User");
    BOOST_CHECK_EQUAL(a.code("def"), 2);
    BOOST_CHECK_EQUAL(a.code("ghi"), 3);
    BOOST_CHECK_EQUAL(a.code("User"), 4);
    BOOST_CHECK_EQUAL(a.code("User-Name"), 4);

    RadProto::BasicDictionary d;

    d.add(4, "jkl");

    BOOST_CHECK_EQUAL(d.name(4), "jkl");
    BOOST_CHECK_EQUAL(d.code("jkl"), 4);

    a.append(d);

    BOOST_CHECK_EQUAL(a.name(2), "def");
    BOOST_CHECK_EQUAL(a.name(3), "ghi");
    BOOST_CHECK_EQUAL(a.name(4), "jkl");
    BOOST_CHECK_EQUAL(a.code("def"), 2);
    BOOST_CHECK_EQUAL(a.code("ghi"), 3);
    BOOST_CHECK_EQUAL(a.code("User"), 4);
    BOOST_CHECK_EQUAL(a.code("User-Name"), 4);
    BOOST_CHECK_EQUAL(a.code("jkl"), 4);

    RadProto::BasicDictionary e;

    e.add(2, "def");

    BOOST_CHECK_EQUAL(e.name(2), "def");
    BOOST_CHECK_EQUAL(e.code("def"), 2);

    a.append(e);

    BOOST_CHECK_EQUAL(a.name(2), "def");
    BOOST_CHECK_EQUAL(a.name(3), "ghi");
    BOOST_CHECK_EQUAL(a.name(4), "jkl");
    BOOST_CHECK_EQUAL(a.code("def"), 2);
    BOOST_CHECK_EQUAL(a.code("ghi"), 3);
    BOOST_CHECK_EQUAL(a.code("User"), 4);
    BOOST_CHECK_EQUAL(a.code("User-Name"), 4);
    BOOST_CHECK_EQUAL(a.code("jkl"), 4);

    RadProto::BasicDictionary f;

    f.add(4, "User-Name");

    BOOST_CHECK_EQUAL(f.name(4), "User-Name");
    BOOST_CHECK_EQUAL(f.code("User-Name"), 4);

    a.append(f);

    BOOST_CHECK_EQUAL(a.name(2), "def");
    BOOST_CHECK_EQUAL(a.name(3), "ghi");
    BOOST_CHECK_EQUAL(a.name(4), "User-Name");
    BOOST_CHECK_EQUAL(a.code("def"), 2);
    BOOST_CHECK_EQUAL(a.code("ghi"), 3);
    BOOST_CHECK_EQUAL(a.code("User-Name"), 4);
}

BOOST_AUTO_TEST_CASE(Constructor)
{
    RadProto::BasicDictionary b;

    BOOST_CHECK_THROW(b.name(0), std::out_of_range);

    BOOST_CHECK_THROW(b.code(""), std::out_of_range);

    b.add(1, "User-Name");
    b.add(1, "User");
    b.add(5, "ijk");
    b.add(3, "def");
    BOOST_CHECK_THROW(b.add(2, "User"), RadProto::Exception);

    BOOST_CHECK_EQUAL(b.name(1), "User");
    BOOST_CHECK_EQUAL(b.name(5), "ijk");
    BOOST_CHECK_EQUAL(b.name(3), "def");
    BOOST_CHECK_EQUAL(b.code("User-Name"), 1);
    BOOST_CHECK_EQUAL(b.code("User"), 1);
    BOOST_CHECK_EQUAL(b.code("ijk"), 5);
    BOOST_CHECK_EQUAL(b.code("def"), 3);
    BOOST_CHECK_THROW(b.name(2), std::out_of_range);

    RadProto::BasicDictionary c;
    c.add(2, "def");
    c.add(5, "ghi");

    BOOST_CHECK_EQUAL(c.name(2), "def");
    BOOST_CHECK_EQUAL(c.name(5), "ghi");
    BOOST_CHECK_EQUAL(c.code("def"), 2);
    BOOST_CHECK_EQUAL(c.code("ghi"), 5);

    BOOST_CHECK_THROW(c.append(b), RadProto::Exception);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
