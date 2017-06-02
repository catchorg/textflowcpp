#include "TextFlow.hpp"

#include "catch.hpp"

using namespace TextFlow;

TEST_CASE( "short string" ) {
    Column col( "short string" );

    auto it = col.begin();
    auto itEnd = col.end();
    CHECK( *it == "short string" );
    CHECK( it != itEnd );
    it++;
    CHECK( it == itEnd );

    CHECK( col.toString() == "short string" );
}

TEST_CASE( "long string" ) {
    Column col( "The quick brown fox jumped over the lazy dog" );

    // Use various column widths
    SECTION("7") {
        col.width(7);
        CHECK( col.toString() ==
                   "The\n"
                   "quick\n"
                   "brown\n"
                   "fox\n"
                   "jumped\n"
                   "over\n"
                   "the\n"
                   "lazy\n"
                   "dog" );
    }
    SECTION("8") {
        col.width(8);
        CHECK( col.toString() ==
                   "The\n"
                   "quick\n"
                   "brown\n"
                   "fox\n"
                   "jumped\n"
                   "over the\n"
                   "lazy dog" );
    }
    SECTION("9") {
        col.width(9);
        CHECK( col.toString() ==
                   "The quick\n"
                   "brown fox\n"
                   "jumped\n"
                   "over the\n"
                   "lazy dog" );
    }
    SECTION("10") {
        col.width(10);
        CHECK( col.toString() ==
                   "The quick\n"
                   "brown fox\n"
                   "jumped\n"
                   "over the\n"
                   "lazy dog" );
    }
    SECTION("11") {
        col.width(11);
        CHECK( col.toString() ==
                   "The quick\n"
                   "brown fox\n"
                   "jumped over\n"
                   "the lazy\n"
                   "dog" );
    }
    SECTION("12") {
        col.width(12);
        CHECK( col.toString() ==
                   "The quick\n"
                   "brown fox\n"
                   "jumped over\n"
                   "the lazy dog" );
    }
    SECTION("20") {
        col.width(20);
        CHECK( col.toString() ==
                   "The quick brown fox\n"
                   "jumped over the lazy\n"
                   "dog" );
    }
}
TEST_CASE( "edge cases" ) {
    SECTION( "empty string" )
        CHECK( Column("").toString() == "" );
    SECTION( "space" )
        CHECK( Column(" ").toString() == " " );
    SECTION( "char with trailing space" )
        CHECK( Column("a ").toString() == "a " );
    SECTION( "long string with trailing space" )
        CHECK( Column("once upon a time ").width(10).toString() == "once upon\na time " );

// can't check this as it's an assert, currently
//    SECTION( "zero width" )
//        CHECK_THROWS( Column("hello").width(0).toString() );
}

TEST_CASE( "unbreakable" ) {
    auto col = Column("unbreakable").width(8);
    CHECK( col.toString() == "unbreak-\nable");
}

TEST_CASE( "wrap points" ) {
    auto col = Column( "(hello)aaa(world)" );

    SECTION( "round brackets" ) {
        SECTION("8")
            CHECK(col.width(8).toString() == "(hello)\naaa\n(world)");
        SECTION("10")
            CHECK(col.width(10).toString() == "(hello)aaa\n(world)");
        SECTION("11")
            CHECK(col.width(11).toString() == "(hello)aaa\n(world)");
        SECTION("18")
            CHECK(col.width(18).toString() == "(hello)aaa(world)");
    }

    // These tests come from the original Catch tests
    SECTION( "Mixed" ) {
        Column col( "one,two(three) <here>" );

        SECTION("Wrap before") {
            CHECK( col.width(11).toString() == "one,two\n(three)\n<here>" );
        }
        SECTION("Wrap after") {
            CHECK( col.width(6).toString() == "one,\ntwo\n(thre-\ne)\n<here>" );
            CHECK( col.width(5).toString() == "one,\ntwo\n(thr-\nee)\n<her-\ne>");
            CHECK( col.width(4).toString() == "one,\ntwo\n(th-\nree)\n<he-\nre>");
        }
    }
}
