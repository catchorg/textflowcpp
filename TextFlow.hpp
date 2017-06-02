//
// Created by Phil Nash on 02/06/2017.
//
#include <cassert>
#include <ostream>
#include <sstream>
#include <vector>

#ifndef TEXTFLOW_HPP_INCLUDED
#define TEXTFLOW_HPP_INCLUDED

namespace TextFlow {

    inline auto isWhitespace( char c ) -> bool {
        static std::string chars = " \t\n\r";
        return chars.find( c ) != std::string::npos;
    }
    inline auto isBreakableBefore( char c ) -> bool {
        static std::string chars = "[({<|";
        return chars.find( c ) != std::string::npos;
    }
    inline auto isBreakableAfter( char c ) -> bool {
        static std::string chars = "])}>.,:;*+-=&/\\";
        return chars.find( c ) != std::string::npos;
    }

    class Column {
        std::vector<std::string> m_strings;
        size_t m_width = 80;

        class Iterator {
            friend Column;

            Column const& m_column;
            size_t m_stringIndex = 0;
            size_t m_pos = 0;

            size_t m_len;
            bool m_suffix = false;

            Iterator( Column const& column, size_t stringIndex )
            :   m_column( column ),
                m_stringIndex( stringIndex )
            {}

            auto line() const -> std::string const& { return m_column.m_strings[m_stringIndex]; }

            auto isBoundary( size_t at ) const -> bool {
                assert( at > 0 );
                assert( at <= line().size() );

                return at == line().size() ||
                       ( isWhitespace( line()[at] ) && !isWhitespace( line()[at-1] ) ) ||
                       isBreakableBefore( line()[at] ) ||
                       isBreakableAfter( line()[at-1] );
            }

            void calcLength() {
                assert( m_stringIndex < m_column.m_strings.size() );

                m_suffix = false;
                auto width = m_column.m_width;
                if( line().size() < m_pos + width ) {
                    m_len = line().size() - m_pos;
                }
                else {
                    size_t len = width;
                    while (len > 0 && !isBoundary(m_pos + len))
                        --len;
                    while (len > 0 && isWhitespace( line()[m_pos + len - 1] ))
                        --len;

                    if (len > 0) {
                        m_len = len;
                    } else {
                        m_suffix = true;
                        m_len = width - 1;
                    }
                }
            }

            auto addSuffix(std::string const &unsuffixed) const -> std::string {
                return m_suffix ? unsuffixed + "-" : unsuffixed;
            }

        public:
            Iterator( Column const& column ) : m_column( column ) {
                calcLength();
                if( m_len == 0 )
                    m_stringIndex++; // Empty string
            }

            auto operator *() const -> std::string {
                assert( m_stringIndex < m_column.m_strings.size() );
                assert( m_pos < line().size() );
                if( m_pos + m_column.m_width < line().size() )
                    return addSuffix( line().substr(m_pos, m_len) );
                else
                    return addSuffix( line().substr(m_pos) );
            }

            auto operator ++() -> Iterator& {
                m_pos += m_len;
                while( m_pos < line().size() && isWhitespace( line()[m_pos] ) )
                    ++m_pos;

                if( m_pos == line().size() ) {
                    m_pos = 0;
                    ++m_stringIndex;
                }
                if( m_stringIndex < m_column.m_strings.size() )
                    calcLength();
                return *this;
            }
            auto operator ++(int) -> Iterator {
                Iterator prev( *this );
                operator++();
                return prev;
            }

            auto operator ==( Iterator const& other ) -> bool {
                return
                    m_pos == other.m_pos &&
                    m_stringIndex == other.m_stringIndex &&
                    &m_column == &other.m_column;
            }
            auto operator !=( Iterator const& other ) -> bool {
                return !operator==( other );
            }
        };

    public:
        Column( std::string const& text ) { m_strings.push_back( text ); }

        auto width( size_t newWidth ) -> Column& {
            assert( newWidth > 0 );
            m_width = newWidth;
            return *this;
        }

        auto begin() const -> Iterator { return Iterator( *this ); }
        auto end() const -> Iterator { return Iterator( *this, m_strings.size() ); }

        inline friend std::ostream& operator << ( std::ostream& os, Column const& col ) {
            bool first = true;
            for( auto line : col ) {
                if( first )
                    first = false;
                else
                    os << "\n";
                os <<  line;
            }
            return os;
        }

        auto toString() const -> std::string {
            std::ostringstream oss;
            oss << *this;
            return oss.str();
        }
    };
}

#endif // TEXTFLOW_HPP_INCLUDED
