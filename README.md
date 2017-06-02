A simple way to wrap a string at different line lengths, optionally with indents.

e.g.

```c++
std::cout << Column( "This is quite a long string" ).width( 8 ) << std::endl;
```

will output:

```
This is
quite a
long
string
```

Columns can be combined, too:

```c++
auto a = Column( "This is a load of text that should go on the left" )
            .width( 10 );
auto b = Column( "Here's some more strings that should be formatted to the right. "
                 "It's longer so there should be blanks on the left" )
            .width( 12 )
            .initialIndent( 2 );
            
                 
auto layout = a + Spacer( 4 ) + b;

std::cout << layout << std::endl;                 
```

gives you:

```
 This is a       Here's
 load of       some more
 text that     strings that
 should go     should be
 on the        formatted to
 left          the right.
               It's longer
               so there
               should be
               blanks on
               the left
```

Not bad!

You can also iterate the lines (which are generated lazily), both from `Column` and the combined `Columns`.

See the tests for more.