/*! \file
    \brief Константы классов символов (C++). При помощи классов символов можно просто писать простейшие лексера
 */
#pragma once

//
#include "umba/debug_helpers.h"
//
#include "umba/assert.h"
#include "umba/string_plus.h"
//
#include "c_char_class.h"
//

#include <array>
#include <cstdint>
#include <climits>
#include <initializer_list>
#include <type_traits>
#include <string>
#include <set>
#include <unordered_set>
#include <iostream>
#include <iomanip>
#include <exception>
#include <stdexcept>
#include <vector>
#include <utility>




/*

// !!! Do not remove this comment, it is not "dead" code

// CharClass table generator sample

#include "umba/umba.h"
#include "umba/tokenizer.h"

int main(int argc, char* argv[])
{

    umba::tokenizer::CharClass charClasses[128] = { umba::tokenizer::CharClass::none };
    umba::tokenizer::generation::generateCharClassTable(charClasses);

    umba::tokenizer::generation::CommentType  commentType  = umba::tokenizer::generation::CommentType ::cppLike;
    umba::tokenizer::generation::ArrayType    arrayType    = umba::tokenizer::generation::ArrayType   ::stdArray;
    umba::tokenizer::generation::TypeNameType typeNameType = umba::tokenizer::generation::TypeNameType::cppLike;

    if (argc>1)
    {
        commentType  = umba::tokenizer::generation::CommentType ::cLike;
        arrayType    = umba::tokenizer::generation::ArrayType   ::cLike;
        typeNameType = umba::tokenizer::generation::TypeNameType::cLike;
    }

    umba::tokenizer::generation::printCharClassArray( charClasses, commentType, arrayType, typeNameType);  // , const std::string &varName="charClassesTable"

    return 0;
}

*/


#if defined(UMBA_TOKENIZER_TYPES_COMPACT)

    #if !defined(UMBA_CHAR_CLASS_UNDERLYING_COMPACT)
        #define UMBA_CHAR_CLASS_UNDERLYING_COMPACT
    #endif

#endif



// umba::tokenizer::
namespace umba {
namespace tokenizer {


// #define UMBA_CHAR_CLASS_UNDERLYING_COMPACT

#if defined(UMBA_CHAR_CLASS_UNDERLYING_COMPACT)

    using char_class_underlying_uint_t = std::uint_least16_t;

#else

    using char_class_underlying_uint_t = std::uint_fast32_t;

#endif


constexpr
std::size_t charToCharClassTableIndex(char ch)
{
    return (((std::size_t)(std::uint_least8_t)ch) < 0x80) ? (std::size_t)(std::uint_least8_t)ch : 0x7Fu; // Всё, что больше или равно 0x80, превращаем в непечатный символ DEL (0x7Fu)
}

constexpr
std::size_t charToCharClassTableIndex(std::uint8_t ch)
{
    return (((std::size_t)(std::uint_least8_t)ch) < 0x80) ? (std::size_t)(std::uint_least8_t)ch : 0x7Fu; // Всё, что больше или равно 0x80, превращаем в непечатный символ DEL (0x7Fu)
}


/*

Классы символов
Символ может входить в несколько классов, например '<'/'>' - скобка и операторный символ

nonprintable     - all less to space
space            - all less or eq to space
quot             - ` ' " (backtik apos quot)
brace            - {}()[]<>
open             - {([<
close            - })]>
operator         - ! $ % & * + , - . / : ; < = > ? @ \ ^ | ~
digit            - 0-9
alpha            - A-Za-z
identifier       - _A-Za-z0-9
identifier-first - _A-Za-z
hyphen           - -
underscore       - _

$ - alpha и operator
@ - alpha и operator

*/

//----------------------------------------------------------------------------
enum class CharClass : char_class_underlying_uint_t
{
    // unknown       = 1,
    none                       = UMBA_TOKENIZER_CHARCLASS_NONE            ,
    string_literal_prefix      = UMBA_TOKENIZER_CHARCLASS_STRING_LITERAL_PREFIX,
    nonprintable               = UMBA_TOKENIZER_CHARCLASS_NONPRINTABLE    ,
    linefeed                   = UMBA_TOKENIZER_CHARCLASS_LINEFEED        ,
    space                      = UMBA_TOKENIZER_CHARCLASS_SPACE           ,
    open                       = UMBA_TOKENIZER_CHARCLASS_OPEN            , // Флаг для парных символов
    close                      = UMBA_TOKENIZER_CHARCLASS_CLOSE           , // Флаг для парных символов
    xdigit                     = UMBA_TOKENIZER_CHARCLASS_XDIGIT          ,
    opchar                     = UMBA_TOKENIZER_CHARCLASS_OPCHAR          ,
    punctuation                = UMBA_TOKENIZER_CHARCLASS_PUNCTUATION     ,
    digit                      = UMBA_TOKENIZER_CHARCLASS_DIGIT           ,
    alpha                      = UMBA_TOKENIZER_CHARCLASS_ALPHA           ,
    upper                      = UMBA_TOKENIZER_CHARCLASS_UPPER           , // Флаг для символов верхнего регистра
    identifier                 = UMBA_TOKENIZER_CHARCLASS_IDENTIFIER      ,
    identifier_first           = UMBA_TOKENIZER_CHARCLASS_IDENTIFIER_FIRST,
    semialpha                  = UMBA_TOKENIZER_CHARCLASS_SEMIALPHA       , // Для символов, которые никуда не вошли, такие как @ # $
    escape                     = UMBA_TOKENIZER_CHARCLASS_ESCAPE            // Для символа '\', который везде используется как escape-символ
};

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
using CharClassUnderlyingType = typename std::underlying_type<CharClass>::type;

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
inline constexpr CharClass operator~(CharClass a)
{
    using TUnder = typename std::underlying_type<CharClass>::type;
    return static_cast<CharClass>(~static_cast<TUnder>(a));
}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
inline constexpr CharClass operator|(CharClass a, CharClass b)
{
    using TUnder = typename std::underlying_type<CharClass>::type;
    return static_cast<CharClass>(static_cast<TUnder>(a) | static_cast<TUnder>(b));
}

//----------------------------------------------------------------------------
inline constexpr CharClass operator&(CharClass a, CharClass b)
{
    using TUnder = typename std::underlying_type<CharClass>::type;
    return static_cast<CharClass>(static_cast<TUnder>(a) & static_cast<TUnder>(b));
}

//----------------------------------------------------------------------------
inline constexpr CharClass operator^(CharClass a, CharClass b)
{
    using TUnder = typename std::underlying_type<CharClass>::type;
    return static_cast<CharClass>(static_cast<TUnder>(a) ^ static_cast<TUnder>(b));
}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
inline constexpr CharClass& operator|=(CharClass& a, CharClass b)
{
    using TUnder = typename std::underlying_type<CharClass>::type;
    a = static_cast<CharClass>(static_cast<TUnder>(a) | static_cast<TUnder>(b));
    return a;
}

//----------------------------------------------------------------------------
inline constexpr CharClass& operator&=(CharClass& a, CharClass b)
{
    using TUnder = typename std::underlying_type<CharClass>::type;
    a = static_cast<CharClass>(static_cast<TUnder>(a) & static_cast<TUnder>(b));
    return a;
}

//----------------------------------------------------------------------------
inline constexpr CharClass& operator^=(CharClass& a, CharClass b)
{
    using TUnder = typename std::underlying_type<CharClass>::type;
    a = static_cast<CharClass>(static_cast<TUnder>(a) ^ static_cast<TUnder>(b));
    return a;
}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
inline constexpr bool operator==(CharClass a, std::underlying_type<CharClass>::type b)
{
    using TUnder = typename std::underlying_type<CharClass>::type;
    return static_cast<TUnder>(a) == b;
}

//----------------------------------------------------------------------------
inline constexpr bool operator==(std::underlying_type<CharClass>::type a, CharClass b)
{
    using TUnder = typename std::underlying_type<CharClass>::type;
    return a == static_cast<TUnder>(b);
}

//----------------------------------------------------------------------------
inline constexpr bool operator!=(CharClass a, std::underlying_type<CharClass>::type b)
{
    using TUnder = typename std::underlying_type<CharClass>::type;
    return static_cast<TUnder>(a) != b;
}

//----------------------------------------------------------------------------
inline constexpr bool operator!=(std::underlying_type<CharClass>::type a, CharClass b)
{
    using TUnder = typename std::underlying_type<CharClass>::type;
    return a != static_cast<TUnder>(b);
}

//----------------------------------------------------------------------------



#if !defined(UMBA_TOKENIZER_DISABLE_TYPES_META)
//----------------------------------------------------------------------------
template<typename StringType>
StringType enum_serialize_single_flag(CharClass f, const StringType &prefix=StringType())
{
    switch(f)
    {
        case CharClass::none                        : return std::string("none");
        case CharClass::string_literal_prefix       : return prefix+umba::string_plus::make_string<StringType>("string_literal_prefix");
        case CharClass::nonprintable                : return prefix+umba::string_plus::make_string<StringType>("nonprintable");
        case CharClass::linefeed                    : return prefix+umba::string_plus::make_string<StringType>("linefeed");
        case CharClass::space                       : return prefix+umba::string_plus::make_string<StringType>("space");
        //case CharClass::tab             : return prefix+umba::string_plus::make_string<StringType>("tab");
        case CharClass::xdigit                      : return prefix+umba::string_plus::make_string<StringType>("xdigit");
        //case CharClass::brace           : return prefix+umba::string_plus::make_string<StringType>("brace");
        case CharClass::open                        : return prefix+umba::string_plus::make_string<StringType>("open");
        case CharClass::close                       : return prefix+umba::string_plus::make_string<StringType>("close");
        case CharClass::opchar                      : return prefix+umba::string_plus::make_string<StringType>("opchar");
        //case CharClass::operator_char   : return prefix+umba::string_plus::make_string<StringType>("operator_char");
        case CharClass::punctuation                 : return prefix+umba::string_plus::make_string<StringType>("punctuation");
        case CharClass::digit                       : return prefix+umba::string_plus::make_string<StringType>("digit");
        case CharClass::alpha                       : return prefix+umba::string_plus::make_string<StringType>("alpha");
        case CharClass::upper                       : return prefix+umba::string_plus::make_string<StringType>("upper");
        case CharClass::identifier                  : return prefix+umba::string_plus::make_string<StringType>("identifier");
        case CharClass::identifier_first            : return prefix+umba::string_plus::make_string<StringType>("identifier_first");
        case CharClass::semialpha                   : return prefix+umba::string_plus::make_string<StringType>("semialpha");
        case CharClass::escape                      : return prefix+umba::string_plus::make_string<StringType>("escape");
        default: return StringType("");
    }
}

//----------------------------------------------------------------------------
template<typename StringType>
StringType enum_serialize_flags(CharClass f, const StringType &prefix=std::string())
{
    char_class_underlying_uint_t fu = (char_class_underlying_uint_t)f;

    std::string res;

    char_class_underlying_uint_t flagBit = 1;
    for(; flagBit!=0; flagBit<<=1)
    {
        if (fu&flagBit)
        {
            std::string flagStr = enum_serialize_single_flag<StringType>((CharClass)flagBit, prefix);
            if (!flagStr.empty())
            {
                if (!res.empty())
                {
                    res.append(1,'|');
                }

                res.append(flagStr);
            }
        }
    }

    if (res.empty())
    {
        res = "0";
    }

    return res;
}

//----------------------------------------------------------------------------
inline
std::string enum_serialize(CharClass f)
{
    return enum_serialize_flags<std::string>(f);
}

//----------------------------------------------------------------------------
#endif

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
// umba::tokenizer::generation::
namespace generation {



//----------------------------------------------------------------------------
template< std::size_t N, typename CharType >
void setCharClassFlags( umba::tokenizer::CharClass (&charClasses)[N], CharType ch, umba::tokenizer::CharClass setClasses)
{
    std::size_t idx = umba::tokenizer::charToCharClassTableIndex((char)ch);
    UMBA_ASSERT(idx<N);
    // if (idx>=N)
    // {
    //     std::cerr << "Bad index" << std::endl;
    //     throw std::runtime_error("Bad index");
    // }

    charClasses[idx] |= setClasses;
}

template< std::size_t N, typename CharType >
void setCharClassFlags( umba::tokenizer::CharClass (&charClasses)[N], const std::basic_string<CharType> &chars, umba::tokenizer::CharClass setClasses)
{
    for(auto ch : chars)
    {
        setCharClassFlags(charClasses, ch, setClasses);
    }
}

template< std::size_t N, typename CharType >
void setCharClassFlags( umba::tokenizer::CharClass (&charClasses)[N], CharType ch1, CharType ch2, umba::tokenizer::CharClass setClasses)
{
    for(auto ch=ch1; ch<=ch2; ++ch)
    {
        setCharClassFlags(charClasses, ch, setClasses);
    }
}

template< std::size_t N, typename CharType >
void setCharClassFlagsForBracePair( umba::tokenizer::CharClass (&charClasses)[N], const std::basic_string<CharType> &braceChars)
{
    UMBA_ASSERT(braceChars.size()==2);
    // if (braceChars.size()!=2)
    // {
    //     std::cerr << "Braces def invalid size" << std::endl;
    //     throw std::runtime_error("Braces def invalid size");
    // }

    setCharClassFlags(charClasses, braceChars[0], umba::tokenizer::CharClass::open );
    setCharClassFlags(charClasses, braceChars[1], umba::tokenizer::CharClass::close);
}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
template< std::size_t N, typename CharType >
void setCharClassFlags( std::array<umba::tokenizer::CharClass, N> &charClasses, CharType ch, umba::tokenizer::CharClass setClasses)
{
    std::size_t idx = umba::tokenizer::charToCharClassTableIndex((char)ch);
    UMBA_ASSERT(idx<N);
    // if (idx>=charClasses.size())
    // {
    //     std::cerr << "Bad index" << std::endl;
    //     throw std::runtime_error("Bad index");
    // }

    charClasses[idx] |= setClasses;
}

template< std::size_t N, typename CharType >
void setCharClassFlags( std::array<umba::tokenizer::CharClass, N> &charClasses, const std::basic_string<CharType> &chars, umba::tokenizer::CharClass setClasses)
{
    for(auto ch : chars)
    {
        setCharClassFlags(charClasses, ch, setClasses);
    }
}

#include "umba/warnings/push_disable_spectre_mitigation.h"
template< std::size_t N, typename CharType >
void setCharClassFlags( std::array<umba::tokenizer::CharClass, N> &charClasses, CharType ch1, CharType ch2, umba::tokenizer::CharClass setClasses)
{
    for(auto ch=ch1; ch<=ch2; ++ch)
    {
        setCharClassFlags(charClasses, ch, setClasses);
    }
}
#include "umba/warnings/pop.h"

template< std::size_t N, typename CharType >
void setCharClassFlagsForBracePair( std::array<umba::tokenizer::CharClass, N> &charClasses, const std::basic_string<CharType> &braceChars)
{
    UMBA_ASSERT(braceChars.size()==2);
    // if (braceChars.size()!=2)
    // {
    //     std::cerr << "Braces def invalid size" << std::endl;
    //     throw std::runtime_error("Braces def invalid size");
    // }

    setCharClassFlags(charClasses, braceChars[0], umba::tokenizer::CharClass::open );
    setCharClassFlags(charClasses, braceChars[1], umba::tokenizer::CharClass::close);
}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------

/*
Делаем таблицу, из которой получаем класс символа
Таблица - это 128 позиций на базоые ASCII-символы
Делаем это не вручную.
Таблица генерируется не константная, надо уметь менять её в рантайме - например,
чтобы управлять поведением символов $/@ в зависимости от контекста - то ли они могутт быть в идентификаторе, то ли нет

Имеем список операторов с названиями - "!==" -> "STRICT_NEQ"
Все операторы разбираем посимвольно, для каждого символа ставим флаг CharClass::opchar

Всё, что меньше пробела - флаг nonprintable, а также 0x7F
[X] \r, \n - linefeed
[X] \t     - tab
[X] \r, \n, \t, ' ' - space

[X] в) кавычка, двойная кавычка, бэктик - quot
[X] г) Для {}()<>[] - расставляем флаги brace, open, close
[X] д) ! ? , . ( ) - punctuation (что ещё?) - можно добавлять/менять в рантайме
[X] е) A-Za-z - alpha, identifier, identifier_first
[X] ж) 0-9    - digit, identifier

*/

template< std::size_t N >
void generateCharClassTable( umba::tokenizer::CharClass (&charClasses)[N], bool addOperatorChars=true, bool addBrackets=true)
{
    for(std::size_t i=0; i!=N; ++i)
        charClasses[i] = CharClass::none;

    // pairs
    if (addBrackets)
    {
        setCharClassFlagsForBracePair( charClasses, std::string("{}"));
        setCharClassFlagsForBracePair( charClasses, std::string("()"));
        setCharClassFlagsForBracePair( charClasses, std::string("[]"));
        setCharClassFlagsForBracePair( charClasses, std::string("<>"));
    }

    // ranges
    setCharClassFlags( charClasses,   0,   8, umba::tokenizer::CharClass::nonprintable);
    // gap for tab
    setCharClassFlags( charClasses,  10,  31, umba::tokenizer::CharClass::nonprintable);
    setCharClassFlags( charClasses, '0', '9', umba::tokenizer::CharClass::digit | umba::tokenizer::CharClass::identifier );
    setCharClassFlags( charClasses, 'A', 'Z', umba::tokenizer::CharClass::alpha | umba::tokenizer::CharClass::identifier | umba::tokenizer::CharClass::identifier_first | umba::tokenizer::CharClass::upper);
    setCharClassFlags( charClasses, 'a', 'z', umba::tokenizer::CharClass::alpha | umba::tokenizer::CharClass::identifier | umba::tokenizer::CharClass::identifier_first);

    // sets
    if (addOperatorChars)
    {
        setCharClassFlags( charClasses, std::string("!%&*+,-./:;<=>?^|~"), umba::tokenizer::CharClass::opchar);
    }
    setCharClassFlags( charClasses, std::string("\r\n")              , umba::tokenizer::CharClass::linefeed);
    setCharClassFlags( charClasses, std::string("\r\n\t ")           , umba::tokenizer::CharClass::space);
    setCharClassFlags( charClasses, std::string(".,!?()\"\'")        , umba::tokenizer::CharClass::punctuation);
    //setCharClassFlags( charClasses, "\"\'`"             , umba::tokenizer::CharClass::quot);
    setCharClassFlags( charClasses, std::string("ABCDEFabcdef")      , umba::tokenizer::CharClass::xdigit);
    setCharClassFlags( charClasses, std::string("@#$")               , umba::tokenizer::CharClass::semialpha);

    // single chars
    setCharClassFlags( charClasses,   0x7F, umba::tokenizer::CharClass::nonprintable); // DEL
    setCharClassFlags( charClasses,   '\\', umba::tokenizer::CharClass::escape);
    setCharClassFlags( charClasses,   '_' , umba::tokenizer::CharClass::identifier | umba::tokenizer::CharClass::identifier_first); // underscore

}


template< std::size_t N >
void generateCharClassTable( std::array<umba::tokenizer::CharClass, N> &charClasses, bool addOperatorChars=true, bool addBrackets=true)
{
    for(std::size_t i=0; i!=charClasses.size(); ++i)
        charClasses[i] = CharClass::none;

    // pairs
    if (addBrackets)
    {
        setCharClassFlagsForBracePair( charClasses, std::string("{}"));
        setCharClassFlagsForBracePair( charClasses, std::string("()"));
        setCharClassFlagsForBracePair( charClasses, std::string("[]"));
        setCharClassFlagsForBracePair( charClasses, std::string("<>"));
    }

    // ranges
    setCharClassFlags( charClasses,   0,   8, umba::tokenizer::CharClass::nonprintable);
    // gap for tab
    setCharClassFlags( charClasses,  10,  31, umba::tokenizer::CharClass::nonprintable);
    setCharClassFlags( charClasses, '0', '9', umba::tokenizer::CharClass::digit | umba::tokenizer::CharClass::identifier );
    setCharClassFlags( charClasses, 'A', 'Z', umba::tokenizer::CharClass::alpha | umba::tokenizer::CharClass::identifier | umba::tokenizer::CharClass::identifier_first | umba::tokenizer::CharClass::upper);
    setCharClassFlags( charClasses, 'a', 'z', umba::tokenizer::CharClass::alpha | umba::tokenizer::CharClass::identifier | umba::tokenizer::CharClass::identifier_first);

    // sets
    if (addOperatorChars)
    {
        setCharClassFlags( charClasses, std::string("!%&*+,-./:;<=>?^|~"), umba::tokenizer::CharClass::opchar);
    }
    setCharClassFlags( charClasses, std::string("\r\n")              , umba::tokenizer::CharClass::linefeed);
    setCharClassFlags( charClasses, std::string("\r\n\t ")           , umba::tokenizer::CharClass::space);
    setCharClassFlags( charClasses, std::string(".,!?()\"\'")        , umba::tokenizer::CharClass::punctuation);
    //setCharClassFlags( charClasses, "\"\'`"             , umba::tokenizer::CharClass::quot);
    setCharClassFlags( charClasses, std::string("ABCDEFabcdef")      , umba::tokenizer::CharClass::xdigit);
    setCharClassFlags( charClasses, std::string("@#$")               , umba::tokenizer::CharClass::semialpha);

    // single chars
    setCharClassFlags( charClasses,   0x7F, umba::tokenizer::CharClass::nonprintable); // DEL
    setCharClassFlags( charClasses,   '\\', umba::tokenizer::CharClass::escape);
    setCharClassFlags( charClasses,   '_' , umba::tokenizer::CharClass::identifier | umba::tokenizer::CharClass::identifier_first); // underscore

}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
enum class CommentType
{
    cLike, cppLike
};


struct CommentPrinter
{
    CommentType commentType;

    CommentPrinter(CommentType ct) : commentType(ct)
    {
        std::cout << (commentType==CommentType::cLike ? "/*" : "//") << " ";
    }

    ~CommentPrinter()
    {
        if (commentType==CommentType::cLike)
             std::cout<<" */";
    }

    template<typename T>
    CommentPrinter& operator<<(const T &t)
    {
        std::cout << t;
        return *this;
    }
};


inline
std::string nsToPath(const std::string &ns)
{
    std::string res; res.reserve(ns.size());

    for(auto ch : ns)
    {
        if (ch==':')
        {
            if (res.empty() || res.back()!='/')
                res.append(1, '/');
        }
        else
        {
            res.append(1, ch);
        }
    }

    return res;
}

inline
std::string pathToNs(const std::string &p)
{
    std::string res; res.reserve(p.size());

    for(auto ch : p)
    {
        if (ch=='/' || ch=='\\')
            res.append(2, ':');
        else
            res.append(1, ch);
    }

    return res;
}

inline
std::string nameToCpp(std::string name)
{
    name = nsToPath(name);

    std::string res; res.reserve(name.size());

    for(auto ch : name)
    {
        if (umba::string_plus::is_alpha_upper(ch) || umba::string_plus::is_digit(ch))
        {
            res.append(1, ch);
            continue;
        }

        if (ch=='|' || ch==' ')
        {
            res.append(1, ch);
            continue;
        }

        if (ch=='/' || ch=='\\')
        {
            res.append(2, ':');
            continue;
        }

        ch = '_';
    }

    return name;
}

inline
std::string nameToDefine(std::string name)
{
    name = nsToPath(name);
    umba::string_plus::toupper(name);

    for(auto &ch : name)
    {
        if (umba::string_plus::is_alpha_upper(ch) || umba::string_plus::is_digit(ch))
            continue;
        if (ch=='|' || ch==' ')
            continue;
        ch = '_';
    }

    return name;
}


enum class ArrayType
{
    cLike, stdArray
};

enum class TypeNameType
{
    cLike, cppLike
};

inline
std::string transformName(const std::string &name, TypeNameType typeNameType)
{
    if (typeNameType==TypeNameType::cLike)
        return nameToDefine(name);
    return name;
}

// Можно напечатать, как массив, можно - как template<class T, std::size_t N> struct array;
// int varName[N] =
// std::array<int, N> varName = {1, 2, 3};.

template< std::size_t N >
void printCharClassArray( umba::tokenizer::CharClass (&charClasses)[N], CommentType commentType, ArrayType arrayType, TypeNameType typeNameType, const std::string &varName="charClassesTable")
{
    static const std::string charClassType = "umba::tokenizer::CharClass";

    if (arrayType==ArrayType::cLike)
    {
        std::cout << transformName(charClassType, typeNameType) << " " << varName << "[" << N << "] =\n";
    }
    else if (arrayType==ArrayType::stdArray)
    {
        std::cout << "std::array<" << transformName(charClassType, typeNameType) << ", " << N << "> " << varName << " =\n";
    }

    for(std::size_t idx=0; idx!=N; ++idx)
    {
        std::cout << (idx==0 ? "{ " : ", ") << transformName(enum_serialize_flags(charClasses[idx], charClassType+"::"), typeNameType) << "  ";
        if (idx>=32)
           CommentPrinter(commentType) << " '" << (char)idx << "'";
        else
           CommentPrinter(commentType) << " " << idx;

        std::cout << "\n";
    }

    std::cout << "};\n";

}




} // namespace generation

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
#if !defined(UMBA_TOKENIZER_ENABLE_UMBA_TOKENIZER_GET_CHAR_CLASS_FUNCTION)
    #if !defined(UMBA_TOKENIZER_DISABLE_UMBA_TOKENIZER_GET_CHAR_CLASS_FUNCTION)
        #define UMBA_TOKENIZER_DISABLE_UMBA_TOKENIZER_GET_CHAR_CLASS_FUNCTION
    #endif
#endif


// Using pregenerated char class table
#if !defined(UMBA_TOKENIZER_DISABLE_UMBA_TOKENIZER_GET_CHAR_CLASS_FUNCTION)
inline
umba::tokenizer::CharClass getCharClass(char ch)
{
    static
    #include "char_class_table.h.inc"

    std::size_t idx = (std::size_t)(unsigned char)ch;
    if (idx>=0x80u)
        idx = 0x7Fu;

    return charClassesTable[idx];
}
#endif



//----------------------------------------------------------------------------
// Новое, для разбора строк-выражений, определяющих набор символов

// umba::tokenizer::char_class::utils::
namespace char_class {
namespace utils {

//----------------------------------------------------------------------------
// Объединение множеств - Union of sets
// Пересечение множеств - Intersection of sets

//----------------------------------------------------------------------------
inline
std::unordered_set<std::uint8_t> makeCharSetsUnion(std::unordered_set<std::uint8_t> s1, const std::unordered_set<std::uint8_t> &s2)
{
    s1.insert(s2.begin(), s2.end());
    return s1;
}

//----------------------------------------------------------------------------
inline
std::unordered_set<std::uint8_t> makeCharSetsUnion(std::unordered_set<std::uint8_t> s1, const std::unordered_set<std::uint8_t> &s2, const std::unordered_set<std::uint8_t> &s3)
{
    s1.insert(s2.begin(), s2.end());
    s1.insert(s3.begin(), s3.end());
    return s1;
}

//----------------------------------------------------------------------------
inline
std::unordered_set<std::uint8_t> makeCharSetsUnion(std::unordered_set<std::uint8_t> s1, const std::unordered_set<std::uint8_t> &s2, const std::unordered_set<std::uint8_t> &s3, const std::unordered_set<std::uint8_t> &s4)
{
    s1.insert(s2.begin(), s2.end());
    s1.insert(s3.begin(), s3.end());
    s1.insert(s4.begin(), s4.end());
    return s1;
}

//----------------------------------------------------------------------------
inline
std::unordered_set<std::uint8_t> makeCharSetsIntersection(const std::unordered_set<std::uint8_t> &set1, const std::unordered_set<std::uint8_t> &set2)
{
    std::unordered_set<std::uint8_t> resSet;
    for(auto s : set1)
    {
        if (set2.find(s)!=set2.end())
            resSet.insert(s);
    }
    return resSet;
}

//----------------------------------------------------------------------------
inline
std::unordered_set<std::uint8_t> makeCharSetsSubtraction(std::unordered_set<std::uint8_t> set1, const std::unordered_set<std::uint8_t> &set2)
{
    for(auto s : set2)
    {
        if (set1.find(s)!=set1.end())
            set1.erase(s);
    }
    return set1;
}

//----------------------------------------------------------------------------
inline
std::unordered_set<std::uint8_t> makeCharSet(std::uint8_t from, std::uint8_t to)
{
    std::size_t szFrom = from;
    std::size_t szTo   = to  ;

    UMBA_ASSERT(szFrom<=szTo);

    ++szTo;

    std::unordered_set<std::uint8_t> res;

    for(std::size_t i=szFrom; i!=szTo; ++i)
        res.insert(std::uint8_t(i));

    return res;
}

//----------------------------------------------------------------------------
inline
std::unordered_set<std::uint8_t> makeCharSet(std::initializer_list<std::uint8_t> elements)
{
    return std::unordered_set<std::uint8_t>(elements);
}

//----------------------------------------------------------------------------
inline
std::unordered_set<std::uint8_t> makeFullCharSet()
{
    return makeCharSet(0u, 0xFFu);
}

//----------------------------------------------------------------------------
inline
const std::unordered_set<std::uint8_t>& getFullCharSet()
{
    static std::unordered_set<std::uint8_t> s = makeFullCharSet();
    return s;
}

//----------------------------------------------------------------------------
inline
std::unordered_set<std::uint8_t> invertCharSet(const std::unordered_set<std::uint8_t> &set)
{
    std::unordered_set<std::uint8_t> resSet;

    auto fullSet = getFullCharSet();

    for(auto s: fullSet)
    {
        if (set.find(s)==set.end())
            resSet.insert(s);
    }

    return resSet;
}

//----------------------------------------------------------------------------
inline
std::unordered_set<std::uint8_t> makeCharSetInverted(std::uint8_t from, std::uint8_t to)
{
    return invertCharSet(makeCharSet(from, to));
}

//----------------------------------------------------------------------------
inline
std::unordered_set<std::uint8_t> makeCharSetInverted(std::initializer_list<std::uint8_t> elements)
{
    return invertCharSet(makeCharSet(elements));
}

//----------------------------------------------------------------------------
inline
const std::unordered_set<std::uint8_t>& getEmptyCharSet()
{
    static std::unordered_set<std::uint8_t> s;
    return s;
}

//----------------------------------------------------------------------------
inline
char digitToHexChar(int d)
{
    if (d<0)
        return 'X';
    if (d<=9)
        return '0'+char(d);
    if (d<=15)
        return 'A'+char(d)-10;

    return 'X';
}

//----------------------------------------------------------------------------
inline
std::string byteToHexString(std::uint8_t b)
{
    std::string res;
    res.append(1, digitToHexChar((b>>4)&0xF));
    res.append(1, digitToHexChar((b   )&0xF));
    return res;
}

//----------------------------------------------------------------------------
inline
int hexCharToDigit(char ch)
{
    if (ch>='0' && ch<='9')
        return ch - '0';
    if (ch>='A' && ch<='F')
        return ch - 'A' + 10;
    if (ch>='a' && ch<='f')
        return ch - 'a' + 10;

    return -1;
}

//----------------------------------------------------------------------------
inline
std::string makePredicateCharCharClassString(std::uint8_t ch, bool caretOnly=false)
{
    switch(ch)
    {
        // \cX - Каретная нотация
        //     https://ru.wikipedia.org/wiki/%D0%9A%D0%B0%D1%80%D0%B5%D1%82%D0%BD%D0%B0%D1%8F_%D0%BD%D0%BE%D1%82%D0%B0%D1%86%D0%B8%D1%8F
        //     https://en.wikipedia.org/wiki/C0_and_C1_control_codes
        case   0: return "\\c@";
        case   1: return "\\cA";
        case   2: return "\\cB";
        case   3: return "\\cC";
        case   4: return "\\cD";
        case   5: return "\\cE";
        case   6: return "\\cF";
        case   7: if (caretOnly) return "\\cG"; return "\\a"; // \a
        case   8: if (caretOnly) return "\\cH"; return "\\b"; // \b
        case   9: if (caretOnly) return "\\cI"; return "\\t"; // \t
        case  10: if (caretOnly) return "\\cJ"; return "\\n"; // \n
        case  11: if (caretOnly) return "\\cK"; return "\\v"; // \v
        case  12: if (caretOnly) return "\\cL"; return "\\f"; // \f
        case  13: if (caretOnly) return "\\cM"; return "\\r"; // \r
        case  14: return "\\cN";
        case  15: return "\\cO";
        case  16: return "\\cP";
        case  17: return "\\cQ";
        case  18: return "\\cR";
        case  19: return "\\cS";
        case  20: return "\\cT";
        case  21: return "\\cU";
        case  22: return "\\cV";
        case  23: return "\\cW";
        case  24: return "\\cX";
        case  25: return "\\cY";
        case  26: return "\\cZ";
        case  27: if (caretOnly) return "\\c["; return "\\e";  // \e
        case  28: return "\\c/"; // Тут замена на символ деления, по сравнению со стандартной нотацией
        case  29: return "\\c]";
        case  30: return "\\c^";
        case  31: return "\\c_";
        case 127: return "\\c?";
        case  45: return "\\-" ; // минус-дефис
        case  92: return "\\\\"; // бэкслеш
        case  96: return "\\`" ; // бэктик `
        case  39: return "\\\'"; // apos '
        case  34: return "\\\""; // quot "
        case  91: return "\\[" ; // [
        case  93: return "\\]" ; // ]

        default:  if (ch<127)
                  {
                      return std::string(1, char(ch));
                  }
                  else
                  {
                      std::string resStr = "\\x";
                      resStr.append(byteToHexString(ch));
                      return resStr;
                  }
    }
}

//----------------------------------------------------------------------------
inline
int caretCharToChar(char ch)
{
    switch(ch)
    {
        case '@': return   0;
        case 'A': return   1;
        case 'B': return   2;
        case 'C': return   3;
        case 'D': return   4;
        case 'E': return   5;
        case 'F': return   6;
        case 'G': return   7;
        case 'H': return   8;
        case 'I': return   9;
        case 'J': return  10;
        case 'K': return  11;
        case 'L': return  12;
        case 'M': return  13;
        case 'N': return  14;
        case 'O': return  15;
        case 'P': return  16;
        case 'Q': return  17;
        case 'R': return  18;
        case 'S': return  19;
        case 'T': return  20;
        case 'U': return  21;
        case 'V': return  22;
        case 'W': return  23;
        case 'X': return  24;
        case 'Y': return  25;
        case 'Z': return  26;
        case '[': return  27;
        case '/': return  28; // Тут замена на символ деления, по сравнению со стандартной нотацией
        case ']': return  29;
        case '^': return  30;
        case '_': return  31;
        case '?': return 127;
        default : return  -1;
    }
}

//----------------------------------------------------------------------------
inline
int escapedCharToChar(char ch)
{
    switch(ch)
    {
        case 'a' : return  7; // \a
        case 'b' : return  8; // \b
        case 't' : return  9; // \t
        case 'n' : return 10; // \n
        case 'v' : return 11; // \v
        case 'f' : return 12; // \f
        case 'r' : return 13; // \r
        case 'e' : return 27; // \e
        case '-' : return 45; // минус-дефис
        case '\\': return 92; // бэкслеш
        case '`' : return 96; // бэктик `
        case '\'': return 39; // apos '
        case '\"': return 34; // quot "
        case '[' : return 91; // [
        case ']' : return 93; // ]
        default  : return -1;
    }
}

//----------------------------------------------------------------------------
inline
bool escapedCharClassToCharSet(char ch, std::unordered_set<std::uint8_t> &s)
{
    switch(ch)
    {
                                        // \t   \n   \v   \f   \r 
        case 's': s = makeCharSet        ({ 9u, 10u, 11u, 12u, 13u}); return true;
        case 'S': s = makeCharSetInverted({ 9u, 10u, 11u, 12u, 13u}); return true;

        case 'd': s = makeCharSet        ('0', '9'); return true;
        case 'D': s = makeCharSetInverted('0', '9'); return true;

        case 'w': s =               makeCharSetsUnion(makeCharSet('a','z'), makeCharSet('A','Z'), makeCharSet('0','9'), makeCharSet({'_'})) ; return true;
        case 'W': s = invertCharSet(makeCharSetsUnion(makeCharSet('a','z'), makeCharSet('A','Z'), makeCharSet('0','9'), makeCharSet({'_'}))); return true;

        case 'l': s =               makeCharSetsUnion(makeCharSet('a','z'), makeCharSet('A','Z')) ; return true;
        case 'L': s = invertCharSet(makeCharSetsUnion(makeCharSet('a','z'), makeCharSet('A','Z'))); return true;

        case '*': s = makeFullCharSet(); return true;

        default : return false;
    }
}

//----------------------------------------------------------------------------
inline
std::set<std::uint8_t> makeOrdered(const std::unordered_set<std::uint8_t> &s)
{
    return std::set<std::uint8_t>(s.begin(), s.end());
}

//----------------------------------------------------------------------------
inline
std::string makePredicateCharClassString(const std::set<std::uint8_t> &charSet)
{
    if (charSet.empty())
        return std::string();

    auto charSetVec = std::vector<std::uint8_t>(charSet.begin(), charSet.end());
    std::vector< std::pair<std::size_t, std::size_t> > ranges;
    ranges.emplace_back(0u,0u);

    std::size_t idx = 1u;

    for(; idx!=charSetVec.size(); ++idx)
    {
        if (charSetVec[idx]==charSetVec[idx-1] + 1)
        {
            ranges.back().second = idx;
        }
        else
        {
            ranges.emplace_back(idx,idx);
        }
    }

    std::string resStr;

    for(auto r: ranges)
    {
        if (r.first==r.second)
        {
            resStr.append(makePredicateCharCharClassString(charSetVec[r.first]));
        }
        else
        {
            resStr.append(makePredicateCharCharClassString(charSetVec[r.first]));
            resStr.append("-");
            resStr.append(makePredicateCharCharClassString(charSetVec[r.second]));
        }
    }

    return resStr;
}

//----------------------------------------------------------------------------
inline
std::string makePredicateCharClassString(const std::unordered_set<std::uint8_t> &charSet)
{
    return makePredicateCharClassString(makeOrdered(charSet));
}

//----------------------------------------------------------------------------
inline
bool parseCharClassDefinition( const std::string &str
                             , std::unordered_set<std::uint8_t> &resSet
                             , std::size_t *pLastProcessedPos=0
                             , std::size_t startPos = 0
                             , bool returnOnCloseBracket = false
                             )
{
    enum State
    {
        stInitial  ,
        stNormal   ,
        stGotEscape,  // был получен escape символ - backslash
        stGotCaret ,  // началась caret-последовательность - "\c"
        stGotHex   ,  // началась HEX-последоватеьность - "\x"
        stGotHex2  ,  // ждём второй символ HEX-кода - "\xH"
        stGotRange
    };

    resSet.clear();

    int rangeStartChar = -1;
    // Когда добавляем char - задаём
    // Когда добавляем set  - сбрасываем
    // В остальных случаях  - не трогаем

    auto err      = [&](std::size_t i) { if (pLastProcessedPos) *pLastProcessedPos = i; return false; };
    auto ok       = [&](std::size_t i) { if (pLastProcessedPos) *pLastProcessedPos = i; return true ; };
    auto th       = [&](std::size_t i)
    {
        throw std::runtime_error("error at pos " + std::to_string(i) + ", rest line: " + std::string(&str[i]));
    };

    auto insertRange = [&](char ch, auto idx) -> bool
    {
        if (std::uint8_t(rangeStartChar)<0u)
            return err(idx);

        if (std::uint8_t(rangeStartChar)> std::uint8_t(ch))
            return err(idx);

        auto s = makeCharSet(std::uint8_t(rangeStartChar), std::uint8_t(ch));
        resSet = makeCharSetsUnion(resSet, s);
        rangeStartChar = -1;

        return ok(idx);
    };

    auto insertCh = [&](char ch, auto idx) -> bool
    {
        if (rangeStartChar<0) // добавляем одиночный символ
        {
            resSet.insert(std::uint8_t(ch));
            rangeStartChar = ch;
            return ok(idx);
        }
        else
        {
            return insertRange(ch, idx);
        }
    };

    State        st  = stInitial;
    //PrevCharType pvt = pvtUnknown;

    int hexDigit = 0;
    // bool bRangeMode = false;


    // Специальные символы у нас:
    //   минус/дефис - обозначает:
    //     диапазон
    //     вычитание множеств, если за ним следует квадратная скобка, открывающая определение множества
    //   закрывающая квадратная скобка
    //  пока вроде всё

    std::size_t idx = startPos;

    for(; idx!=str.size(); ++idx)
    {
        char ch = str[idx];

        switch(st)
        {
            case stInitial:
            {
                if (ch=='\\')
                    st  = stGotEscape;

                else if (ch=='-') // или для минуса-дефиса сделать на старте исключение? наверное не стоит
                    return err(idx); // у нас ситуация на старте, ни диапазон, ни вычитание не возможны

                else if (ch==']')
                {
                    if (returnOnCloseBracket)
                        return ok(idx);
                }

                else // просто символ
                {
                    st  = stNormal;
                    if (!insertCh(ch, idx))
                        return err(idx);
                }

                break;
            }

            case stNormal:
            {
                if (ch=='\\')
                    st  = stGotEscape;

                else if (ch=='-') // у нас либо диапазон, либо начало "вычитания" множеств
                    st  = stGotRange;

                else if (ch==']')
                {
                    if (returnOnCloseBracket)
                        return ok(idx);
                }

                else // просто символ
                {
                    if (!insertCh(ch, idx))
                        return err(idx);
                }
                break;
            }
        
            case stGotEscape:
            {
                if (ch=='c') // "caret" escape sequence // \cX 
                    st  = stGotCaret;

                else if (ch=='x') // \xhh \xd \xD 
                    st  = stGotHex  ;

                else // вероятно, обычная escape sequence, или escape sequence класса символа
                {
                    auto escapedChar = escapedCharToChar(ch);
                    if (escapedChar>=0) // обычная escape-последовательность - \X
                    {
                        st  = stNormal;
                        if (!insertCh(char(escapedChar), idx))
                            return err(idx);
                    }
                    else
                    {
                        std::unordered_set<std::uint8_t> escapedCharSet;
                        if (escapedCharClassToCharSet(ch, escapedCharSet))
                        {
                            // нормальный char-класс приехал, объединяем с существующим, и пилим по нормасу дальше
                            st  = stNormal;
                            rangeStartChar = -1;
                            resSet = makeCharSetsUnion(resSet, escapedCharSet);
                        }

                        else
                            return err(idx); // что-то пошло не так
                    }

                }

                break;
            }
        
            case stGotCaret:
            {
                auto caretChar = caretCharToChar(ch);

                if (caretChar<0)
                    return err(idx); // что-то пошло не так

                if (!insertCh(char(caretChar), idx))
                    return err(idx);

                st  = stNormal;

                break;
            }
        
            case stGotHex:
            {
                if (ch=='d')
                {
                    auto s = makeCharSetsUnion(makeCharSet('a','f'), makeCharSet('A','F'), makeCharSet('0','9'));
                    resSet = makeCharSetsUnion(resSet, s);
                    rangeStartChar = -1;
                }
                else if (ch=='D')
                {
                    auto s = makeCharSetsUnion(makeCharSet('a','f'), makeCharSet('A','F'), makeCharSet('0','9'));
                    resSet = makeCharSetsUnion(resSet, invertCharSet(s));
                    rangeStartChar = -1;
                }
                else
                {
                    int d = hexCharToDigit(ch);
                    if (d<0 || d>15)
                        return err(idx);
                    st  = stGotHex2;
                    hexDigit = d;
                }
                break;
            }
        
            case stGotHex2:
            {
                int d = hexCharToDigit(ch);
                if (d<0 || d>15)
                    return err(idx);
                st  = stNormal;
                if (!insertCh(char(std::uint8_t((hexDigit<<4)+d)), idx))
                    return err(idx);
                break;
            }
        
            case stGotRange:
            {
                if (ch=='[') // У нас тут вычитание
                {
                    std::unordered_set<std::uint8_t> tmpSet;
                    if (!parseCharClassDefinition(str, tmpSet, &idx, idx+1, true  /* returnOnCloseBracket */ ))
                        return err(idx);

                    resSet = makeCharSetsSubtraction(resSet, tmpSet);
                    rangeStartChar = -1;
                }
                else // обычный символ или escape-последовательность как завершение диапазона
                {
                    if (rangeStartChar<0) // но у нас нет начала диапазноа
                        return err(idx);

                    if (ch=='\\')
                        st  = stGotEscape;

                    else
                    {
                        if (!insertRange(ch, idx))
                            return err(idx);
                    }
                }

                break;
            }

            default: return err(idx); // хз чо
        
        } // switch(st)
    
    } // for(; idx!=str.size(); ++idx)

    return ok(idx);
}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------

} // namespace utils
} // namespace char_class
// umba::tokenizer::char_class::utils::

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------

} // namespace tokenizer
} // namespace umba
// umba::tokenizer::

