#include <random>
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

// can't check these as they assert, currently

//    SECTION( "indent > width" )
//        CHECK_THROWS( Column("test").width(10).indent(10).toString() );
//    SECTION( "initialIndent > width" )
//        CHECK_THROWS( Column("test").width(10).initialIndent(10).toString() );

//    SECTION( "zero width" )
//        CHECK_THROWS( Column("hello").width(0).toString() );
}

TEST_CASE( "unbreakable" ) {
    auto col = Column("unbreakable").width(8);
    CHECK( col.toString() == "unbreak-\nable");
}

TEST_CASE( "wrap points" ) {

    SECTION( "round brackets" ) {
        auto col = Column( "(hello)aaa(world)" );

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

auto toVector ( Column const& col ) -> std::vector<std::string> {
    std::vector<std::string> lines;
    std::copy( col.begin(), col.end(), std::back_inserter( lines ) );
    return lines;
}
auto toVector ( Columns const& cols ) -> std::vector<std::string> {
    std::vector<std::string> lines;
    std::copy( cols.begin(), cols.end(), std::back_inserter( lines ) );
    return lines;
}

TEST_CASE( "indents" ) {
    auto col = Column(
            "It is a period of civil war. "
            "Rebel spaceships, striking from a hidden base, have won their first victory against the evil Galactic Empire. "
            "During the battle, Rebel spies managed to steal secret plans to the Empire’s ultimate weapon, the DEATH STAR, an armored space station with enough power to destroy an entire planet. \n"
            "Pursued by the Empire’s sinister agents, Princess Leia races home aboard her starship, custodian of the stolen plans that can save her people and restore freedom to the galaxy..."
        ).width(40);

    SECTION( "no indent" ) {
        auto lines = toVector(col);

        REQUIRE(lines[0] == "It is a period of civil war. Rebel");
        REQUIRE(lines[1] == "spaceships, striking from a hidden base,");
        REQUIRE(lines[2] == "have won their first victory against the");
        REQUIRE(lines[3] == "evil Galactic Empire. During the battle,");
    }

    SECTION( "indent only" ) {
        col.indent( 4 );
        auto lines = toVector(col);

        //                   0123456789012345678901234567890123456789
        REQUIRE(lines[0] == "    It is a period of civil war. Rebel");
        REQUIRE(lines[1] == "    spaceships, striking from a hidden");
        REQUIRE(lines[2] == "    base, have won their first victory");
        REQUIRE(lines[3] == "    against the evil Galactic Empire." );
    }
    SECTION( "initial indent only" ) {
        col.initialIndent( 8 );
        auto lines = toVector(col);

        //                   0123456789012345678901234567890123456789
        REQUIRE(lines[0] == "        It is a period of civil war.");
        REQUIRE(lines[1] == "Rebel spaceships, striking from a hidden");
        REQUIRE(lines[2] == "base, have won their first victory");
        REQUIRE(lines[3] == "against the evil Galactic Empire. During" );
    }
    SECTION( "initial indent > indent" ) {
        col.initialIndent( 8 ).indent( 4 );
        auto lines = toVector(col);

        //                   0123456789012345678901234567890123456789
        REQUIRE(lines[0] == "        It is a period of civil war.");
        REQUIRE(lines[1] == "    Rebel spaceships, striking from a");
        REQUIRE(lines[2] == "    hidden base, have won their first");
        REQUIRE(lines[3] == "    victory against the evil Galactic" );
    }
    SECTION( "initial indent < indent" ) {
        col.initialIndent( 4 ).indent( 8 );
        auto lines = toVector(col);

        //                   0123456789012345678901234567890123456789
        REQUIRE(lines[0] == "    It is a period of civil war. Rebel");
        REQUIRE(lines[1] == "        spaceships, striking from a");
        REQUIRE(lines[2] == "        hidden base, have won their");
        REQUIRE(lines[3] == "        first victory against the evil" );
    }
}

TEST_CASE( "combined columns" ) {

    auto a = Column( "This is a load of text that should go on the left" ).width(10);
    auto b = Column( "Here's some more strings that should be formatted to the right. "
                     "It's longer so there should be blanks on the left" ).width(12);

    auto layout = a + Spacer(4) + b;


    auto lines = toVector( layout );
    CHECK( lines[0] == "This is a     Here's some" );
    CHECK( lines[1] == "load of       more strings" );
    CHECK( lines[2] == "text that     that should" );
    CHECK( lines[6] == "              longer so" );

    CHECK( layout.toString() ==
           "This is a     Here's some\n"
           "load of       more strings\n"
           "text that     that should\n"
           "should go     be formatted\n"
           "on the        to the\n"
           "left          right. It's\n"
           "              longer so\n"
           "              there should\n"
           "              be blanks on\n"
           "              the left" );

}

TEST_CASE( "indent at existing newlines" ) {
    auto col = Column( "This text has\n  newlines\nembedded in it - but also some long text that should be wrapped" )
        .width(20)
        .indent(2);

    REQUIRE( col.toString() ==
            "  This text has\n"
            "    newlines\n"
            "  embedded in it -\n"
            "  but also some long\n"
            "  text that should\n"
            "  be wrapped" );
}

TEST_CASE( "another long string" ) {
    // This test is taken from @JoeyGrajciar's PR against the Clara repo:

    // https://github.com/catchorg/Clara/pull/74

    const auto long_string = std::string(
        "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Pellentesque nisl \n"
        "massa, luctus ut ligula vitae, suscipit tempus velit. Vivamus sodales, quam in \n"
        "convallis posuere, libero nisi ultricies orci, nec lobortis.\n");

    auto col = Column( long_string )
        .width(79)
        .indent(2);

    REQUIRE( col.toString() ==
        "  Lorem ipsum dolor sit amet, consectetur adipiscing elit. Pellentesque nisl \n"
        "  massa, luctus ut ligula vitae, suscipit tempus velit. Vivamus sodales, quam\n"
        "  in \n"
        "  convallis posuere, libero nisi ultricies orci, nec lobortis." );
}

TEST_CASE( "message with linebreak at start" )
{
    const auto message = std::string(
        "\nthis is a message starting with linebreak"
    );

    auto col = Column( message )
        .width(79)
        .indent(2);

    REQUIRE(col.toString() ==
        "  \n"
        "this is a message starting with linebreak"
    );
}

std::mt19937 rng;
std::uniform_int_distribution<std::mt19937::result_type> wordCharGenerator(33,126);
std::uniform_int_distribution<std::mt19937::result_type> wsGenerator(0, 11);
std::uniform_int_distribution<std::mt19937::result_type> wordLenGenerator(1, 10);
std::uniform_int_distribution<std::mt19937::result_type> wsLenGenerator(1, 2);
std::uniform_int_distribution<std::mt19937::result_type> punctGenerator(0, 40);

auto randomWordChar() -> char {
    return static_cast<char>( wordCharGenerator(rng) );
}
auto randomWS() -> char {
    static char const* ws = "         \t\r\n";
    return ( ws[wsGenerator(rng)] );
}
auto generateWord( int length ) -> std::string {
    std::string s;
    for(; length > 0; --length )
        s += randomWordChar();
    return s;
}
auto generateWord() -> std::string {
    return generateWord( wordLenGenerator(rng) );
}
auto generateWS( int length ) -> std::string {
    std::string s;
    for(; length > 0; --length )
        s += randomWS();
    return s;
}
auto generateText( int words ) -> std::string {
    std::string text;
    for(; words > 0; --words ) {
        text += generateWord();
        switch( punctGenerator(rng) )
        {
            case 1:
            case 2:
                text += ",";
                break;
            case 3:
            case 4:
                text += ".";
                break;
        }
        text += generateWS( wsLenGenerator(rng) );
    }
    return text;
}

TEST_CASE( "randomly generated text", "[.]" ) {
    for( int j = 0; j < 5; ++j ) {
        for(int i = 1; i < 200; ++i ) {
            auto s = generateText( i );
            auto size = s.size();
            for( int c = 3; c < s.size(); ++c ) {
                CAPTURE( j );
                CAPTURE( i );
                CAPTURE( c );
                CAPTURE( s );
                INFO( Column( s ).width(c).toString() );
            }
        }
    }
    SUCCEED();
}
