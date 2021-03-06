#include "catch.hpp"
#include "gubg/hash/MD5.hpp"
#include "gubg/debug.hpp"
using namespace gubg::hash;

namespace 
{
    auto logns = "test";

    bool hash(md5::Stream &md5, const std::string &message, const std::string &expected)
    {
		S(logns);
        md5.clear();
        md5 << message;
        const auto hash = md5.hash();
        const auto h1 = hash.to_hex();
        const auto h2 = md5.hash_hex();
        L("Hash: " << h1);
        return expected == h1 && h1 == h2;
    }
}

TEST_CASE("hash::MD5", "[ut]")
{
    md5::Stream md5;

    REQUIRE(hash(md5, "", "d41d8cd98f00b204e9800998ecf8427e"));
    REQUIRE(hash(md5, "The quick brown fox jumps over the lazy dog", "9e107d9d372bb6826bd81d3542a419d6"));
    REQUIRE(hash(md5, "The quick brown fox jumps over the lazy dog.", "e4d909c290d0fb1ca068ffaddf22cbd0"));
    REQUIRE(hash(md5, "aaa", "47bce5c74f589f4867dbd57e9ca9f808"));
    REQUIRE(hash(md5, std::string(622, 'a')+'\xa', "ad09e2739607c40e36fb75450263276f"));

    md5.clear();
    md5 << "aaa";
    auto h1 = md5.hash();
    md5.clear();
    md5 << "a" << "a" << "a";
    auto h2 = md5.hash();
    REQUIRE(h1 == h2);

    //Copying the MD5 state
    md5::Stream md5_1(md5);
    auto h3 = md5_1.hash();
    REQUIRE(h1 == h3);
}
