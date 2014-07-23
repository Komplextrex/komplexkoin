#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-KPX");
    BOOST_CHECK(GetBoolArg("-KPX"));
    BOOST_CHECK(GetBoolArg("-KPX", false));
    BOOST_CHECK(GetBoolArg("-KPX", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-KPXo"));
    BOOST_CHECK(!GetBoolArg("-KPXo", false));
    BOOST_CHECK(GetBoolArg("-KPXo", true));

    ResetArgs("-KPX=0");
    BOOST_CHECK(!GetBoolArg("-KPX"));
    BOOST_CHECK(!GetBoolArg("-KPX", false));
    BOOST_CHECK(!GetBoolArg("-KPX", true));

    ResetArgs("-KPX=1");
    BOOST_CHECK(GetBoolArg("-KPX"));
    BOOST_CHECK(GetBoolArg("-KPX", false));
    BOOST_CHECK(GetBoolArg("-KPX", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noKPX");
    BOOST_CHECK(!GetBoolArg("-KPX"));
    BOOST_CHECK(!GetBoolArg("-KPX", false));
    BOOST_CHECK(!GetBoolArg("-KPX", true));

    ResetArgs("-noKPX=1");
    BOOST_CHECK(!GetBoolArg("-KPX"));
    BOOST_CHECK(!GetBoolArg("-KPX", false));
    BOOST_CHECK(!GetBoolArg("-KPX", true));

    ResetArgs("-KPX -noKPX");  // -KPX should win
    BOOST_CHECK(GetBoolArg("-KPX"));
    BOOST_CHECK(GetBoolArg("-KPX", false));
    BOOST_CHECK(GetBoolArg("-KPX", true));

    ResetArgs("-KPX=1 -noKPX=1");  // -KPX should win
    BOOST_CHECK(GetBoolArg("-KPX"));
    BOOST_CHECK(GetBoolArg("-KPX", false));
    BOOST_CHECK(GetBoolArg("-KPX", true));

    ResetArgs("-KPX=0 -noKPX=0");  // -KPX should win
    BOOST_CHECK(!GetBoolArg("-KPX"));
    BOOST_CHECK(!GetBoolArg("-KPX", false));
    BOOST_CHECK(!GetBoolArg("-KPX", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--KPX=1");
    BOOST_CHECK(GetBoolArg("-KPX"));
    BOOST_CHECK(GetBoolArg("-KPX", false));
    BOOST_CHECK(GetBoolArg("-KPX", true));

    ResetArgs("--noKPX=1");
    BOOST_CHECK(!GetBoolArg("-KPX"));
    BOOST_CHECK(!GetBoolArg("-KPX", false));
    BOOST_CHECK(!GetBoolArg("-KPX", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-KPX", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-KPX", "eleven"), "eleven");

    ResetArgs("-KPX -bar");
    BOOST_CHECK_EQUAL(GetArg("-KPX", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-KPX", "eleven"), "");

    ResetArgs("-KPX=");
    BOOST_CHECK_EQUAL(GetArg("-KPX", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-KPX", "eleven"), "");

    ResetArgs("-KPX=11");
    BOOST_CHECK_EQUAL(GetArg("-KPX", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-KPX", "eleven"), "11");

    ResetArgs("-KPX=eleven");
    BOOST_CHECK_EQUAL(GetArg("-KPX", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-KPX", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-KPX", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-KPX", 0), 0);

    ResetArgs("-KPX -bar");
    BOOST_CHECK_EQUAL(GetArg("-KPX", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-KPX=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-KPX", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-KPX=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-KPX", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--KPX");
    BOOST_CHECK_EQUAL(GetBoolArg("-KPX"), true);

    ResetArgs("--KPX=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-KPX", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noKPX");
    BOOST_CHECK(!GetBoolArg("-KPX"));
    BOOST_CHECK(!GetBoolArg("-KPX", true));
    BOOST_CHECK(!GetBoolArg("-KPX", false));

    ResetArgs("-noKPX=1");
    BOOST_CHECK(!GetBoolArg("-KPX"));
    BOOST_CHECK(!GetBoolArg("-KPX", true));
    BOOST_CHECK(!GetBoolArg("-KPX", false));

    ResetArgs("-noKPX=0");
    BOOST_CHECK(GetBoolArg("-KPX"));
    BOOST_CHECK(GetBoolArg("-KPX", true));
    BOOST_CHECK(GetBoolArg("-KPX", false));

    ResetArgs("-KPX --noKPX");
    BOOST_CHECK(GetBoolArg("-KPX"));

    ResetArgs("-noKPX -KPX"); // KPX always wins:
    BOOST_CHECK(GetBoolArg("-KPX"));
}

BOOST_AUTO_TEST_SUITE_END()
