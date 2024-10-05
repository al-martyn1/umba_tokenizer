#pragma once

#ifndef UMBA_TOKENIZER_H__DCAEEDE0_7624_4E47_9919_08460EF65A3B__
    #error "Do not include this file directly, include 'umba/tokenizer.h header instead'"
#endif

#if !defined(UMBA_TOKENOZER_DONT_USE_MARTY_DECIMAL)
    #include "marty_decimal/marty_decimal.h"
    #if !defined(UMBA_TOKENOZER_MARTY_DECIMAL_USED)
        #define UMBA_TOKENOZER_MARTY_DECIMAL_USED
    #endif
#endif


// umba::tokenizer::
namespace umba {
namespace tokenizer {


//----------------------------------------------------------------------------
inline
void trieNodeInitMakeUninitialized(TrieNode &node)
{
    umba_tokenizer_trie_node_init_make_uninitialized(&node);
}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
// trie_index_type tokenTrieFindFirst(const ContainerType &tokenTrie, token_type tk)
// trie_index_type tokenTrieFindNext(const ContainerType &tokenTrie, trie_index_type curIndex, token_type tk)
// trie_index_type tokenTrieTraverse(const ContainerType &tokenTrie, TokenSequenceIteratorType b, TokenSequenceIteratorType e)
// void tokenTrieBackTrace(const ContainerType &tokenTrie, trie_index_type curIndex, BackTraceHandlerType handler)
// void tokenTriePrintTableGraph(const std::string &name, const ContainerType &tokenTrie, StreamType &s, TokenToStringConverter converter)

//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
#if !defined(UMBA_TOKENIZER_TRIE_NODE_LEVEL_FIELD_DISABLE)
template<typename ContainerType, typename StreamType, typename TokenToStringConverter>
void tokenTriePrintTableGraph(const std::string &name, const ContainerType &tokenTrie, StreamType &s, TokenToStringConverter converter)
{
    // requires UMBA_TOKENIZER_TRIE_NODE_LEVEL_FIELD_DISABLE is not defined

    s << "digraph " << name << " {\nnode [shape=record];\n";


    //s << "L"<<lvl<< "[label=\"";

    trie_index_type lvl = trie_index_invalid;
    trie_index_type idx = 0;
    bool levelChanged = false;
    for( const auto &t : tokenTrie)
    {
        if (lvl!=t.level)
        {
            lvl = t.level;
            if (lvl!=0)
                s << "\"];\n";
            s << "L"<<lvl<< "[label=\"";
            levelChanged = true;
        }

        if (!levelChanged)
           s << "|";
        levelChanged = false;

        s << "{<I" << idx << ">";

        std::string strToken = converter(t.token);
        if (strToken.empty())
        {
            s << t.token;
        }
        else
        {
            s << escapeStringGraphViz(strToken);
        }

        s << "|<O" << idx << ">" << idx << "}";

        // //l2 [label="
        // {<t4>+|<f4>4}
        // |{<t5>-|<f5>5}|{<t6>=|<f6>6}|{<t7>\>|<f7>7}|{<t8>+|<f8>8}|{<t9>-|<f9>9}|{<t10>=|<f10>10}|{<t11>-|<f11>11}|{<t12>=|<f12>12}|{<t13>\>|<f13>13}"] // |{<t15>=|<f15>15}|{<t16>\>|<f16>16}

        ++idx;
    }

    s << "\"];\n";

    //trie_index_type lvl = trie_index_invalid;
    //trie_index_type
    idx = 0;
    for( const auto &t : tokenTrie)
    {
        // l1:f0  -> l2:t4

        if (t.childsIndex!=trie_index_invalid)
        {
            s << "L" << t.level << ":O" << idx << " -> L" << tokenTrie[t.childsIndex].level << ":I" << t.childsIndex <<"\n";
        }

        ++idx;
    }

    s << "}\n";

}
#endif
//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
namespace utils {




//----------------------------------------------------------------------------
// https://stackoverflow.com/questions/19392361/adding-element-to-back-of-stl-container
// https://stackoverflow.com/questions/14882588/correct-signature-of-detect-presence-of-containerreserve
//----------------------------------------------------------------------------
//! Базовый false-тип для детекта наличия метода rebase у объекта
template< typename C, typename = void >
struct iterator_has_rebase : std::false_type
{};

//------------------------------
//! Специализация, тестирующая наличие метода reserve у объекта
template< typename C >
struct iterator_has_rebase< C, std::enable_if_t< std::is_same<decltype( std::declval<C>().rebase( reinterpret_cast<const typename C::value_type *>(1)) ), void >::value > >
  : std::true_type
{};

//------------------------------
template< typename C >
std::enable_if_t< iterator_has_rebase< C >::value > inline
iterator_rebase( C& c, const typename C::value_type * newBase )
{
  c.rebase( newBase );
}

//------------------------------
//! Версия для итераторов, не имеющих метода rebase. Не делает ничего.
template< typename C >
std::enable_if_t< !iterator_has_rebase< C >::value > inline
iterator_rebase( C& c, const typename C::value_type * newBase ) {}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
//! Базовый false-тип для детекта наличия метода rebase у объекта
template< typename C, typename = void >
struct iterator_has_getRawValueTypePointer : std::false_type
{};

//------------------------------
//! Специализация, тестирующая наличие метода reserve у объекта
template< typename C >
struct iterator_has_getRawValueTypePointer< C, std::enable_if_t< std::is_same<decltype( std::declval<C>().getRawValueTypePointer()), void >::value > >
  : std::true_type
{};

//------------------------------
template< typename C >
std::enable_if_t< iterator_has_getRawValueTypePointer< C >::value > inline
iterator_getRawValueTypePointer( C& c )
{
  c.getRawValueTypePointer( );
}

//------------------------------
//! Версия для итераторов, не имеющих метода getRawValueTypePointer
template< typename C >
std::enable_if_t< !iterator_has_rebase< C >::value > inline
iterator_getRawValueTypePointer( C& c ) { return &*c; }

//----------------------------------------------------------------------------






//----------------------------------------------------------------------------
// bool isCommentToken(payload_type tokenType)
// bool isSingleLineCommentToken(payload_type tokenType)
// bool isMultiLineCommentStartToken(payload_type tokenType)
// bool isMultiLineCommentEndToken(payload_type tokenType)

//----------------------------------------------------------------------------
constexpr
inline
bool isCommentToken(payload_type tokenType)
{
    return ((tokenType>=UMBA_TOKENIZER_TOKEN_OPERATOR_SINGLE_LINE_COMMENT_FIRST && tokenType<=UMBA_TOKENIZER_TOKEN_OPERATOR_MULTI_LINE_COMMENT_END))
         ? true
         : false
         ;
}

// Тут надо, чтобы это уже был коммент - вызывать только после вызова isCommentToken
constexpr
inline
bool isSingleLineCommentToken(payload_type tokenType)
{
    return (tokenType<=UMBA_TOKENIZER_TOKEN_OPERATOR_SINGLE_LINE_COMMENT_LAST) ? true : false;
}

// Тут надо, чтобы это уже был коммент, и не однострочный - вызывать только после вызова isCommentToken
constexpr
inline
bool isMultiLineCommentStartToken(payload_type tokenType)
{
    return tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_MULTI_LINE_COMMENT_START;
}

// Тут надо, чтобы это уже был коммент, и не однострочный - вызывать только после вызова isCommentToken
constexpr
inline
bool isMultiLineCommentEndToken(payload_type tokenType)
{
    return tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_MULTI_LINE_COMMENT_END;
}

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
// bool isNumberPrefixRequiresDigits(payload_type tokenType)
// bool isNumberPrefixAllowMissDigits(payload_type tokenType)
// int numberPrefixGetBase(payload_type tokenType)
// bool isNumberHexDigitsAllowed(int base)
// bool isDigitAllowed(CharType ch, int base)
// int getNumberBaseFromExplicitAndDefault(int explicitBase, int defaultBase)

//----------------------------------------------------------------------------
// Достаточно ли только префикса для валидного числа, или обязательно нужны цыфры?
constexpr
inline
bool isNumberPrefixRequiresDigits(payload_type tokenType)
{
    return (tokenType & UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_FLAG_MISS_DIGIT)==0;
}

// Превикс допускает отсутствие дополнительных цифр
constexpr
inline
bool isNumberPrefixAllowMissDigits(payload_type tokenType)
{
    return (tokenType & UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_FLAG_MISS_DIGIT)!=0;
}

//constexpr
inline
int numberPrefixGetBase(payload_type tokenType)
{
    auto masked = tokenType&UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_BASE_MASK;
    switch(masked)
    {
        case UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_BASE_DEC : return 10u;
        case UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_BASE_BIN : return  2u;
        case UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_BASE_QUAT: return  4u;
        case UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_BASE_OCT : return  8u;
        case UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_BASE_DUOD: return 12u;
        case UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_BASE_HEX : return 16u;
        default: return 10u;
    }
}

constexpr
inline
bool isNumberHexDigitsAllowed(int base)
{
    return base > 10;
}

template<typename CharType>
inline
int charToDigit(CharType ch)
{
    if (ch>=(CharType)'0' && ch<=(CharType)'9') return (int)(ch-(CharType)'0');
    if (ch>=(CharType)'A' && ch<=(CharType)'F') return (int)(ch-(CharType)'A'+10);
    if (ch>=(CharType)'a' && ch<=(CharType)'f') return (int)(ch-(CharType)'a'+10);
    return -1;
}

template<typename CharType>
inline
bool isDigitAllowed(CharType ch, int base)
{
    int d = charToDigit(ch);
    if (d<0)
        return false;
    return d<base ? true : false;
}

//----------------------------------------------------------------------------
constexpr
inline
int getNumberBaseFromExplicitAndDefault(int explicitBase, int defaultBase)
{
    return (explicitBase<=0) ? defaultBase : explicitBase;
}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
inline
std::uint64_t mulAndCheckOverflow(std::uint64_t op1, std::uint64_t op2, bool &bOverflow)
{
    std::uint64_t newVal = op1 * op2;
    if (op2!=0)
    {
        if ((newVal/op2)!=op1)
            bOverflow = true;
    }

    return newVal;
}

//----------------------------------------------------------------------------
inline
std::uint64_t addAndCheckOverflow(std::uint64_t op1, std::uint64_t op2, bool &bOverflow)
{
    std::uint64_t newVal = op1 + op2;
    if (newVal<op1 || newVal<op2)
        bOverflow = true;

    return newVal;
}

//----------------------------------------------------------------------------
inline
marty::Decimal mulAndCheckOverflow(marty::Decimal op1, std::uint64_t op2, bool &bOverflow)
{
    UMBA_USED(bOverflow);
    return op1 * (marty::Decimal)op2; // переполнения никогда не будет
}

//----------------------------------------------------------------------------
inline
marty::Decimal addAndCheckOverflow(marty::Decimal op1, std::uint64_t op2, bool &bOverflow)
{
    UMBA_USED(bOverflow);
    return op1 + (marty::Decimal)op2; // переполнения никогда не будет
}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
template<typename PowerValueType>
PowerValueType makePowerOf(PowerValueType base, std::uint64_t pwr, bool &bOverflow)
{
    PowerValueType res = 1;
    while(pwr>0)
    {
        res = mulAndCheckOverflow(res, base, bOverflow);
        pwr -= 1;
    }

    return res;
}


//----------------------------------------------------------------------------




} // namespace utils
//----------------------------------------------------------------------------










} // namespace tokenizer
} // namespace umba

