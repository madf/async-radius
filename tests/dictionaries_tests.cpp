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

BOOST_AUTO_TEST_CASE(TestAdd)
{
    RadProto::BasicDictionary b;

    b.add(1, "User-Name");
    b.add(1, "User");
    b.add(1, "abc");
    BOOST_CHECK_THROW(b.add(2, "abc"), RadProto::Exception);
    b.add(3, "def");
    b.add(1, "abc");

    BOOST_CHECK_EQUAL(b.name(1), "abc");
    BOOST_CHECK_EQUAL(b.name(3), "def");
    BOOST_CHECK_EQUAL(b.code("User-Name"), 1);
    BOOST_CHECK_EQUAL(b.code("User"), 1);
    BOOST_CHECK_EQUAL(b.code("abc"), 1);
    BOOST_CHECK_EQUAL(b.code("def"), 3);

    BOOST_CHECK_THROW(b.name(2), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(TestAppend)
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
    BOOST_CHECK_EQUAL(a.code("User"), 4);
    BOOST_CHECK_EQUAL(a.code("jkl"), 4);
}

BOOST_AUTO_TEST_CASE(TestConstructor)
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

    RadProto::BasicDictionary a;

    a.add(2, "def");

    BOOST_CHECK_EQUAL(a.name(2), "def");
    BOOST_CHECK_EQUAL(a.code("def"), 2);

    c.append(a);

    BOOST_CHECK_EQUAL(c.name(2), "def");
    BOOST_CHECK_EQUAL(c.name(5), "ghi");
    BOOST_CHECK_EQUAL(c.code("def"), 2);
    BOOST_CHECK_EQUAL(c.code("ghi"), 5);
}
BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(DependentDictionaryTests)

BOOST_AUTO_TEST_CASE(TestAdd)
{
    RadProto::DependentDictionary b;

    b.add(1, "Login-User", "Service-Type");
    b.add(2, "Framed-User", "Service-Type");
    b.add(3, "def", "abc");
    b.add(3, "ghi", "abc");
    BOOST_CHECK_THROW(b.add(4, "Framed-User", "Service-Type"), RadProto::Exception);
    b.add(3, "ghi", "abc");

    BOOST_CHECK_EQUAL(b.name("Service-Type", 1), "Login-User");
    BOOST_CHECK_EQUAL(b.name("Service-Type", 2), "Framed-User");
    BOOST_CHECK_EQUAL(b.name("abc", 3), "ghi");
    BOOST_CHECK_EQUAL(b.code("Service-Type", "Login-User"), 1);
    BOOST_CHECK_EQUAL(b.code("Service-Type", "Framed-User"), 2);
    BOOST_CHECK_EQUAL(b.code("abc", "def"), 3);
    BOOST_CHECK_EQUAL(b.code("abc", "ghi"), 3);

    BOOST_CHECK_THROW(b.name("Service-Type", 4), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(TestAppend)
{
    RadProto::DependentDictionary a;

    a.add(1, "Login-User", "Service-Type");
    a.add(2, "Framed-User", "Service-Type");
    a.add(3, "def", "abc");

    BOOST_CHECK_EQUAL(a.name("Service-Type", 1), "Login-User");
    BOOST_CHECK_EQUAL(a.name("Service-Type", 2), "Framed-User");
    BOOST_CHECK_EQUAL(a.name("abc", 3), "def");
    BOOST_CHECK_EQUAL(a.code("Service-Type", "Login-User"), 1);
    BOOST_CHECK_EQUAL(a.code("Service-Type", "Framed-User"), 2);
    BOOST_CHECK_EQUAL(a.code("abc", "def"), 3);

    RadProto::DependentDictionary b;

    b.add(4, "def", "abc");

    BOOST_CHECK_EQUAL(b.name("abc", 4), "def");
    BOOST_CHECK_EQUAL(b.code("abc", "def"), 4);

    BOOST_CHECK_THROW(a.append(b), RadProto::Exception);

    BOOST_CHECK_THROW(a.name("abc", 4), std::out_of_range);

    RadProto::DependentDictionary c;

    c.add(3, "ghi", "abc");

    BOOST_CHECK_EQUAL(c.name("abc", 3), "ghi");
    BOOST_CHECK_EQUAL(c.code("abc", "ghi"), 3);

    a.append(c);

    BOOST_CHECK_EQUAL(a.name("Service-Type", 1), "Login-User");
    BOOST_CHECK_EQUAL(a.name("Service-Type", 2), "Framed-User");
    BOOST_CHECK_EQUAL(a.name("abc", 3), "ghi");
    BOOST_CHECK_EQUAL(a.code("Service-Type", "Login-User"), 1);
    BOOST_CHECK_EQUAL(a.code("Service-Type", "Framed-User"), 2);
    BOOST_CHECK_EQUAL(a.code("abc", "def"), 3);
    BOOST_CHECK_EQUAL(a.code("abc", "ghi"), 3);

    RadProto::DependentDictionary d;

    d.add(1, "Login-User", "Service-Type");

    BOOST_CHECK_EQUAL(d.name("Service-Type", 1), "Login-User");
    BOOST_CHECK_EQUAL(d.code("Service-Type", "Login-User"), 1);

    a.append(d);

    BOOST_CHECK_EQUAL(a.name("Service-Type", 1), "Login-User");
    BOOST_CHECK_EQUAL(a.name("Service-Type", 2), "Framed-User");
    BOOST_CHECK_EQUAL(a.name("abc", 3), "ghi");
    BOOST_CHECK_EQUAL(a.code("Service-Type", "Login-User"), 1);
    BOOST_CHECK_EQUAL(a.code("Service-Type", "Framed-User"), 2);
    BOOST_CHECK_EQUAL(a.code("abc", "def"), 3);
    BOOST_CHECK_EQUAL(a.code("abc", "ghi"), 3);

    RadProto::DependentDictionary e;

    e.add(10, "Call-Check", "Service-Type");

    BOOST_CHECK_EQUAL(e.name("Service-Type", 10), "Call-Check");
    BOOST_CHECK_EQUAL(e.code("Service-Type", "Call-Check"), 10);

    a.append(e);

    BOOST_CHECK_EQUAL(a.name("Service-Type", 1), "Login-User");
    BOOST_CHECK_EQUAL(a.name("Service-Type", 2), "Framed-User");
    BOOST_CHECK_EQUAL(a.name("abc", 3), "ghi");
    BOOST_CHECK_EQUAL(a.name("Service-Type", 10), "Call-Check");
    BOOST_CHECK_EQUAL(a.code("Service-Type", "Login-User"), 1);
    BOOST_CHECK_EQUAL(a.code("Service-Type", "Framed-User"), 2);
    BOOST_CHECK_EQUAL(a.code("abc", "def"), 3);
    BOOST_CHECK_EQUAL(a.code("abc", "ghi"), 3);
    BOOST_CHECK_EQUAL(a.code("Service-Type", "Call-Check"), 10);


    RadProto::DependentDictionary f;

    f.add(3, "def", "abc");

    BOOST_CHECK_EQUAL(f.name("abc", 3), "def");
    BOOST_CHECK_EQUAL(f.code("abc", "def"), 3);

    a.append(f);

    BOOST_CHECK_EQUAL(a.name("Service-Type", 1), "Login-User");
    BOOST_CHECK_EQUAL(a.name("Service-Type", 2), "Framed-User");
    BOOST_CHECK_EQUAL(a.name("abc", 3), "def");
    BOOST_CHECK_EQUAL(a.name("Service-Type", 10), "Call-Check");
    BOOST_CHECK_EQUAL(a.code("Service-Type", "Login-User"), 1);
    BOOST_CHECK_EQUAL(a.code("Service-Type", "Framed-User"), 2);
    BOOST_CHECK_EQUAL(a.code("abc", "def"), 3);
    BOOST_CHECK_EQUAL(a.code("abc", "ghi"), 3);
    BOOST_CHECK_EQUAL(a.code("Service-Type", "Call-Check"), 10);
}

BOOST_AUTO_TEST_CASE(TestConstructor)
{
    RadProto::DependentDictionary b;

    BOOST_CHECK_THROW(b.name("", 0), std::out_of_range);

    BOOST_CHECK_THROW(b.code("", ""), std::out_of_range);

    b.add(2, "Framed-User", "Service-Type");
    b.add(3, "def", "abc");
    b.add(3, "ghi", "abc");
    BOOST_CHECK_THROW(b.add(4, "Framed-User", "Service-Type"), RadProto::Exception);
    b.add(3, "ghi", "abc");

    BOOST_CHECK_EQUAL(b.name("Service-Type", 2), "Framed-User");
    BOOST_CHECK_EQUAL(b.name("abc", 3), "ghi");
    BOOST_CHECK_EQUAL(b.code("Service-Type", "Framed-User"), 2);
    BOOST_CHECK_EQUAL(b.code("abc", "def"), 3);
    BOOST_CHECK_EQUAL(b.code("abc", "ghi"), 3);

    BOOST_CHECK_THROW(b.name("Service-Type", 4), std::out_of_range);

    RadProto::DependentDictionary c;

    c.add(3, "Framed-User", "Service-Type");
    c.add(5, "jkl", "abc");

    BOOST_CHECK_EQUAL(c.name("Service-Type", 3), "Framed-User");
    BOOST_CHECK_EQUAL(c.name("abc", 5), "jkl");
    BOOST_CHECK_EQUAL(c.code("Service-Type", "Framed-User"), 3);
    BOOST_CHECK_EQUAL(c.code("abc", "jkl"), 5);

    BOOST_CHECK_THROW(c.append(b), RadProto::Exception);

    RadProto::DependentDictionary a;

    a.add(5, "jkl", "abc");

    BOOST_CHECK_EQUAL(a.name("abc", 5), "jkl");
    BOOST_CHECK_EQUAL(a.code("abc", "jkl"), 5);

    c.append(a);

    BOOST_CHECK_EQUAL(c.name("Service-Type", 3), "Framed-User");
    BOOST_CHECK_EQUAL(c.name("abc", 5), "jkl");
    BOOST_CHECK_EQUAL(c.code("Service-Type", "Framed-User"), 3);
    BOOST_CHECK_EQUAL(c.code("abc", "jkl"), 5);
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(DictionariesTests)

BOOST_AUTO_TEST_CASE(TestAttributeName)
{
    RadProto::Dictionaries a("dictionary");

    BOOST_CHECK_EQUAL(a.attributeName(1), "User-Name");
}

BOOST_AUTO_TEST_CASE(TestAttributeCode)
{
    RadProto::Dictionaries a("dictionary");

    BOOST_CHECK_EQUAL(a.attributeCode("User-Password"), 2);
}

BOOST_AUTO_TEST_CASE(TestAttributeValueName)
{
    RadProto::Dictionaries a("dictionary");

    BOOST_CHECK_EQUAL(a.attributeValueName("Service-Type", 1), "Login-User");
}

BOOST_AUTO_TEST_CASE(TestAttributeValueCode)
{
    RadProto::Dictionaries a("dictionary");

    BOOST_CHECK_EQUAL(a.attributeValueCode("Service-Type", "Framed-User"), 2);
}

BOOST_AUTO_TEST_CASE(TestVendorName)
{
    RadProto::Dictionaries a("dictionary");

    BOOST_CHECK_EQUAL(a.vendorName(171), "Dlink");
}

BOOST_AUTO_TEST_CASE(TestVendorCode)
{
    RadProto::Dictionaries a("dictionary");

    BOOST_CHECK_EQUAL(a.vendorCode("Dlink"), 171);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
