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

//  1.  codes !=, names == - exception

    b.add(1, "abc", "string");
    BOOST_CHECK_THROW(b.add(2, "abc", "string"), RadProto::Exception);
    BOOST_CHECK_THROW(b.add(2, "abc", "integer"), RadProto::Exception);

    BOOST_CHECK_EQUAL(b.name(1), "abc");
    BOOST_CHECK_EQUAL(b.code("abc"), 1);
    BOOST_CHECK_EQUAL(b.type("abc"), "string");
    BOOST_CHECK_EQUAL(b.type(1), "string");

    BOOST_CHECK_THROW(b.name(2), std::out_of_range);
    BOOST_CHECK_THROW(b.type(2), std::out_of_range);


//  2.  codes ==, names !=, types ==

    b.add(3, "def", "string");
    b.add(3, "ghi", "string");

    BOOST_CHECK_EQUAL(b.name(3), "ghi");
    BOOST_CHECK_EQUAL(b.type(3), "string");

    BOOST_CHECK_EQUAL(b.code("def"), 3);
    BOOST_CHECK_EQUAL(b.code("ghi"), 3);

    BOOST_CHECK_EQUAL(b.type("def"), "string");
    BOOST_CHECK_EQUAL(b.type("ghi"), "string");

//  2.1.  codes ==, names !=, types !=

    b.add(4, "jkl", "string");
    b.add(4, "mno", "integer");

    BOOST_CHECK_EQUAL(b.name(4), "mno");
    BOOST_CHECK_EQUAL(b.type(4), "integer");

    BOOST_CHECK_EQUAL(b.code("jkl"), 4);
    BOOST_CHECK_EQUAL(b.code("mno"), 4);

    BOOST_CHECK_EQUAL(b.type("jkl"), "string");
    BOOST_CHECK_EQUAL(b.type("mno"), "integer");


//  3.  codes ==, names ==, types ==

    b.add(5, "cde", "integer");
    b.add(5, "cde", "integer");

    BOOST_CHECK_EQUAL(b.name(5), "cde");
    BOOST_CHECK_EQUAL(b.type(5), "integer");

    BOOST_CHECK_EQUAL(b.code("cde"), 5);

    BOOST_CHECK_EQUAL(b.type("cde"), "integer");

//  3.1.  codes ==, names ==, types !=

    b.add(6, "efg", "integer");
    b.add(6, "efg", "string");

    BOOST_CHECK_EQUAL(b.name(6), "efg");
    BOOST_CHECK_EQUAL(b.type(6), "string");

    BOOST_CHECK_EQUAL(b.code("efg"), 6);

    BOOST_CHECK_EQUAL(b.type("efg"), "string");
}

BOOST_AUTO_TEST_CASE(TestAppend)
{

//  1.  codes !=, names == - exception

    RadProto::BasicDictionary a;

    a.add(1, "abc", "string");

    BOOST_CHECK_EQUAL(a.name(1), "abc");
    BOOST_CHECK_EQUAL(a.code("abc"), 1);
    BOOST_CHECK_EQUAL(a.type("abc"), "string");
    BOOST_CHECK_EQUAL(a.type(1), "string");

    RadProto::BasicDictionary b;

    b.add(2, "abc", "string");

    BOOST_CHECK_EQUAL(b.name(2), "abc");
    BOOST_CHECK_EQUAL(b.code("abc"), 2);
    BOOST_CHECK_EQUAL(b.type("abc"), "string");
    BOOST_CHECK_EQUAL(b.type(2), "string");

    BOOST_CHECK_THROW(a.append(b), RadProto::Exception);

    RadProto::BasicDictionary c;

    c.add(2, "abc", "integer");

    BOOST_CHECK_EQUAL(c.name(2), "abc");
    BOOST_CHECK_EQUAL(c.code("abc"), 2);
    BOOST_CHECK_EQUAL(c.type("abc"), "integer");
    BOOST_CHECK_EQUAL(c.type(2), "integer");

    BOOST_CHECK_THROW(a.append(c), RadProto::Exception);

    BOOST_CHECK_THROW(a.name(2), std::out_of_range);
    BOOST_CHECK_THROW(a.type(2), std::out_of_range);


//  2.  codes ==, names !=, types ==

    RadProto::BasicDictionary d;

    d.add(1, "abc", "string");

    BOOST_CHECK_EQUAL(d.name(1), "abc");
    BOOST_CHECK_EQUAL(d.type(1), "string");
    BOOST_CHECK_EQUAL(d.code("abc"), 1);
    BOOST_CHECK_EQUAL(d.type("abc"), "string");

    RadProto::BasicDictionary e;

    e.add(1, "bcd", "string");

    BOOST_CHECK_EQUAL(e.name(1), "bcd");
    BOOST_CHECK_EQUAL(e.type(1), "string");
    BOOST_CHECK_EQUAL(e.code("bcd"), 1);
    BOOST_CHECK_EQUAL(e.type("bcd"), "string");

    d.append(e);

    BOOST_CHECK_EQUAL(d.name(1), "bcd");
    BOOST_CHECK_EQUAL(d.type(1), "string");
    BOOST_CHECK_EQUAL(d.code("abc"), 1);
    BOOST_CHECK_EQUAL(d.code("bcd"), 1);
    BOOST_CHECK_EQUAL(d.type("abc"), "string");
    BOOST_CHECK_EQUAL(d.type("bcd"), "string");

//  2.1.  codes ==, names !=, types !=

    RadProto::BasicDictionary f;

    f.add(1, "abc", "string");

    BOOST_CHECK_EQUAL(f.name(1), "abc");
    BOOST_CHECK_EQUAL(f.type(1), "string");
    BOOST_CHECK_EQUAL(f.code("abc"), 1);
    BOOST_CHECK_EQUAL(f.type("abc"), "string");

    RadProto::BasicDictionary g;

    g.add(1, "bcd", "integer");

    BOOST_CHECK_EQUAL(g.name(1), "bcd");
    BOOST_CHECK_EQUAL(g.type(1), "integer");
    BOOST_CHECK_EQUAL(g.code("bcd"), 1);
    BOOST_CHECK_EQUAL(g.type("bcd"), "integer");

    f.append(g);

    BOOST_CHECK_EQUAL(f.name(1), "bcd");
    BOOST_CHECK_EQUAL(f.type(1), "integer");
    BOOST_CHECK_EQUAL(f.code("abc"), 1);
    BOOST_CHECK_EQUAL(f.code("bcd"), 1);
    BOOST_CHECK_EQUAL(f.type("abc"), "string");
    BOOST_CHECK_EQUAL(f.type("bcd"), "integer");


//  3.  codes ==, names ==, types ==

    RadProto::BasicDictionary h;

    h.add(1, "abc", "string");

    BOOST_CHECK_EQUAL(h.name(1), "abc");
    BOOST_CHECK_EQUAL(h.type(1), "string");
    BOOST_CHECK_EQUAL(h.code("abc"), 1);
    BOOST_CHECK_EQUAL(h.type("abc"), "string");

    RadProto::BasicDictionary i;

    i.add(1, "abc", "string");

    BOOST_CHECK_EQUAL(i.name(1), "abc");
    BOOST_CHECK_EQUAL(i.type(1), "string");
    BOOST_CHECK_EQUAL(i.code("abc"), 1);
    BOOST_CHECK_EQUAL(i.type("abc"), "string");

    h.append(i);

    BOOST_CHECK_EQUAL(h.name(1), "abc");
    BOOST_CHECK_EQUAL(h.type(1), "string");
    BOOST_CHECK_EQUAL(h.code("abc"), 1);
    BOOST_CHECK_EQUAL(h.type("abc"), "string");

//  3.1  codes ==, names ==, types !=

    RadProto::BasicDictionary j;

    j.add(1, "abc", "string");

    BOOST_CHECK_EQUAL(j.name(1), "abc");
    BOOST_CHECK_EQUAL(j.type(1), "string");
    BOOST_CHECK_EQUAL(j.code("abc"), 1);
    BOOST_CHECK_EQUAL(j.type("abc"), "string");

    RadProto::BasicDictionary k;

    k.add(1, "abc", "integer");

    BOOST_CHECK_EQUAL(k.name(1), "abc");
    BOOST_CHECK_EQUAL(k.type(1), "integer");
    BOOST_CHECK_EQUAL(k.code("abc"), 1);
    BOOST_CHECK_EQUAL(k.type("abc"), "integer");

    j.append(k);

    BOOST_CHECK_EQUAL(j.name(1), "abc");
    BOOST_CHECK_EQUAL(j.type(1), "integer");
    BOOST_CHECK_EQUAL(j.code("abc"), 1);
    BOOST_CHECK_EQUAL(j.type("abc"), "integer");
}

BOOST_AUTO_TEST_CASE(TestsFindByNameAndFindByCode)
{
    RadProto::BasicDictionary b;

    b.add(1, "abc", "string");

    BOOST_CHECK_EQUAL(b.findByName("abc"), true);
    BOOST_CHECK_EQUAL(b.findByName("def"), false);

    BOOST_CHECK_EQUAL(b.findByCode(1), true);
    BOOST_CHECK_EQUAL(b.findByCode(2), false);
}

BOOST_AUTO_TEST_CASE(TestConstructor)
{
    RadProto::BasicDictionary b;

    BOOST_CHECK_THROW(b.name(0), std::out_of_range);

    BOOST_CHECK_THROW(b.code(""), std::out_of_range);

//  Function add

//  1.  codes !=, names == - exception

    b.add(1, "abc", "string");
    BOOST_CHECK_THROW(b.add(2, "abc", "string"), RadProto::Exception);
    BOOST_CHECK_THROW(b.add(2, "abc", "integer"), RadProto::Exception);

    BOOST_CHECK_EQUAL(b.name(1), "abc");
    BOOST_CHECK_EQUAL(b.code("abc"), 1);
    BOOST_CHECK_EQUAL(b.type("abc"), "string");
    BOOST_CHECK_EQUAL(b.type(1), "string");

    BOOST_CHECK_THROW(b.name(2), std::out_of_range);
    BOOST_CHECK_THROW(b.type(2), std::out_of_range);


//  2.  codes ==, names !=, types ==

    b.add(3, "def", "string");
    b.add(3, "ghi", "string");

    BOOST_CHECK_EQUAL(b.name(3), "ghi");
    BOOST_CHECK_EQUAL(b.type(3), "string");

    BOOST_CHECK_EQUAL(b.code("def"), 3);
    BOOST_CHECK_EQUAL(b.code("ghi"), 3);

    BOOST_CHECK_EQUAL(b.type("def"), "string");
    BOOST_CHECK_EQUAL(b.type("ghi"), "string");

//  2.1.  codes ==, names !=, types !=

    b.add(4, "jkl", "string");
    b.add(4, "mno", "integer");

    BOOST_CHECK_EQUAL(b.name(4), "mno");
    BOOST_CHECK_EQUAL(b.type(4), "integer");

    BOOST_CHECK_EQUAL(b.code("jkl"), 4);
    BOOST_CHECK_EQUAL(b.code("mno"), 4);

    BOOST_CHECK_EQUAL(b.type("jkl"), "string");
    BOOST_CHECK_EQUAL(b.type("mno"), "integer");


//  3.  codes ==, names ==, types ==

    b.add(5, "cde", "integer");
    b.add(5, "cde", "integer");

    BOOST_CHECK_EQUAL(b.name(5), "cde");
    BOOST_CHECK_EQUAL(b.type(5), "integer");

    BOOST_CHECK_EQUAL(b.code("cde"), 5);

    BOOST_CHECK_EQUAL(b.type("cde"), "integer");

//  3.1.  codes ==, names ==, types !=

    b.add(6, "efg", "integer");
    b.add(6, "efg", "string");

    BOOST_CHECK_EQUAL(b.name(6), "efg");
    BOOST_CHECK_EQUAL(b.type(6), "string");

    BOOST_CHECK_EQUAL(b.code("efg"), 6);

    BOOST_CHECK_EQUAL(b.type("efg"), "string");


//  Function append

//  1.  codes !=, names == - exception

    RadProto::BasicDictionary a;

    a.add(1, "abc", "string");

    BOOST_CHECK_EQUAL(a.name(1), "abc");
    BOOST_CHECK_EQUAL(a.code("abc"), 1);
    BOOST_CHECK_EQUAL(a.type("abc"), "string");
    BOOST_CHECK_EQUAL(a.type(1), "string");

    RadProto::BasicDictionary p;

    p.add(2, "abc", "string");

    BOOST_CHECK_EQUAL(p.name(2), "abc");
    BOOST_CHECK_EQUAL(p.code("abc"), 2);
    BOOST_CHECK_EQUAL(p.type("abc"), "string");
    BOOST_CHECK_EQUAL(p.type(2), "string");

    BOOST_CHECK_THROW(a.append(p), RadProto::Exception);

    RadProto::BasicDictionary c;

    c.add(2, "abc", "integer");

    BOOST_CHECK_EQUAL(c.name(2), "abc");
    BOOST_CHECK_EQUAL(c.code("abc"), 2);
    BOOST_CHECK_EQUAL(c.type("abc"), "integer");
    BOOST_CHECK_EQUAL(c.type(2), "integer");

    BOOST_CHECK_THROW(a.append(c), RadProto::Exception);

    BOOST_CHECK_THROW(a.name(2), std::out_of_range);
    BOOST_CHECK_THROW(a.type(2), std::out_of_range);


//  2.  codes ==, names !=, types ==

    RadProto::BasicDictionary d;

    d.add(1, "abc", "string");

    BOOST_CHECK_EQUAL(d.name(1), "abc");
    BOOST_CHECK_EQUAL(d.type(1), "string");
    BOOST_CHECK_EQUAL(d.code("abc"), 1);
    BOOST_CHECK_EQUAL(d.type("abc"), "string");

    RadProto::BasicDictionary e;

    e.add(1, "bcd", "string");

    BOOST_CHECK_EQUAL(e.name(1), "bcd");
    BOOST_CHECK_EQUAL(e.type(1), "string");
    BOOST_CHECK_EQUAL(e.code("bcd"), 1);
    BOOST_CHECK_EQUAL(e.type("bcd"), "string");

    d.append(e);

    BOOST_CHECK_EQUAL(d.name(1), "bcd");
    BOOST_CHECK_EQUAL(d.type(1), "string");
    BOOST_CHECK_EQUAL(d.code("abc"), 1);
    BOOST_CHECK_EQUAL(d.code("bcd"), 1);
    BOOST_CHECK_EQUAL(d.type("abc"), "string");
    BOOST_CHECK_EQUAL(d.type("bcd"), "string");

//  2.1.  codes ==, names !=, types !=

    RadProto::BasicDictionary f;

    f.add(1, "abc", "string");

    BOOST_CHECK_EQUAL(f.name(1), "abc");
    BOOST_CHECK_EQUAL(f.type(1), "string");
    BOOST_CHECK_EQUAL(f.code("abc"), 1);
    BOOST_CHECK_EQUAL(f.type("abc"), "string");

    RadProto::BasicDictionary g;

    g.add(1, "bcd", "integer");

    BOOST_CHECK_EQUAL(g.name(1), "bcd");
    BOOST_CHECK_EQUAL(g.type(1), "integer");
    BOOST_CHECK_EQUAL(g.code("bcd"), 1);
    BOOST_CHECK_EQUAL(g.type("bcd"), "integer");

    f.append(g);

    BOOST_CHECK_EQUAL(f.name(1), "bcd");
    BOOST_CHECK_EQUAL(f.type(1), "integer");
    BOOST_CHECK_EQUAL(f.code("abc"), 1);
    BOOST_CHECK_EQUAL(f.code("bcd"), 1);
    BOOST_CHECK_EQUAL(f.type("abc"), "string");
    BOOST_CHECK_EQUAL(f.type("bcd"), "integer");


//  3.  codes ==, names ==, types ==

    RadProto::BasicDictionary h;

    h.add(1, "abc", "string");

    BOOST_CHECK_EQUAL(h.name(1), "abc");
    BOOST_CHECK_EQUAL(h.type(1), "string");
    BOOST_CHECK_EQUAL(h.code("abc"), 1);
    BOOST_CHECK_EQUAL(h.type("abc"), "string");

    RadProto::BasicDictionary i;

    i.add(1, "abc", "string");

    BOOST_CHECK_EQUAL(i.name(1), "abc");
    BOOST_CHECK_EQUAL(i.type(1), "string");
    BOOST_CHECK_EQUAL(i.code("abc"), 1);
    BOOST_CHECK_EQUAL(i.type("abc"), "string");

    h.append(i);

    BOOST_CHECK_EQUAL(h.name(1), "abc");
    BOOST_CHECK_EQUAL(h.type(1), "string");
    BOOST_CHECK_EQUAL(h.code("abc"), 1);
    BOOST_CHECK_EQUAL(h.type("abc"), "string");

//  3.1  codes ==, names ==, types !=

    RadProto::BasicDictionary j;

    j.add(1, "abc", "string");

    BOOST_CHECK_EQUAL(j.name(1), "abc");
    BOOST_CHECK_EQUAL(j.type(1), "string");
    BOOST_CHECK_EQUAL(j.code("abc"), 1);
    BOOST_CHECK_EQUAL(j.type("abc"), "string");

    RadProto::BasicDictionary k;

    k.add(1, "abc", "integer");

    BOOST_CHECK_EQUAL(k.name(1), "abc");
    BOOST_CHECK_EQUAL(k.type(1), "integer");
    BOOST_CHECK_EQUAL(k.code("abc"), 1);
    BOOST_CHECK_EQUAL(k.type("abc"), "integer");

    j.append(k);

    BOOST_CHECK_EQUAL(j.name(1), "abc");
    BOOST_CHECK_EQUAL(j.type(1), "integer");
    BOOST_CHECK_EQUAL(j.code("abc"), 1);
    BOOST_CHECK_EQUAL(j.type("abc"), "integer");
}
BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(VendorDictionaryTests)

BOOST_AUTO_TEST_CASE(TestAdd)
{
    RadProto::VendorDictionary b;

    b.add(1, "Vendor-Name");
    b.add(1, "Vendor");
    b.add(1, "abc");
    BOOST_CHECK_THROW(b.add(2, "abc"), RadProto::Exception);
    b.add(3, "def");
    b.add(1, "abc");

    BOOST_CHECK_EQUAL(b.name(1), "abc");
    BOOST_CHECK_EQUAL(b.name(3), "def");
    BOOST_CHECK_EQUAL(b.code("Vendor-Name"), 1);
    BOOST_CHECK_EQUAL(b.code("Vendor"), 1);
    BOOST_CHECK_EQUAL(b.code("abc"), 1);
    BOOST_CHECK_EQUAL(b.code("def"), 3);

    BOOST_CHECK_THROW(b.name(2), std::out_of_range);
}
BOOST_AUTO_TEST_CASE(TestAppend)
{
    RadProto::VendorDictionary a;

    a.add(2, "def");
    a.add(3, "ghi");
    a.add(4, "Vendor-Name");

    BOOST_CHECK_EQUAL(a.name(2), "def");
    BOOST_CHECK_EQUAL(a.name(3), "ghi");
    BOOST_CHECK_EQUAL(a.name(4), "Vendor-Name");
    BOOST_CHECK_EQUAL(a.code("def"), 2);
    BOOST_CHECK_EQUAL(a.code("ghi"), 3);
    BOOST_CHECK_EQUAL(a.code("Vendor-Name"), 4);

    RadProto::VendorDictionary b;

    b.add(1, "Vendor-Name");

    BOOST_CHECK_EQUAL(b.name(1), "Vendor-Name");
    BOOST_CHECK_EQUAL(b.code("Vendor-Name"), 1);

    BOOST_CHECK_THROW(a.append(b), RadProto::Exception);

    BOOST_CHECK_THROW(a.name(1), std::out_of_range);

    RadProto::VendorDictionary c;

    c.add(4, "Vendor");

    BOOST_CHECK_EQUAL(c.name(4), "Vendor");
    BOOST_CHECK_EQUAL(c.code("Vendor"), 4);

    a.append(c);

    BOOST_CHECK_EQUAL(a.name(2), "def");
    BOOST_CHECK_EQUAL(a.name(3), "ghi");
    BOOST_CHECK_EQUAL(a.name(4), "Vendor");
    BOOST_CHECK_EQUAL(a.code("def"), 2);
    BOOST_CHECK_EQUAL(a.code("ghi"), 3);
    BOOST_CHECK_EQUAL(a.code("Vendor"), 4);
    BOOST_CHECK_EQUAL(a.code("Vendor-Name"), 4);

    RadProto::VendorDictionary d;

    d.add(4, "jkl");

    BOOST_CHECK_EQUAL(d.name(4), "jkl");
    BOOST_CHECK_EQUAL(d.code("jkl"), 4);

    a.append(d);

    BOOST_CHECK_EQUAL(a.name(2), "def");
    BOOST_CHECK_EQUAL(a.name(3), "ghi");
    BOOST_CHECK_EQUAL(a.name(4), "jkl");
    BOOST_CHECK_EQUAL(a.code("def"), 2);
    BOOST_CHECK_EQUAL(a.code("ghi"), 3);
    BOOST_CHECK_EQUAL(a.code("Vendor"), 4);
    BOOST_CHECK_EQUAL(a.code("Vendor-Name"), 4);
    BOOST_CHECK_EQUAL(a.code("jkl"), 4);

    RadProto::VendorDictionary e;

    e.add(2, "def");

    BOOST_CHECK_EQUAL(e.name(2), "def");
    BOOST_CHECK_EQUAL(e.code("def"), 2);

    a.append(e);

    BOOST_CHECK_EQUAL(a.name(2), "def");
    BOOST_CHECK_EQUAL(a.name(3), "ghi");
    BOOST_CHECK_EQUAL(a.name(4), "jkl");
    BOOST_CHECK_EQUAL(a.code("def"), 2);
    BOOST_CHECK_EQUAL(a.code("ghi"), 3);
    BOOST_CHECK_EQUAL(a.code("Vendor"), 4);
    BOOST_CHECK_EQUAL(a.code("Vendor-Name"), 4);
    BOOST_CHECK_EQUAL(a.code("jkl"), 4);

    RadProto::VendorDictionary f;

    f.add(4, "Vendor-Name");

    BOOST_CHECK_EQUAL(f.name(4), "Vendor-Name");
    BOOST_CHECK_EQUAL(f.code("Vendor-Name"), 4);

    a.append(f);

    BOOST_CHECK_EQUAL(a.name(2), "def");
    BOOST_CHECK_EQUAL(a.name(3), "ghi");
    BOOST_CHECK_EQUAL(a.name(4), "Vendor-Name");
    BOOST_CHECK_EQUAL(a.code("def"), 2);
    BOOST_CHECK_EQUAL(a.code("ghi"), 3);
    BOOST_CHECK_EQUAL(a.code("Vendor-Name"), 4);
    BOOST_CHECK_EQUAL(a.code("Vendor"), 4);
    BOOST_CHECK_EQUAL(a.code("jkl"), 4);
}

BOOST_AUTO_TEST_CASE(TestsFindByNameAndFindByCode)
{
    RadProto::VendorDictionary b;

    b.add(1, "Vendor-Name");

    BOOST_CHECK_EQUAL(b.findByName("Vendor-Name"), true);
    BOOST_CHECK_EQUAL(b.findByName("Name"), false);

    BOOST_CHECK_EQUAL(b.findByCode(1), true);
    BOOST_CHECK_EQUAL(b.findByCode(2), false);
}

BOOST_AUTO_TEST_CASE(TestConstructor)
{
    RadProto::VendorDictionary b;

    BOOST_CHECK_THROW(b.name(0), std::out_of_range);

    BOOST_CHECK_THROW(b.code(""), std::out_of_range);

    b.add(1, "Vendor-Name");
    b.add(1, "Vendor");
    b.add(5, "ijk");
    b.add(3, "def");
    BOOST_CHECK_THROW(b.add(2, "Vendor"), RadProto::Exception);

    BOOST_CHECK_EQUAL(b.name(1), "Vendor");
    BOOST_CHECK_EQUAL(b.name(5), "ijk");
    BOOST_CHECK_EQUAL(b.name(3), "def");
    BOOST_CHECK_EQUAL(b.code("Vendor-Name"), 1);
    BOOST_CHECK_EQUAL(b.code("Vendor"), 1);
    BOOST_CHECK_EQUAL(b.code("ijk"), 5);
    BOOST_CHECK_EQUAL(b.code("def"), 3);
    BOOST_CHECK_THROW(b.name(2), std::out_of_range);

    RadProto::VendorDictionary c;

    c.add(2, "def");
    c.add(5, "ghi");

    BOOST_CHECK_EQUAL(c.name(2), "def");
    BOOST_CHECK_EQUAL(c.name(5), "ghi");
    BOOST_CHECK_EQUAL(c.code("def"), 2);
    BOOST_CHECK_EQUAL(c.code("ghi"), 5);

    BOOST_CHECK_THROW(c.append(b), RadProto::Exception);

    RadProto::VendorDictionary a;

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

BOOST_AUTO_TEST_CASE(TestsFindByNameAndFindByCode)
{
    RadProto::DependentDictionary b;

    b.add(2, "Framed-User", "Service-Type");

    BOOST_CHECK_EQUAL(b.findByName("Service-Type", "Framed-User"), true);
    BOOST_CHECK_EQUAL(b.findByName("Service-Type", "Name"), false);

    BOOST_CHECK_EQUAL(b.findByCode("Service-Type", 2), true);
    BOOST_CHECK_EQUAL(b.findByCode("Service-Type", 3), false);
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

BOOST_AUTO_TEST_CASE(TestAttributeType)
{
    RadProto::Dictionaries a("dictionary");

    BOOST_CHECK_EQUAL(a.attributeType(1), "string");
    BOOST_CHECK_EQUAL(a.attributeType("User-Name"), "string");

//  flag = "encypt=1"

    BOOST_CHECK_EQUAL(a.attributeType(2), "encrypted");
    BOOST_CHECK_EQUAL(a.attributeType("User-Password"), "encrypted");

//  flag = "def=2"

    BOOST_CHECK_EQUAL(a.attributeType(7), "bytes");
    BOOST_CHECK_EQUAL(a.attributeType("abc"), "bytes");
}

BOOST_AUTO_TEST_CASE(TestsAttributeFindByNameAndAttributeFindByCode)
{
    RadProto::Dictionaries a("dictionary");

    BOOST_CHECK_EQUAL(a.attributeFindByName("User-Name"), true);
    BOOST_CHECK_EQUAL(a.attributeFindByName("Name"), false);

    BOOST_CHECK_EQUAL(a.attributeFindByCode(1), true);
    BOOST_CHECK_EQUAL(a.attributeFindByCode(3), false);
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

BOOST_AUTO_TEST_CASE(TestsAttributeValueFindByNameAndAttributeValueFindByCode)
{
    RadProto::Dictionaries a("dictionary");

    BOOST_CHECK_EQUAL(a.attributeValueFindByName("Service-Type","Framed-User"), true);
    BOOST_CHECK_EQUAL(a.attributeValueFindByName("Service-Type", "Framed"), false);

    BOOST_CHECK_EQUAL(a.attributeValueFindByCode("Service-Type", 2), true);
    BOOST_CHECK_EQUAL(a.attributeValueFindByCode("Service-Type", 3), false);
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

BOOST_AUTO_TEST_CASE(TestsVendorFindByNameAndVendorByCode)
{
    RadProto::Dictionaries a("dictionary");

    BOOST_CHECK_EQUAL(a.vendorFindByName("Dlink"), true);
    BOOST_CHECK_EQUAL(a.vendorFindByName("Name"), false);

    BOOST_CHECK_EQUAL(a.vendorFindByCode(171), true);
    BOOST_CHECK_EQUAL(a.vendorFindByCode(160), false);
}

BOOST_AUTO_TEST_CASE(TestVendorAttributeName)
{
    RadProto::Dictionaries a("dictionary");

    BOOST_CHECK_EQUAL(a.vendorAttributeName("Dlink", 1), "Dlink-User-Level");
}

BOOST_AUTO_TEST_CASE(TestVendorAttributeCode)
{
    RadProto::Dictionaries a("dictionary");

    BOOST_CHECK_EQUAL(a.vendorAttributeCode("Dlink", "Dlink-VLAN-Name"), 10);
}

BOOST_AUTO_TEST_CASE(TestVendorAttributeValueName)
{
    RadProto::Dictionaries a("dictionary");

    BOOST_CHECK_EQUAL(a.vendorAttributeValueName("Dlink-User-Level", 1), "User-Legacy");
}

BOOST_AUTO_TEST_CASE(TestVendorAttributeValueCode)
{
    RadProto::Dictionaries a("dictionary");

    BOOST_CHECK_EQUAL(a.vendorAttributeValueCode("Dlink-User-Level", "User"), 3);
}

BOOST_AUTO_TEST_CASE(TestAppend)
{
    RadProto::Dictionaries a("dictionary.1");
    RadProto::Dictionaries b("dictionary.dlink");

    a.append(b);

    BOOST_CHECK_EQUAL(a.attributeName(1), "User-Name");
    BOOST_CHECK_EQUAL(a.attributeCode("User-Password"), 2);

    BOOST_CHECK_EQUAL(a.attributeValueName("Service-Type", 1), "Login-User");
    BOOST_CHECK_EQUAL(a.attributeValueCode("Service-Type", "Framed-User"), 2);

    BOOST_CHECK_EQUAL(a.vendorName(171), "Dlink");
    BOOST_CHECK_EQUAL(a.vendorCode("Dlink"), 171);

    BOOST_CHECK_EQUAL(a.vendorAttributeName("Dlink", 1), "Dlink-User-Level");
    BOOST_CHECK_EQUAL(a.vendorAttributeCode("Dlink", "Dlink-VLAN-Name"), 10);

    BOOST_CHECK_EQUAL(a.vendorAttributeValueName("Dlink-User-Level", 1), "User-Legacy");
    BOOST_CHECK_EQUAL(a.vendorAttributeValueCode("Dlink-User-Level", "User"), 3);
}

BOOST_AUTO_TEST_CASE(TestConstructor)
{
    RadProto::Dictionaries b("dictionary");

    BOOST_CHECK_THROW(b.attributeName(0), std::out_of_range);
    BOOST_CHECK_THROW(b.attributeCode(""), std::out_of_range);

    BOOST_CHECK_THROW(b.attributeValueName("", 0), std::out_of_range);
    BOOST_CHECK_THROW(b.attributeValueCode("", ""), std::out_of_range);

    BOOST_CHECK_THROW(b.vendorName(0), std::out_of_range);
    BOOST_CHECK_THROW(b.vendorCode(""), std::out_of_range);

    BOOST_CHECK_THROW(b.vendorAttributeName("", 0), std::out_of_range);
    BOOST_CHECK_THROW(b.vendorAttributeCode("", ""), std::out_of_range);

    BOOST_CHECK_THROW(b.vendorAttributeValueName("", 0), std::out_of_range);
    BOOST_CHECK_THROW(b.vendorAttributeValueCode("", ""), std::out_of_range);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
