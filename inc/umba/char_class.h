#pragma once

#include <cstdint>
#include <climits>
#include <type_traits>
#include <string>
#include <iostream>
#include <iomanip>
#include <exception>
#include <stdexcept>
#include <array>

//
#include "assert.h"
#include "string_plus.h"
//
#include "c_char_class.h"



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

template< std::size_t N, typename CharType >
void setCharClassFlags( std::array<umba::tokenizer::CharClass, N> &charClasses, CharType ch1, CharType ch2, umba::tokenizer::CharClass setClasses)
{
    for(auto ch=ch1; ch<=ch2; ++ch)
    {
        setCharClassFlags(charClasses, ch, setClasses);
    }
}

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



} // namespace tokenizer
} // namespace umba

