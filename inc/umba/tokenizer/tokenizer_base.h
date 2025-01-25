#pragma once

#ifndef UMBA_TOKENIZER_H__DCAEEDE0_7624_4E47_9919_08460EF65A3B__
    #error "Do not include this file directly, include 'umba/tokenizer.h header instead'"
#endif

#include "umba/warnings/push_disable_rel_inc_contains_dbldot.h"
#include "umba/the.h"
#include "umba/rule_of_five.h"
#include "umba/warnings/pop.h"

#if !defined(UMBA_TOKENOZER_DONT_USE_MARTY_DECIMAL)
    #include "marty_decimal/marty_decimal.h"
    #if !defined(UMBA_TOKENOZER_MARTY_DECIMAL_USED)
        #define UMBA_TOKENOZER_MARTY_DECIMAL_USED
    #endif
#endif

//
#include <variant>
#include <array>
#include <algorithm>
#include <memory>


//----------------------------------------------------------------------------
// umba::tokenizer::
namespace umba {
namespace tokenizer {

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
enum class TokenizerInternalState
{
    stInitial              = 0,
    stReadSpace               ,
    stReadIdentifier          ,
    stReadNumberPrefix        ,
    stReadNumber              ,
    stReadNumberFloat         ,
    stReadNumberMayBeFloat    ,
    stReadOperator            ,
    stReadSingleLineComment   ,
    stReadMultilineLineComment,
    stReadStringLiteral       ,
    stContinuationWaitLinefeed

};

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
#if !defined(UMBA_TOKENIZER_DISABLE_TYPES_META)
template<typename StringType>
StringType getTokenizerInternalStateStr(TokenizerInternalState s)
{
    switch(s)
    {
        case TokenizerInternalState::stInitial                 : return umba::string_plus::make_string<StringType>("Initial");
        case TokenizerInternalState::stReadSpace               : return umba::string_plus::make_string<StringType>("ReadSpace");
        case TokenizerInternalState::stReadIdentifier          : return umba::string_plus::make_string<StringType>("ReadIdentifier");
        case TokenizerInternalState::stReadNumberPrefix        : return umba::string_plus::make_string<StringType>("ReadNumberPrefix");
        case TokenizerInternalState::stReadNumber              : return umba::string_plus::make_string<StringType>("ReadNumber");
        case TokenizerInternalState::stReadNumberFloat         : return umba::string_plus::make_string<StringType>("ReadNumberFloat");
        case TokenizerInternalState::stReadNumberMayBeFloat    : return umba::string_plus::make_string<StringType>("ReadNumberMayBeFloat");
        case TokenizerInternalState::stReadOperator            : return umba::string_plus::make_string<StringType>("ReadOperator");
        case TokenizerInternalState::stReadSingleLineComment   : return umba::string_plus::make_string<StringType>("ReadSingleLineComment");
        case TokenizerInternalState::stReadMultilineLineComment: return umba::string_plus::make_string<StringType>("ReadMultilineLineComment");
        case TokenizerInternalState::stReadStringLiteral       : return umba::string_plus::make_string<StringType>("ReadStringLiteral");
        case TokenizerInternalState::stContinuationWaitLinefeed: return umba::string_plus::make_string<StringType>("ContinuationWaitLinefeed");

        default:                         return umba::string_plus::make_string<StringType>("<UNKNOWN>");
    }
}

template<typename StringType>
StringType getTokenizerTokenStr(payload_type p)
{
    switch(p)
    {
        case UMBA_TOKENIZER_TOKEN_UNEXPECTED                       : return umba::string_plus::make_string<StringType>("<UNEXPECTED>");
        case UMBA_TOKENIZER_TOKEN_LINEFEED                         : return umba::string_plus::make_string<StringType>("LINEFEED");
        case UMBA_TOKENIZER_TOKEN_SPACE                            : return umba::string_plus::make_string<StringType>("SPACE");
        case UMBA_TOKENIZER_TOKEN_TAB                              : return umba::string_plus::make_string<StringType>("TAB");
        case UMBA_TOKENIZER_TOKEN_FORM_FEED                        : return umba::string_plus::make_string<StringType>("FF");
        case UMBA_TOKENIZER_TOKEN_LINE_CONTINUATION                : return umba::string_plus::make_string<StringType>("CONTINUATION");
        case UMBA_TOKENIZER_TOKEN_IDENTIFIER                       : return umba::string_plus::make_string<StringType>("IDENTIFIER");
        case UMBA_TOKENIZER_TOKEN_SEMIALPHA                        : return umba::string_plus::make_string<StringType>("SEMIALPHA");
        case UMBA_TOKENIZER_TOKEN_CURLY_BRACKET_OPEN               : return umba::string_plus::make_string<StringType>("KIND_OF_BRACKET");
        case UMBA_TOKENIZER_TOKEN_CURLY_BRACKET_CLOSE              : return umba::string_plus::make_string<StringType>("KIND_OF_BRACKET");
        case UMBA_TOKENIZER_TOKEN_ROUND_BRACKET_OPEN               : return umba::string_plus::make_string<StringType>("KIND_OF_BRACKET");
        case UMBA_TOKENIZER_TOKEN_ROUND_BRACKET_CLOSE              : return umba::string_plus::make_string<StringType>("KIND_OF_BRACKET");
        case UMBA_TOKENIZER_TOKEN_ANGLE_BRACKET_OPEN               : return umba::string_plus::make_string<StringType>("KIND_OF_BRACKET");
        case UMBA_TOKENIZER_TOKEN_ANGLE_BRACKET_CLOSE              : return umba::string_plus::make_string<StringType>("KIND_OF_BRACKET");
        case UMBA_TOKENIZER_TOKEN_SQUARE_BRACKET_OPEN              : return umba::string_plus::make_string<StringType>("KIND_OF_BRACKET");
        case UMBA_TOKENIZER_TOKEN_SQUARE_BRACKET_CLOSE             : return umba::string_plus::make_string<StringType>("KIND_OF_BRACKET");
        case UMBA_TOKENIZER_TOKEN_PLANTUML_PSEUDO_STATE            : return umba::string_plus::make_string<StringType>("PLANTUML_TRANSITION");
        case UMBA_TOKENIZER_TOKEN_PLANTUML_HISTORY_STATE           : return umba::string_plus::make_string<StringType>("PLANTUML_PSEUDO_STATE");

        case UMBA_TOKENIZER_TOKEN_OPERATOR_MULTI_LINE_COMMENT_START: return umba::string_plus::make_string<StringType>("COMMENT_START");
        case UMBA_TOKENIZER_TOKEN_OPERATOR_MULTI_LINE_COMMENT_END  : return umba::string_plus::make_string<StringType>("COMMENT_END");
        //case : return umba::string_plus::make_string<StringType>("");
        default:

            if (p>=UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_FIRST && p<=UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_LAST)
            {
                if (p&UMBA_TOKENIZER_TOKEN_FLOAT_FLAG)
                    return umba::string_plus::make_string<StringType>("KIND_OF_FLOAT_NUMBER");
                else
                    return umba::string_plus::make_string<StringType>("KIND_OF_NUMBER");
            }

            if (p>=UMBA_TOKENIZER_TOKEN_OPERATOR_SINGLE_LINE_COMMENT_FIRST && p<=UMBA_TOKENIZER_TOKEN_OPERATOR_SINGLE_LINE_COMMENT_LAST)
                return umba::string_plus::make_string<StringType>("KIND_OF_SINGLE_LINE_COMMENT");

            if (p>=UMBA_TOKENIZER_TOKEN_NUMBER_USER_LITERAL_FIRST && p<=UMBA_TOKENIZER_TOKEN_NUMBER_USER_LITERAL_LAST)
                return umba::string_plus::make_string<StringType>("KIND_OF_NUMBER");

            if (p>=UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST && p<=UMBA_TOKENIZER_TOKEN_OPERATOR_LAST)
                return umba::string_plus::make_string<StringType>("KIND_OF_OPERATOR");

            if (p>=UMBA_TOKENIZER_TOKEN_STRING_LITERAL_FIRST && p<=UMBA_TOKENIZER_TOKEN_STRING_LITERAL_LAST)
                return umba::string_plus::make_string<StringType>("KIND_OF_STRING_LITERAL");

            if (p>=UMBA_TOKENIZER_TOKEN_KEYWORD_SET1_FIRST && p<=UMBA_TOKENIZER_TOKEN_KEYWORD_SET4_LAST)
                return umba::string_plus::make_string<StringType>("KIND_OF_KEYWORD");



            return umba::string_plus::make_string<StringType>("");
    }
}
#endif // !defined(UMBA_TOKENIZER_DISABLE_TYPES_META)
//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
// CharType идёт первым после TBase параметром, чтобы можно было задавать только его, оставляя остальные по умолчанию
// UsedDataType идёт вторым, чтобы при необходимости его также можно было задать, оставляя остальные по умолчанию

#include "umba/warnings/push_disable_padding_added.h"
template< typename TBase
        , typename CharType            // Input chars type
        , typename UserDataType        = void*
        , typename CharClassTableType  = std::array<CharClass, 128>
        , typename TrieVectorType      = std::vector<TrieNode>
        , typename StringType          = std::basic_string<CharType>  // Тип строки, которая хранит входные символы
        , typename MessagesStringType  = std::string  // Тип строки, которая используется для сообщений (в том числе и от внешних суб-парсеров)
        , typename InputIteratorType   = umba::iterator::TextPositionCountingIterator<CharType>
        , typename InputIteratorTraits = umba::iterator::TextIteratorTraits<InputIteratorType>
        >
class TokenizerBaseImpl
{

//------------------------------
public: // depending types

    using user_data_type           = UserDataType;
    using char_type                = CharType;
    using value_type               = CharType;
    using char_class_table_type    = CharClassTableType;
    using trie_vector_type         = TrieVectorType;
    using string_type              = StringType;
    using iterator_type            = InputIteratorType;
    using iterator_traits_type     = InputIteratorTraits;
    using messages_string_type     = MessagesStringType;

    using ITokenizerLiteralParser  = umba::tokenizer::ITokenizerLiteralParser<CharType, MessagesStringType, InputIteratorType, InputIteratorTraits>;


    //------------------------------
    // EmptyData
    // CommentData
    // StringLiteralData
    // IdentifierData
    // IntegerNumericLiteralData
    // FloatNumericLiteralData
    //------------------------------
    struct EmptyData
    {};

    //------------------------------
    struct CommentData // Текст комента без обрамляющих символов
    {
        std::basic_string_view<value_type>  data;

        StringType asString() const
        {
            return StringType(data);
        }

    }; // struct CommentData

    //------------------------------
    struct StringLiteralData
    {
        std::basic_string_view<value_type>  data;

        bool                 hasSuffix = false;
        iterator_type        suffixStartPos = {};

        StringType asString() const
        {
            return StringType(data);
        }

    }; // struct StringLiteralData

    //------------------------------
    struct IdentifierData
    {
        std::basic_string_view<value_type>  data;

        StringType asString() const
        {
            return StringType(data);
        }

    }; // struct IdentifierData

    //------------------------------
    struct IntegerNumericLiteralData
    {
#if defined(UMBA_TOKENOZER_MARTY_DECIMAL_USED)
        marty::Decimal       data;
#else
        std::uint64_t        data;
#endif
        bool                 fOverflow; // число не влезло в используемый тип (std::uint64_t). Для marty::Decimal такой ситуации не происходит.

        bool                 hasSuffix = false;
        iterator_type        suffixStartPos = {};

    }; // struct NumericLiteralData

    //------------------------------
    struct FloatNumericLiteralData
    {
#if defined(UMBA_TOKENOZER_MARTY_DECIMAL_USED)
        using DataValueType  = marty::Decimal;
#else
        using DataValueType  = double;
#endif

        DataValueType        data;
        bool                 fIntegerOverflow   ; // при разборе целая часть не влезла в std::uint64_t. Для marty::Decimal такой ситуации не происходит.
        bool                 fFractionalOverflow; // при разборе дробная часть не влезла в std::uint64_t. Для marty::Decimal такой ситуации не происходит.

        bool                 hasSuffix = false;
        iterator_type        suffixStartPos = {};

    }; // struct NumericLiteralData

    //------------------------------

    // https://en.cppreference.com/w/cpp/utility/variant
    // https://en.cppreference.com/w/cpp/utility/variant/get_if
    // https://en.cppreference.com/w/cpp/utility/variant/get
    // https://en.cppreference.com/w/cpp/utility/variant/holds_alternative
    // https://en.cppreference.com/w/cpp/utility/variant/visit

    // EmptyData must be a first type in a variant (with zero index)
    using TokenParsedData = std::variant<EmptyData, CommentData, IdentifierData, StringLiteralData, IntegerNumericLiteralData, FloatNumericLiteralData>;

    using TokenParsedDataType    = TokenParsedData;
    using token_parsed_data_type = TokenParsedData;

    using empty_data_type                    = EmptyData                ;
    using comment_data_type                  = CommentData              ;
    using identifier_data_type               = IdentifierData           ;
    using string_literal_data_type           = StringLiteralData        ;
    using integer_numeric_literal_data_type  = IntegerNumericLiteralData;
    using float_numeric_literal_data_type    = FloatNumericLiteralData  ;



//------------------------------
public: // ctors and op=

    // А почему мы запрещаем копирование?
    // UMBA_RULE_OF_FIVE(TokenizerBaseImpl, default, delete, delete, default, default);

    // Для запрета вроде бы нет объективных причин
    UMBA_RULE_OF_FIVE(TokenizerBaseImpl, default, default, default, default, default);



//------------------------------
protected: // internal types

    using TokenizerLiteralCharStringInserter  = umba::tokenizer::TokenizerLiteralCharStringInserter<CharType, StringType>;


//------------------------------
protected: // fields

    CharClassTableType    charClassTable;

    TrieVectorType        numbersTrie   ;
    TrieVectorType        bracketsTrie  ;
    TrieVectorType        operatorsTrie ;
    TrieVectorType        literalsTrie  ;

    StringType            multiLineCommentEndStr;

    std::vector<std::shared_ptr<ITokenizerLiteralParser> >  literalParsersStorage;

    TokenizerOptions      options; // Не являются состоянием, обычно задаются в начале, и никогда не меняются

    user_data_type        userData;


//------------------------------
protected: // fileds - состояние токенизатора

    // Наверное, что-то можно упростить, но лень пока разбираться, когда-нибудь потом займусь

    mutable TokenizerInternalState st            = TokenizerInternalState::stInitial;
    mutable TokenizerInternalState stEscapeSaved = TokenizerInternalState::stInitial;

    mutable int                    rawModeCounter = 0;

    mutable bool                   curPosAtLineBeginning = true;

    // Общее
    mutable InputIteratorType      tokenStartIt;

    // Операторы
    mutable trie_index_type        operatorIdx = trie_index_invalid;

    // Числовые литералы
    // https://learn.microsoft.com/ru-ru/cpp/cpp/numeric-boolean-and-pointer-literals-cpp?view=msvc-170
    // https://en.cppreference.com/w/cpp/language/integer_literal
    // https://en.cppreference.com/w/cpp/language/floating_literal
    mutable payload_type           numberTokenId                = 0;
    mutable int                    numberExplicitBase           = 0;
    mutable trie_index_type        numberPrefixIdx              = trie_index_invalid;
    mutable payload_type           numberReadedDigits           = 0;
    mutable CharClass              allowedDigitCharClass        = CharClass::none;
    mutable int                    numbersBase                  = 0;

#if defined(UMBA_TOKENOZER_MARTY_DECIMAL_USED)
    mutable marty::Decimal         numberCurrentIntValue        = 0;
    mutable marty::Decimal         numberCurrentFractionalValue = 0;
    mutable marty::Decimal         numberCurrentFractionalPower = 1;
#else
    mutable std::uint64_t          numberCurrentIntValue        = 0;
    mutable std::uint64_t          numberCurrentFractionalValue = 0;
    mutable std::uint64_t          numberCurrentFractionalPower = 1;
#endif
    mutable bool                   numberIntegerOverflow        = false; // При использовании marty::Decimal всегда false
    mutable bool                   numberFractionalOverflow     = false; //

    // Коментарии
    mutable InputIteratorType      commentStartIt;
    mutable InputIteratorType      commentEndStartIt;
    mutable payload_type           commentTokenId = 0;
    mutable std::size_t            commentEndMatchIndex = 0;


    // Строковые литералы
    // https://learn.microsoft.com/ru-ru/cpp/cpp/string-and-character-literals-cpp?view=msvc-170
    mutable ITokenizerLiteralParser*            pCurrentLiteralParser = 0;
    mutable payload_type                        literalTokenId = 0;
    mutable MessagesStringType                  externHandlerMessage;
    mutable TokenizerLiteralCharStringInserter  stringLiteralValueCollector;



//------------------------------
protected: // methods


//------------------------------
public: // methods

    //mutable std::size_t            rawModeCounter = 0;

    static
    InputIteratorTraits getInputIteratorTraits()
    {
        return InputIteratorTraits();
    }

    user_data_type getUserData() const
    {
        return userData;
    }

    user_data_type setUserData(user_data_type data)
    {
        using namespace std;
        swap(data, userData);
        return data;
    }

    bool isInRawMode() const
    {
        return rawModeCounter>0;
    }

    void setRawMode(bool bRawMode)
    {
        if (bRawMode)
            ++rawModeCounter;
        else
            --rawModeCounter;
    }

    void addOwnershipForLiteralParsers(const std::vector<std::shared_ptr<ITokenizerLiteralParser> > &literalParsers)
    {
        literalParsersStorage.insert(literalParsersStorage.end(), literalParsers.begin(), literalParsers.end());
    }

    void setOptions(const TokenizerOptions &opts)
    {
        options = opts;
    }

    const TokenizerOptions& getOptions() const
    {
        return options;
    }

    void setCharClassTable( const CharClassTableType &cht)
    {
        UMBA_ASSERT(cht.size()>=128);
        charClassTable = cht;
    }

    // charClassTable может модифицироваться, и тогда она составляет часть состояния токенизатора.
    // Если его надо сохранять, то charClassTable надо сохранять вручную, потому что по умолчанию сохранять
    // в состоянии всю charClassTable слишком жирно. Пользователь может сохранять лишь те элементы, которые
    // модифицировал сам.
    CharClassTableType getCharClassTable() const { return charClassTable; }

    //! Возвращает предыдущее состояние флагов
    umba::tokenizer::CharClass setResetCharClassFlags(CharType ch, umba::tokenizer::CharClass setFlags, umba::tokenizer::CharClass clrFlags)
    {
        std::size_t idx = umba::tokenizer::charToCharClassTableIndex(ch); // clamp 127
        UMBA_ASSERT(idx<charClassTable.size());

        auto res = charClassTable[idx];
        charClassTable[idx] |=  setFlags;
        charClassTable[idx] &= ~clrFlags;
        return res;
    }

    //! Возвращает предыдущее состояние флагов
    umba::tokenizer::CharClass invertCharClassFlags(CharType ch, umba::tokenizer::CharClass invertFlags)
    {
        std::size_t idx = umba::tokenizer::charToCharClassTableIndex(ch); // clamp 127
        UMBA_ASSERT(idx<charClassTable.size());

        auto res = charClassTable[idx];
        charClassTable[idx] ^=  invertFlags;
        return res;
    }

    umba::tokenizer::CharClass getCharClass(CharType ch) const
    {
        std::size_t idx = umba::tokenizer::charToCharClassTableIndex(ch); // clamp 127
        UMBA_ASSERT(idx<charClassTable.size());
        return charClassTable[idx];
    }

    umba::tokenizer::CharClass setCharClass(CharType ch, umba::tokenizer::CharClass cls)
    {
        std::size_t idx = umba::tokenizer::charToCharClassTableIndex(ch); // clamp 127
        UMBA_ASSERT(idx<charClassTable.size());
        std::swap(cls, charClassTable[idx]);
        return cls;
    }

    // Конец многострочного коментария ищется тупо как текст, поэтому он не входит в operatorsTrie (в отличие от начала)
    void setMultiLineCommentEndString(const StringType s) { multiLineCommentEndStr = s; }

    void setNumbersTrie  (const TrieVectorType &tv) { numbersTrie   = tv; }
    void setBracketsTrie (const TrieVectorType &tv) { bracketsTrie  = tv; }
    void setOperatorsTrie(const TrieVectorType &tv) { operatorsTrie = tv; }
    void setLiteralsTrie (const TrieVectorType &tv) { literalsTrie  = tv; }


    // Если у нас standalone токенизер, а не был получен из TokenizerBuilder, то
    // в указателях (которые лежат в payloadExtra) либо мусор, либо нули. Надо туда
    // установить актуальные указатели на парсеры строковых литералов
    // literalsTrie и charClass'ы соответствующих символов должны быть корректно установлены

    bool setLiteralParser(const StringType &strLiteralPrefix, ITokenizerLiteralParser* pParser, payload_type strLiteralPrefixTokenId)
    {
        UMBA_ASSERT(pParser);
        UMBA_ASSERT(!strLiteralPrefix.empty());

        auto tokenIdx = tokenTrieTraverse(literalsTrie, strLiteralPrefix.begin(), strLiteralPrefix.end());
        UMBA_ASSERT(tokenIdx!=trie_index_invalid); // Префикс должен быть в дереве

        // Флаг string_literal_prefix должен быть установлен у первого символа в последовательности префикса, мы по нему детектим префикс
        UMBA_ASSERT(umba::TheFlags(getCharClass(strLiteralPrefix[0])).oneOf(CharClass::string_literal_prefix));

        literalsTrie[tokenIdx].payloadExtra = reinterpret_cast<umba::tokenizer::payload_type>(pParser);

        return true; // !!! return'а не было, был забыт, а что и в каких случаях возращать - я уже не помню
    }


//------------------------------
protected: // methods - helpers - из "грязного" проекта, где я наговнякал первую версию, выносим лямбды.

        // numberCurrentIntValue        = 0;
        // numberCurrentFractionalValue = 0;
        // numberCurrentFractionalPower = 0;

    void resetNumberStateVals() const
    {
        numberPrefixIdx              = trie_index_invalid;
        numberExplicitBase           = 0;
        numberReadedDigits           = 0;
        allowedDigitCharClass        = CharClass::none;
        numbersBase                  = 0;

        numberCurrentIntValue        = 0;
        numberCurrentFractionalValue = 0;
        numberCurrentFractionalPower = 1;

        numberIntegerOverflow        = false;
        numberFractionalOverflow     = false;
        numberCurrentFractionalPower = 0;
    }


    void addNumberIntPartDigit(CharType ch) const
    {
        int d = utils::charToDigit(ch);
        //UMBA_ASSER(d>=0);
        if (d<0)
            return; // просто игнорим

        numberCurrentIntValue = utils::mulAndCheckOverflow(numberCurrentIntValue, (std::uint64_t)numbersBase, numberIntegerOverflow);
        numberCurrentIntValue = utils::addAndCheckOverflow(numberCurrentIntValue, (std::uint64_t)d          , numberIntegerOverflow);
    }

    void addNumberFloatPartDigit(CharType ch) const
    {
        int d = utils::charToDigit(ch);
        //UMBA_ASSER(d>=0);
        if (d<0)
            return; // просто игнорим

        numberCurrentFractionalValue = utils::mulAndCheckOverflow(numberCurrentFractionalValue, (std::uint64_t)numbersBase, numberFractionalOverflow);
        numberCurrentFractionalValue = utils::addAndCheckOverflow(numberCurrentFractionalValue, (std::uint64_t)d          , numberFractionalOverflow);
        numberCurrentFractionalPower = utils::mulAndCheckOverflow(numberCurrentFractionalPower, (std::uint64_t)numbersBase, numberFractionalOverflow);
    }

    IntegerNumericLiteralData makeIntegerLiteralData(iterator_type itEnd) const
    {
        IntegerNumericLiteralData res;
        res.hasSuffix      = false;
        res.suffixStartPos = itEnd;

        res.data      = numberCurrentIntValue;
        res.fOverflow = numberIntegerOverflow;

        return res;
    }

    FloatNumericLiteralData makeFloatLiteralData(iterator_type itEnd) const
    {
        FloatNumericLiteralData res;
        res.hasSuffix      = false;
        res.suffixStartPos = itEnd;

        // auto powerDivider = (typename FloatNumericLiteralData::DataValueType)utils::makePowerOf((typename FloatNumericLiteralData::DataValueType)numbersBase, numberCurrentFractionalPower, numberFractionalOverflow);
        //
        typename FloatNumericLiteralData::DataValueType fractionalPart = 0;
        if (numberCurrentFractionalPower>0)
        {
            fractionalPart = (typename FloatNumericLiteralData::DataValueType)numberCurrentFractionalValue / (typename FloatNumericLiteralData::DataValueType)numberCurrentFractionalPower;
        }

        res.data = (typename FloatNumericLiteralData::DataValueType)numberCurrentIntValue;
        res.data += fractionalPart;

        res.fIntegerOverflow    = numberIntegerOverflow;
        res.fFractionalOverflow = numberFractionalOverflow;

        return res;
    }


    //NOTE: !!! Надо ли semialpha проверять, не является ли она началом числового префикса? Наверное, не помешает

    void performStartReadingNumberLambda(CharType ch, InputIteratorType it) const
    {
        resetNumberStateVals();
        tokenStartIt                 = it;
        numberPrefixIdx              = tokenTrieFindNext(numbersTrie, trie_index_invalid, (token_type)ch);
        // numberTokenId                = 0;
        // numberReadedDigits           = 0;
        // numberExplicitBase           = 0;
        // numberCurrentIntValue        = 0;
        // numberCurrentFractionalValue = 0;
        // numberCurrentFractionalPower = 0;
        // numberIntegerOverflow        = false;
        // numberFractionalOverflow     = false;

        if (numberPrefixIdx!=trie_index_invalid) // Найдено начало префикса числа
        {
            st = TokenizerInternalState::stReadNumberPrefix;
            allowedDigitCharClass = CharClass::digit; // Потом всё равно зададим, после определения префикса
            numbersBase = 0;
        }
        else
        {
            st = TokenizerInternalState::stReadNumber;
            numbersBase = options.numberDefaultBase;
            allowedDigitCharClass = CharClass::digit;
            if (utils::isNumberHexDigitsAllowed(numbersBase))
                allowedDigitCharClass |= CharClass::xdigit;
            addNumberIntPartDigit(ch);
        }
    }

    bool performStartReadingOperatorLambda(CharType ch, InputIteratorType it) const
    {
        tokenStartIt = it;
        operatorIdx = tokenTrieFindNext(operatorsTrie, trie_index_invalid, (token_type)ch);
        if (operatorIdx==trie_index_invalid)
            return false;
        st = TokenizerInternalState::stReadOperator;
        return true;
    }

    bool performProcessBracketLambda(CharType ch, InputIteratorType it) const
    {
        auto idx = tokenTrieFindNext(bracketsTrie, trie_index_invalid, (token_type)ch);
        if (idx==trie_index_invalid)
            return false;
        if (!parsingHandlerLambda(bracketsTrie[idx].payload, it, it+1)) // выплюнули
            return false;
        st = TokenizerInternalState::stInitial;
        return true;
    }

    #include "umba/warnings/push_disable_spectre_mitigation.h"
    bool processCommentStartFromNonCommentedLambda(payload_type curPayload, InputIteratorType it) const
    {
        // tokenStartIt - указывает на начало оператора, в данном случае - оператора коментария
        // входной итератор it - указывает на позицию сразу после оператора

        commentTokenId = curPayload;

        if (utils::isSingleLineCommentToken(curPayload))
        {
            if (options.singleLineCommentOnlyAtBeginning && !curPosAtLineBeginning)
            {
                // Error? Or process as regular operator?
                //UMBA_TOKENIZER_TOKEN_OPERATOR_SINGLE_LINE_COMMENT_FLAG_AS_REGULAR_OPERATOR
                if (!parsingHandlerLambda(UMBA_TOKENIZER_TOKEN_OPERATOR_SINGLE_LINE_COMMENT_FLAG_AS_REGULAR_OPERATOR | commentTokenId, tokenStartIt, it)) // выплюнули как обычный оператор
                    return false;
                st = TokenizerInternalState::stInitial;
                return true;
            }
            //tokenStartIt   = it;
            commentStartIt = it; // Сохранили начало коментария без самого оператора начала коментария - текст коментария потом выдадим по окончании
            st = TokenizerInternalState::stReadSingleLineComment;
            operatorIdx = trie_index_invalid;
            return true;
        }
        else if (utils::isMultiLineCommentStartToken(curPayload))
        {
            //tokenStartIt   = it; // А тут бы надо оставить начало оператора !!!
            commentStartIt = it; // Сохранили начало коментария без самого оператора начала коментария - текст коментария потом выдадим по окончании
            st = TokenizerInternalState::stReadMultilineLineComment;
            operatorIdx = trie_index_invalid;
            // commentNestingLevel = 1; // Один вход сделали тут !!! Вложенные коментарии не поддерживаются
            return true;
        }
        else // коментарий, но не однострочный, и не начало многострочного - ошибка
        {
            return false;
        }
    }
    #include "umba/warnings/pop.h"

    [[nodiscard]] // Сменили void на bool, и теперь надо заставить везде проверять результат
    bool processEscapeSymbolLambda(InputIteratorType it) const
    {
        if (options.processLineContinuation)
        {
            tokenStartIt = it;
            stEscapeSaved = st;
            st = TokenizerInternalState::stContinuationWaitLinefeed;
        }
        else
        {
            if (!parsingHandlerLambda(UMBA_TOKENIZER_TOKEN_ESCAPE, it, it+1)) // выплюнули
                return false;
            st = TokenizerInternalState::stInitial;
        }

        return true;
    }

    bool processUnclassifiedCharsRawLambda(InputIteratorType it, InputIteratorType itEnd) const
    {
        if (options.unclassifiedCharsRaw)
        {
            if (!parsingHandlerLambda(UMBA_TOKENIZER_TOKEN_RAW_CHAR, it, it+1)) // выплюнули
                return false;
            st = TokenizerInternalState::stInitial;

            return true;
        }
        else
        {
            return unexpectedHandlerLambda(it, itEnd, __FILE__, __LINE__);
        }
    }

                    // if (options.unclassifiedCharsRaw)
                    // {
                    //  
                    // }
                    // else
                    // {
                    //     return unexpectedHandlerLambda(it, itEnd, __FILE__, __LINE__);
                    // }


    // ITokenizerLiteralParser*            pCurrentLiteralParser = 0;
    // payload_type                        literalTokenId = 0;
    // MessagesStringType                  externHandlerMessage;
    // TokenizerLiteralCharStringInserter  stringLiteralValueCollector;
    //
    // ITokenizerLiteralParser

    ITokenizerLiteralParser* checkIsLiteralPrefix(InputIteratorType it, InputIteratorType end, payload_type &literalToken) const
    {
        if (it==end)
            return 0;

        auto idx = tokenTrieFindFirst(literalsTrie, (token_type)*it);
        for(; idx!=trie_index_invalid && it!=end; ++it)
        {
            auto nextIdx = tokenTrieFindNext(literalsTrie, idx, (token_type)*it);
            if (nextIdx==trie_index_invalid) // Дошли до конца, проверяем предыдущий индекс
            {
                if (literalsTrie[idx].payload==payload_invalid)
                    return 0;
                literalToken = literalsTrie[idx].payload;
                auto pParser = reinterpret_cast<ITokenizerLiteralParser*>(literalsTrie[idx].payloadExtra);
                if (pParser)
                {
                    pParser->reset();
                    stringLiteralValueCollector.clear();
                }
                return pParser;
            }

            idx = nextIdx;
        }

        return 0;
    }


public: // methods - методы собственно разбора


// enum class TokenizerInternalState
// {
//     stInitial              = 0,
//     stReadSpace               ,
//     stReadIdentifier          ,
//     stReadNumberPrefix        ,
//     stReadNumber              ,
//     stReadNumberFloat         ,
//     stReadNumberMayBeFloat    ,
//     stReadOperator            ,
//     stReadSingleLineComment   ,
//     stReadMultilineLineComment,
//     stReadStringLiteral       ,
//     stContinuationWaitLinefeed
//
// };

    void tokenizeInit() const
    {
        st            = TokenizerInternalState::stInitial;
        stEscapeSaved = TokenizerInternalState::stInitial;

        rawModeCounter = 0;

        curPosAtLineBeginning = true;

        // Общее
        //tokenStartIt;

        // Операторы
        operatorIdx = trie_index_invalid;

        // Числовые литералы
        resetNumberStateVals();

        // Коментарии
        //commentStartIt;
        commentTokenId = 0;
        commentEndMatchIndex = 0;

        // Строковые литералы
        pCurrentLiteralParser = 0;
        literalTokenId = 0;
        externHandlerMessage.clear();
        stringLiteralValueCollector.clear();

        {
            MessagesStringType msg;
            InputIteratorType b, e;
            static_cast<const TBase*>(this)->hadleToken( false, UMBA_TOKENIZER_TOKEN_CTRL_RST, b, e
                                                       , EmptyData() // std::basic_string_view<value_type>()
                                                       , msg
                                                       );
        }
    }



    bool tokenizeFinalize(InputIteratorType itEnd) const
    {
        if (isInRawMode())
        {
             if (!parsingHandlerLambda(UMBA_TOKENIZER_TOKEN_CTRL_FIN, itEnd, itEnd))
                 return false;
             return true;
        }

        switch(st)
        {
            case TokenizerInternalState::stInitial  :
                 if (!parsingHandlerLambda(UMBA_TOKENIZER_TOKEN_CTRL_FIN, itEnd, itEnd))
                     return false;
                 return true;

            case TokenizerInternalState::stReadSpace:
                 if (!parsingHandlerLambda(UMBA_TOKENIZER_TOKEN_SPACE, tokenStartIt, itEnd))
                     return false;
                 if (!parsingHandlerLambda(UMBA_TOKENIZER_TOKEN_CTRL_FIN, itEnd, itEnd))
                     return false;
                 return true;

            case TokenizerInternalState::stReadIdentifier:
                 if (!parsingIdentifierHandlerLambda(tokenStartIt, itEnd)) // выплюнули
                     return false;
                 if (!parsingHandlerLambda(UMBA_TOKENIZER_TOKEN_CTRL_FIN, itEnd, itEnd))
                     return false;
                 return true;

            case TokenizerInternalState::stReadNumberPrefix:
            {
                 if (numberPrefixIdx==trie_index_invalid)
                     return unexpectedHandlerLambda(itEnd, itEnd, __FILE__, __LINE__);
                 {
                     // Надо проверить, является ли то, что уже есть, чисто числом
                     // int charToDigit(CharType ch)

                     CharType prefixDigits[64];
                     std::size_t idx = 0;
                     const std::size_t idxEnd = std::size(prefixDigits)-1;

                     bool prefixIsNumber     = true ;
                     tokenTrieBackTrace( numbersTrie
                                       , numberPrefixIdx
                                       , [&](token_type ch, const TrieNode& /*trieNode*/)
                                         {
                                             if (!utils::isDigitAllowed(ch, numbersBase))
                                                 prefixIsNumber = false;
                                             if (prefixIsNumber && idx!=idxEnd)
                                             {
                                                 prefixDigits[idx] = (CharType)ch; // в token_type тут лежат CharType'ы
                                                 ++idx;
                                                 prefixDigits[idx] = 0;
                                             }
                                         }
                                       );

                     if (!idx)
                        prefixIsNumber = false; // Нет цифр в префиксе

                     if (!prefixIsNumber)
                         return unexpectedHandlerLambda(itEnd, itEnd, __FILE__, __LINE__);

                     // !!! Тут надо преобразовать префикс в число, и поместить его в numberCurrentIntValue
                     std::reverse(&prefixDigits[0], &prefixDigits[idx]);
                     for(std::size_t idx2=0; idx2!=idx; ++idx2)
                     {
                         addNumberIntPartDigit(prefixDigits[idx2]);
                     }

                     if (!parsingNumberHandlerLambda(UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER, tokenStartIt, itEnd, itEnd)) // выплёвываем накопленное число как число без префикса, с системой счисления по умолчанию
                         return false;

                     if (!parsingHandlerLambda(UMBA_TOKENIZER_TOKEN_CTRL_FIN, itEnd, itEnd))
                         return false;
                     return true;
                 }
            }

            case TokenizerInternalState::stReadNumber:
                 // !!! Не забыть передать numberCurrentIntValue
                 if (numberTokenId==0 || numberTokenId==payload_invalid)
                 {
                     if (!parsingNumberHandlerLambda(UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER, tokenStartIt, itEnd, itEnd)) // выплёвываем накопленное число как число без префикса, с системой счисления по умолчанию
                         return false;
                 }
                 else
                 {
                     if (!parsingNumberHandlerLambda(numberTokenId, tokenStartIt, itEnd, itEnd)) // выплёвываем накопленное число с явно указанной системой счисления
                         return false;
                 }

                 if (!parsingHandlerLambda(UMBA_TOKENIZER_TOKEN_CTRL_FIN, itEnd, itEnd))
                     return false;

                 return true;

            case TokenizerInternalState::stReadNumberMayBeFloat:

                 // TODO: У нас был встречен символ разделяющий целую и дробную часть плавающего числа, но без целой части.
                 // Пока просто съедаем его, но вообще этот символ может быть оператором, надо это обработать
                 // Например, это может быть паскалевский `End.` (End с точкой)
                 return true;

            case TokenizerInternalState::stReadNumberFloat:
                 if (numberTokenId==0 || numberTokenId==payload_invalid)
                 {
                     if (!parsingFloatNumberHandlerLambda(UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER|UMBA_TOKENIZER_TOKEN_FLOAT_FLAG, tokenStartIt, itEnd, itEnd)) // выплёвываем накопленное число с системой счисления по умолчанию
                         return false;
                 }
                 else
                 {
                     if (!parsingFloatNumberHandlerLambda(numberTokenId|UMBA_TOKENIZER_TOKEN_FLOAT_FLAG, tokenStartIt, itEnd, itEnd)) // выплёвываем накопленное число с явно указанной системой счисления
                         return false;
                 }

                 if (!parsingHandlerLambda(UMBA_TOKENIZER_TOKEN_CTRL_FIN, itEnd, itEnd))
                     return false;

                 return true;

            case TokenizerInternalState::stReadOperator:
                 if (operatorIdx==trie_index_invalid)
                 {
                     reportPossibleUnknownOperatorLambda(tokenStartIt, itEnd);
                     return unexpectedHandlerLambda(itEnd, itEnd, __FILE__, __LINE__);
                 }
                 else
                 {
                     auto curPayload = operatorsTrie[operatorIdx].payload;
                     if (curPayload==payload_invalid) // текущий оператор нифига не оператор
                     {
                         reportPossibleUnknownOperatorLambda(tokenStartIt, itEnd);
                         return unexpectedHandlerLambda(itEnd, itEnd, __FILE__, __LINE__);
                     }

                     if (utils::isCommentToken(curPayload)) // на каждом операторе в обрабатываемом тексте у нас это срабатывает. Жирно или нет?
                     {
                         //TODO: !!! Надо уточнить, что за комент
                         if (!parsingHandlerLambda(UMBA_TOKENIZER_TOKEN_CTRL_FIN, itEnd, itEnd))
                             return false;
                         return true; // Пока считаем, что всё нормально
                     }

                     if (!parsingHandlerLambda(curPayload, tokenStartIt, itEnd)) // выплюнули текущий оператор
                         return false;
                     if (!parsingHandlerLambda(UMBA_TOKENIZER_TOKEN_CTRL_FIN, itEnd, itEnd))
                         return false;
                     return true;
                 }

            case TokenizerInternalState::stReadSingleLineComment:
                 if (!parsingCommentHandlerLambda(commentTokenId, tokenStartIt, itEnd, commentStartIt, itEnd))
                     return false;
                 if (!parsingHandlerLambda(UMBA_TOKENIZER_TOKEN_CTRL_FIN, itEnd, itEnd))
                     return false;
                 return true;

            case TokenizerInternalState::stReadMultilineLineComment:
                 return unexpectedHandlerLambda(itEnd, itEnd, __FILE__, __LINE__);

            case TokenizerInternalState::stReadStringLiteral:
                 return unexpectedHandlerLambda(itEnd, itEnd, __FILE__, __LINE__);

            case TokenizerInternalState::stContinuationWaitLinefeed:
                 return unexpectedHandlerLambda(itEnd, itEnd, __FILE__, __LINE__);

            default: return false;
        }

        //return true;
    }


    //----------------------------------------------------------------------------
    // Links
    // Is it possible to use goto with switch? - https://stackoverflow.com/questions/8202199/is-it-possible-to-use-goto-with-switch
    #include "umba/warnings/push_disable_spectre_mitigation.h"
    bool tokenize(InputIteratorType it, InputIteratorType itEnd) const
    {
        if (isInRawMode())
        {
            if (!parsingHandlerLambda(UMBA_TOKENIZER_TOKEN_RAW_CHAR, it, it+1)) // Сырые символы мы всегда отдельно выплёвываем
                return false;
             return true;
        }

        const auto ch = *it;
        UMBA_ASSERT(charClassTable.size()>charToCharClassTableIndex(ch));
        CharClass charClass = charClassTable[charToCharClassTableIndex(ch)];

        switch(st)
        {
            //------------------------------
            explicit_initial:
            case TokenizerInternalState::stInitial:
            {
                if (options.isFloatingPointSeparator<value_type>(ch))
                {
                    performStartReadingNumberLambda(ch, it);
                    st = TokenizerInternalState::stReadNumberMayBeFloat;
                    break;
                }

                if (umba::TheFlags(charClass).oneOf(CharClass::string_literal_prefix))
                {
                    pCurrentLiteralParser = checkIsLiteralPrefix(it, itEnd, literalTokenId);
                    if (pCurrentLiteralParser)
                    {
                        tokenStartIt = it;
                        st = TokenizerInternalState::stReadStringLiteral;
                        goto explicit_readstringliteral;
                    }
                }

                if (umba::TheFlags(charClass).oneOf(CharClass::linefeed))
                {
                    if (!parsingHandlerLambda(UMBA_TOKENIZER_TOKEN_LINEFEED, it, it+1)) // Перевод строки мы всегда отдельно выплёвываем
                        return false;
                }
                else if (umba::TheFlags(charClass).oneOf(CharClass::space))
                {
                    if (ch==(std::decay_t<decltype(ch)>)'\t') // Новый символ - табуляция?
                    {
                        if (!parsingHandlerLambda(options.getTabToken(), it, it+1)) // табуляцию выплёвываем по одному символу
                            return false;
                        st = TokenizerInternalState::stInitial;
                        return true;
                    }

                    tokenStartIt = it;
                    st = TokenizerInternalState::stReadSpace;
                }
                else if (umba::TheFlags(charClass).oneOf(CharClass::opchar))
                {
                    if (!performStartReadingOperatorLambda(ch, it))
                        return unexpectedHandlerLambda(it, itEnd, __FILE__, __LINE__);
                }
                else if (umba::TheFlags(charClass).oneOf(CharClass::identifier_first))
                {
                    tokenStartIt = it;
                    st = TokenizerInternalState::stReadIdentifier;
                }
                else if (umba::TheFlags(charClass).oneOf(CharClass::digit))
                {
                    performStartReadingNumberLambda(ch, it);
                }
                else if (umba::TheFlags(charClass).oneOf(CharClass::open, CharClass::close)) // Открывающая или закрывающая скобка
                {
                    if (!performProcessBracketLambda(ch, it))
                        return unexpectedHandlerLambda(it, itEnd, __FILE__, __LINE__);
                }
                else if (umba::TheFlags(charClass).oneOf(CharClass::semialpha))
                {
                    if (!parsingHandlerLambda(UMBA_TOKENIZER_TOKEN_SEMIALPHA, it, it+1)) // выплюнули
                        return false;
                }
                else if (umba::TheFlags(charClass).oneOf(CharClass::escape))
                {
                    if (!processEscapeSymbolLambda(it))
                        return false;
                }
                else
                {
                    return processUnclassifiedCharsRawLambda(it, itEnd);
                }

                //if ((charClass::linefeed))
            } break;


            //------------------------------
            case TokenizerInternalState::stReadSpace:
            {
                if (options.isFloatingPointSeparator<value_type>(ch))
                {
                    if (!parsingHandlerLambda(UMBA_TOKENIZER_TOKEN_SPACE, tokenStartIt, it))
                        return false;
                    st = TokenizerInternalState::stReadNumberMayBeFloat;
                    tokenStartIt = it;
                    resetNumberStateVals();
                    numbersBase = options.numberDefaultBase;
                    allowedDigitCharClass = CharClass::digit;
                    if (utils::isNumberHexDigitsAllowed(numbersBase))
                        allowedDigitCharClass |= CharClass::xdigit;
                    break;
                }

                if (umba::TheFlags(charClass).oneOf(CharClass::string_literal_prefix))
                {
                    pCurrentLiteralParser = checkIsLiteralPrefix(it, itEnd, literalTokenId);
                    if (pCurrentLiteralParser)
                    {
                        if (!parsingHandlerLambda(UMBA_TOKENIZER_TOKEN_SPACE, tokenStartIt, it))
                            return false;
                        tokenStartIt = it;
                        st = TokenizerInternalState::stReadStringLiteral;
                        goto explicit_readstringliteral;
                    }
                }

                if (umba::TheFlags(charClass).oneOf(CharClass::linefeed))
                {
                    if (!parsingHandlerLambda(UMBA_TOKENIZER_TOKEN_SPACE, tokenStartIt, it))
                        return false;
                    if (!parsingHandlerLambda(UMBA_TOKENIZER_TOKEN_LINEFEED, it, it+1)) // Перевод строки мы всегда отдельно выплёвываем
                        return false;
                    st = TokenizerInternalState::stInitial;
                }
                else if (umba::TheFlags(charClass).oneOf(CharClass::space))
                {
                    if (*tokenStartIt!=*it) // пробелы бывают разные, одинаковые мы стараемся коллекционировать, разные - выплевываем разными пачками
                    {
                        if (!parsingHandlerLambda(UMBA_TOKENIZER_TOKEN_SPACE, tokenStartIt, it)) // выплюнули
                            return false;

                        if (ch==(std::decay_t<decltype(ch)>)'\t') // Новый символ - табуляция?
                        {
                            if (!parsingHandlerLambda(options.getTabToken(), it, it+1)) // табуляцию выплёвываем по одному символу
                                return false;
                            st = TokenizerInternalState::stInitial;
                            return true;
                        }

                        // Новый символ - какая-то другая разновидность пробелов, но не

                        tokenStartIt = it; // Сохранили начало нового токена
                    }
                    else // пробел - такой же
                    {
                        break; // коллекционируем
                    }
                }
                else if (umba::TheFlags(charClass).oneOf(CharClass::opchar))
                {
                    if (!parsingHandlerLambda(UMBA_TOKENIZER_TOKEN_SPACE, tokenStartIt, it))
                        return false;
                    if (!performStartReadingOperatorLambda(ch, it))
                        return unexpectedHandlerLambda(it, itEnd, __FILE__, __LINE__);
                }
                else if (umba::TheFlags(charClass).oneOf(CharClass::identifier_first))
                {
                    if (!parsingHandlerLambda(UMBA_TOKENIZER_TOKEN_SPACE, tokenStartIt, it)) // выплюнули
                        return false;
                    tokenStartIt = it;
                    st = TokenizerInternalState::stReadIdentifier;
                }
                else if (umba::TheFlags(charClass).oneOf(CharClass::digit))
                {
                    if (!parsingHandlerLambda(UMBA_TOKENIZER_TOKEN_SPACE, tokenStartIt, it)) // выплюнули
                        return false;
                    performStartReadingNumberLambda(ch, it);
                }
                else if (umba::TheFlags(charClass).oneOf(CharClass::open, CharClass::close)) // Открывающая или закрывающая скобка
                {
                    if (!parsingHandlerLambda(UMBA_TOKENIZER_TOKEN_SPACE, tokenStartIt, it)) // выплюнули
                        return false;
                    if (!performProcessBracketLambda(ch, it))
                        return unexpectedHandlerLambda(it, itEnd, __FILE__, __LINE__);
                }
                else if (umba::TheFlags(charClass).oneOf(CharClass::semialpha))
                {
                    if (!parsingHandlerLambda(UMBA_TOKENIZER_TOKEN_SPACE, tokenStartIt, it)) // выплюнули
                        return false;
                    if (!parsingHandlerLambda(UMBA_TOKENIZER_TOKEN_SEMIALPHA, it, it+1)) // выплюнули
                        return false;
                    st = TokenizerInternalState::stInitial;
                }
                else if (umba::TheFlags(charClass).oneOf(CharClass::escape))
                {
                    if (!parsingHandlerLambda(UMBA_TOKENIZER_TOKEN_SPACE, tokenStartIt, it)) // выплюнули
                        return false;
                    if (!processEscapeSymbolLambda(it))
                        return false;
                }
                else
                {
                    //return unexpectedHandlerLambda(it, itEnd, __FILE__, __LINE__);
                    return processUnclassifiedCharsRawLambda(it, itEnd);
                }
            } break;


            //------------------------------
            case TokenizerInternalState::stReadIdentifier:
            {
                if (umba::TheFlags(charClass).oneOf(CharClass::identifier, CharClass::identifier_first))
                {
                    break; // коллекционируем
                }

                if (!parsingIdentifierHandlerLambda(tokenStartIt, it)) // выплюнули
                    return false;
                charClass = charClassTable[charToCharClassTableIndex(ch)]; // Перечитали значение класса символа - оно могло измениться
                tokenStartIt = it; // Сохранили начало нового токена

                if (umba::TheFlags(charClass).oneOf(CharClass::string_literal_prefix))
                {
                    pCurrentLiteralParser = checkIsLiteralPrefix(it, itEnd, literalTokenId);
                    if (pCurrentLiteralParser)
                    {
                        tokenStartIt = it;
                        st = TokenizerInternalState::stReadStringLiteral;
                        goto explicit_readstringliteral;
                    }
                }

                if (umba::TheFlags(charClass).oneOf(CharClass::linefeed))
                {
                    if (!parsingHandlerLambda(UMBA_TOKENIZER_TOKEN_LINEFEED, it, it+1)) // Перевод строки мы всегда отдельно выплёвываем
                        return false;
                    st = TokenizerInternalState::stInitial;
                }
                else if (umba::TheFlags(charClass).oneOf(CharClass::space))
                {
                    if (ch==(std::decay_t<decltype(ch)>)'\t') // Новый символ - табуляция?
                    {
                        if (!parsingHandlerLambda(options.getTabToken(), it, it+1)) // табуляцию выплёвываем по одному символу
                            return false;
                        st = TokenizerInternalState::stInitial;
                        return true;
                    }

                    tokenStartIt = it;
                    st = TokenizerInternalState::stReadSpace;
                }
                else if (umba::TheFlags(charClass).oneOf(CharClass::opchar))
                {
                    if (!performStartReadingOperatorLambda(ch, it))
                        return unexpectedHandlerLambda(it, itEnd, __FILE__, __LINE__);
                }
                else if (umba::TheFlags(charClass).oneOf(CharClass::open, CharClass::close)) // Открывающая или закрывающая скобка
                {
                    if (!performProcessBracketLambda(ch, it))
                        return unexpectedHandlerLambda(it, itEnd, __FILE__, __LINE__);
                }
                else if (umba::TheFlags(charClass).oneOf(CharClass::escape))
                {
                    if (!processEscapeSymbolLambda(it))
                        return false;
                }
                else
                {
                    //return unexpectedHandlerLambda(it, itEnd, __FILE__, __LINE__);
                    return processUnclassifiedCharsRawLambda(it, itEnd);
                }
            } break;


            //------------------------------
            case TokenizerInternalState::stReadNumberPrefix:
            {
                //NOTE: !!! У нас пока так: префикс числа начинается с любой цифры, потом могут следовать любые символы, после префикса - те символы, которые разрешены префиксом
                payload_type curPayload = payload_invalid;
                bool prefixIsNumber     = true ;
                bool requiresDigits     = false;
                CharType prefixDigits[64];
                std::size_t idx = 0;
                const std::size_t idxEnd = std::size(prefixDigits)-1;


                auto nextNumberPrefixIdx = tokenTrieFindNext(numbersTrie, numberPrefixIdx, (token_type)ch);
                if (nextNumberPrefixIdx!=trie_index_invalid)
                {
                    numberPrefixIdx = nextNumberPrefixIdx; // Всё в порядке, префикс числа продолжается
                    break;
                }
                else // Нет продолжения префикса, вероятно, он у нас окончился
                {
                    curPayload = numbersTrie[numberPrefixIdx].payload;
                    if (curPayload==payload_invalid) // текущий префикс нифига не префикс - в начале был префикс, а потом перестал им быть - может, банальная опечатка, и мы находимся в середине префикса, и кода payload'а у нас нету
                    {
                        // Надо проверить, является ли то, что уже есть, чисто числом

                        numbersBase = options.numberDefaultBase;
                        allowedDigitCharClass = CharClass::digit;
                        if (utils::isNumberHexDigitsAllowed(numbersBase))
                            allowedDigitCharClass |= CharClass::xdigit;

                        tokenTrieBackTrace( numbersTrie
                                          , numberPrefixIdx
                                          , [&](token_type ch, const TrieNode& /*trieNode*/)
                                            {
                                                if (!utils::isDigitAllowed(ch, numbersBase))
                                                    prefixIsNumber = false;

                                                if (prefixIsNumber && (idx!=idxEnd))
                                                {
                                                    prefixDigits[idx] = (CharType)ch; // в token_type тут лежат CharType'ы
                                                    ++idx;
                                                    prefixDigits[idx] = 0;
                                                }
                                            }
                                          );

                        if (!idx)
                           prefixIsNumber = false; // Нет цифр в префиксе

                        // !!! Тут ещё надо проработать - а вдруг это 0 с каким-то суффиксом?
                        // по идее, всю цепочку итераторов надо бы хранить где-то, чтобы в данном случае сбросить её пользователю, а не выдавать ошибку
                        if (!prefixIsNumber)
                            return unexpectedHandlerLambda(it, itEnd, __FILE__, __LINE__);

                        // В том, что считалось префиксом, только цифры
                        // Их надо всунуть в число
                        std::reverse(&prefixDigits[0], &prefixDigits[idx]);
                        for(std::size_t idx2=0; idx2!=idx; ++idx2)
                        {
                            addNumberIntPartDigit(prefixDigits[idx2]);
                        }

                        // А вот обработчик не вызываем
                        // parsingNumberHandlerLambda(UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER, tokenStartIt, it); // выплёвываем накопленное число как число без префикса, с системой счисления по умолчанию

                        st = TokenizerInternalState::stReadNumber;
                        goto explicit_readnumber; // надо обработать текущий символ
                    }

                    // Префикс нашелся
                    resetNumberStateVals();
                    requiresDigits        = utils::isNumberPrefixRequiresDigits(curPayload);
                    numberPrefixIdx       = trie_index_invalid; // сбрасываем индекс префикса, чтобы потом не париться

                    numberTokenId         = curPayload;
                    numberExplicitBase    = utils::numberPrefixGetBase(numberTokenId);
                    //numberReadedDigits    = 0;
                    numbersBase           = numberExplicitBase;
                    allowedDigitCharClass = CharClass::digit;
                    if (utils::isNumberHexDigitsAllowed(numbersBase))
                        allowedDigitCharClass |= CharClass::xdigit;

                    // Теперь тут у нас либо цифра, либо что-то другое
                    if (umba::TheFlags(charClass).oneOf(allowedDigitCharClass) && utils::isDigitAllowed(ch, numbersBase))
                    {
                        //NOTE: !!! Да, сразу после префикса у нас не может быть разделителя разрядов
                        st = TokenizerInternalState::stReadNumber; // Тут у нас годная цифра, продолжаем
                        numberReadedDigits = 1;
                        addNumberIntPartDigit(ch);
                        // !!! тут надо уже начинать подсчитывать std::uint64_t numberCurrentIntValue
                        break;
                    }

                    if (umba::TheFlags(charClass).oneOf(CharClass::escape))
                    {
                        // Проблема будет, если на половине префикса пришел эскейп. Или не будет. В общем, если и будет, то выше выдадут ошибку, и в здравом уме никто такое не напишет в сорцах
                        if (!parsingHandlerLambda(numberTokenId, tokenStartIt, it)) // выплёвываем префикс как число
                            return false;
                        if (!processEscapeSymbolLambda(it))
                            return false;
                        break;
                    }


                    if (requiresDigits)
                        return unexpectedHandlerLambda(it, itEnd, __FILE__, __LINE__); // нужна хоть одна цифра после префикса, а её нет

                    std::reverse(&prefixDigits[0], &prefixDigits[idx]);
                    for(std::size_t idx2=0; idx2!=idx; ++idx2)
                    {
                        addNumberIntPartDigit(prefixDigits[idx2]);
                    }

                    if (!parsingNumberHandlerLambda(numberTokenId, tokenStartIt, it, itEnd)) // выплёвываем префикс (он является годным числом и без доп цифр)
                        return false;

                    // у тут понеслась вся та же тема, как и в состоянии stInitial, только без обработки цифр

                    st = TokenizerInternalState::stInitial; // на всякий случай, если в stInitial обрабтчике состояние не переустанавливается, а подразумевается, что уже такое и есть
                    goto explicit_initial;

                }

            } break;


            //------------------------------
            explicit_readnumber:
            case TokenizerInternalState::stReadNumber:
            {
                if (options.isFloatingPointSeparator<value_type>(ch))
                {
                    st = TokenizerInternalState::stReadNumberFloat;
                    break;
                }

                if (umba::TheFlags(charClass).oneOf(allowedDigitCharClass) && utils::isDigitAllowed(ch, numbersBase))
                {
                    addNumberIntPartDigit(ch);
                    break; // Тут у нас годная цифра
                }

                if (options.isNumberRankSeparator(ch) && ch==(std::decay_t<decltype(ch)>)'\'')
                {
                    break; // Тут у нас разделитель разрядов
                }

                if (numberTokenId==0 || numberTokenId==payload_invalid)
                {
                    if (!parsingNumberHandlerLambda(UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER, tokenStartIt, it, itEnd)) // выплёвываем накопленное число как число без префикса, с системой счисления по умолчанию
                        return false;
                }
                else
                {
                    if (!parsingNumberHandlerLambda(numberTokenId, tokenStartIt, it, itEnd)) // выплёвываем накопленное число с явно указанной системой счисления
                        return false;
                }

                if (umba::TheFlags(charClass).oneOf(CharClass::escape))
                {
                    if (!processEscapeSymbolLambda(it))
                        return false;
                    break;
                }

                numberTokenId = 0;

                // Далее у нас всё как начальном состоянии
                st = TokenizerInternalState::stInitial; // на всякий случай, если в stInitial обрабтчике состояние не переустанавливается, а подразумевается, что уже такое и есть
                goto explicit_initial;

            } break;


            //------------------------------
            case TokenizerInternalState::stReadNumberMayBeFloat:
            {
                if (umba::TheFlags(charClass).oneOf(CharClass::escape))
                {
                    if (!processEscapeSymbolLambda(it)) // !!! Вот тут гавно какое-то получится, надо подумать
                        return false;
                    break;
                }

                if (umba::TheFlags(charClass).oneOf(allowedDigitCharClass) && utils::isDigitAllowed(ch, numbersBase))
                {
                    st = TokenizerInternalState::stReadNumberFloat; //!!! Добавил
                    break; // Тут у нас годная цифра
                }

                // Тут мы проверяем вторую точку (по первой мы попали в данное состояние)
                CharClass dotCharClass = charClassTable[charToCharClassTableIndex((std::decay_t<decltype(ch)>)'.')];

                if (umba::TheFlags(dotCharClass).oneOf(CharClass::opchar)) // Точка - операторный символ?
                {
                    if (!performStartReadingOperatorLambda((std::decay_t<decltype(ch)>)'.', tokenStartIt))
                        return unexpectedHandlerLambda(tokenStartIt, itEnd, __FILE__, __LINE__); // но у нас нет операторов, начинающихся с точки
                    st = TokenizerInternalState::stReadOperator; //!!! Добавил
                    goto explicit_readoperator; // Надо обработать текущий символ
                }
                else // точка - не операторный символ, и не начало дробной части плавающего числа (разделитель дробной части уже был)
                {
                    return unexpectedHandlerLambda(tokenStartIt, itEnd, __FILE__, __LINE__);
                }

                // !!! unreachable code
                //st = TokenizerInternalState::stReadNumberFloat;

                //goto explicit_readnumberfloat; // Текущий символ таки надо обработать

            } break;


            //------------------------------
            // explicit_readnumberfloat: // !!! После закоменчивания unreachable code выше эта метка стала uneferenced. Возможно, тут что-то не так
            case TokenizerInternalState::stReadNumberFloat:
            {
                if (umba::TheFlags(charClass).oneOf(CharClass::escape))
                {
                    if (!processEscapeSymbolLambda(it)) // !!! Вот тут гавно какое-то получится, надо подумать
                        return false;
                    break;
                }

                if (umba::TheFlags(charClass).oneOf(allowedDigitCharClass) && utils::isDigitAllowed(ch, numbersBase))
                {
                    addNumberFloatPartDigit(ch);
                    break; // Тут у нас годная цифра
                }

                if (numberTokenId==0 || numberTokenId==payload_invalid)
                {
                    if (!parsingFloatNumberHandlerLambda(UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER|UMBA_TOKENIZER_TOKEN_FLOAT_FLAG, tokenStartIt, it, itEnd)) // выплёвываем накопленное число с системой счисления по умолчанию
                        return false;
                }
                else
                {
                    if (!parsingFloatNumberHandlerLambda(numberTokenId|UMBA_TOKENIZER_TOKEN_FLOAT_FLAG, tokenStartIt, it, itEnd)) // выплёвываем накопленное число с явно указанной системой счисления
                        return false;
                }

                st = TokenizerInternalState::stInitial; // на всякий случай, если в stInitial обрабтчике состояние не переустанавливается, а подразумевается, что уже такое и есть
                goto explicit_initial;

            } break;


            //------------------------------
            explicit_readoperator:
            case TokenizerInternalState::stReadOperator:
            {
                if (umba::TheFlags(charClass).oneOf(CharClass::escape))
                {
                    if (!processEscapeSymbolLambda(it)) // !!! Вот тут гавно какое-то получится, надо подумать
                        return false;
                    break;
                }

                if (umba::TheFlags(charClass).oneOf(CharClass::opchar))
                {
                    auto nextOperatorIdx = tokenTrieFindNext(operatorsTrie, operatorIdx, (token_type)ch);

                    if (nextOperatorIdx!=trie_index_invalid)
                    {
                        operatorIdx = nextOperatorIdx; // Всё в порядке, оператор продолжается
                    }
                    else
                    {
                        // Нет продолжения, у нас был, вероятно, полностью заданный оператор
                        auto curPayload = operatorsTrie[operatorIdx].payload;
                        if (curPayload==payload_invalid) // текущий оператор нифига не оператор
                        {
                            reportPossibleUnknownOperatorLambda(tokenStartIt, it);
                            return unexpectedHandlerLambda(it, itEnd, __FILE__, __LINE__);
                        }
                        if (utils::isCommentToken(curPayload)) // на каждом операторе в обрабатываемом тексте у нас это срабатывает. Жирно или нет?
                        {
                            if (!processCommentStartFromNonCommentedLambda(curPayload, it))
                                return unexpectedHandlerLambda(it, itEnd, __FILE__, __LINE__);
                            if (utils::isMultiLineCommentStartToken(curPayload))
                                goto explicit_readmultilinecomment;
                            else
                                goto explicit_readsinglelinecomment;
                            //break;
                        }

                        if (!parsingHandlerLambda(curPayload, tokenStartIt, it)) // выплюнули текущий оператор
                            return false;
                        tokenStartIt = it; // Сохранили начало нового токена

                        operatorIdx = tokenTrieFindNext(operatorsTrie, trie_index_invalid, (token_type)ch); // Начали поиск нового оператора с начала
                        if (operatorIdx==trie_index_invalid)
                        {
                            reportPossibleUnknownOperatorLambda(it,it+1);
                            return unexpectedHandlerLambda(it, itEnd, __FILE__, __LINE__);
                        }
                    }
                    break; // коллекционируем символы оператора
                }

                // Заканчиваем обработку оператора на неоператорном символе

                if (operatorIdx==trie_index_invalid) // Текущий оператор почему-то не оператор
                    return unexpectedHandlerLambda(it, itEnd, __FILE__, __LINE__);

                if (operatorsTrie[operatorIdx].payload==payload_invalid)
                {
                    reportPossibleUnknownOperatorLambda(tokenStartIt, it);
                    return unexpectedHandlerLambda(it, itEnd, __FILE__, __LINE__);
                }

                auto curPayload = operatorsTrie[operatorIdx].payload;

                // Всё тоже самое, что и выше, наверное, надо как-то это потом вынести
                if (utils::isCommentToken(curPayload)) // на каждом операторе в обрабатываемом тексте у нас это срабатывает. Жирно или нет?
                {
                    if (!processCommentStartFromNonCommentedLambda(curPayload, it))
                        return unexpectedHandlerLambda(it, itEnd, __FILE__, __LINE__);
                    //break;
                    //goto explicit_readsinglelinecomment;
                    if (utils::isMultiLineCommentStartToken(curPayload))
                        goto explicit_readmultilinecomment;
                    else
                        goto explicit_readsinglelinecomment;
                }

                if (!parsingHandlerLambda(curPayload, tokenStartIt, it)) // выплюнули
                    return false;
                tokenStartIt = it; // Сохранили начало нового токена

                // Далее у нас всё как начальном состоянии
                st = TokenizerInternalState::stInitial; // на всякий случай, если в stInitial обрабтчике состояние не переустанавливается, а подразумевается, что уже такое и есть
                goto explicit_initial;

            } break;


            //------------------------------
            explicit_readstringliteral:
            case TokenizerInternalState::stReadStringLiteral:
            {
                // литералы сами ловят хвостовой escape, если умеют
                externHandlerMessage.clear();
                auto res = pCurrentLiteralParser->parseChar(it, itEnd, &stringLiteralValueCollector, &externHandlerMessage);

                if (res==StringLiteralParsingResult::error)
                {
                    reportStringLiteralMessageLambda(true /* bErr */ , it, externHandlerMessage);
                    return unexpectedHandlerLambda(it, itEnd, __FILE__, __LINE__);
                }

                if (res==StringLiteralParsingResult::warnContinue || res==StringLiteralParsingResult::warnStop)
                {
                   reportStringLiteralMessageLambda(false /* !bErr */ , it, externHandlerMessage);
                }

                if (res==StringLiteralParsingResult::okStop || res==StringLiteralParsingResult::warnStop)
                {
                    if (!parsingStringLiteralHandlerLambda(literalTokenId, tokenStartIt, it+1, makeStringView(stringLiteralValueCollector.str()))) // выплюнули текущий литерал
                        return false;
                    st = TokenizerInternalState::stInitial;
                }

            } break;


            //------------------------------
            explicit_readsinglelinecomment:
            case TokenizerInternalState::stReadSingleLineComment:
            {

                if (umba::TheFlags(charClass).oneOf(CharClass::escape))
                {
                    auto nextIt = it+1;
                    if (nextIt==itEnd)
                    {
                        if (!parsingCommentHandlerLambda(commentTokenId, tokenStartIt, it, commentStartIt, it))
                            return false;
                        if (!processEscapeSymbolLambda(it /* , stInitial */ ))
                            return false;
                        break;
                    }

                    CharClass nextCharClass = charClassTable[charToCharClassTableIndex(*nextIt)];
                    if (umba::TheFlags(nextCharClass).oneOf(CharClass::linefeed))
                    {
                        if (!parsingCommentHandlerLambda(commentTokenId, tokenStartIt, it, commentStartIt, it))
                            return false;
                        if (!processEscapeSymbolLambda(it /* , stInitial */ ))
                            return false;
                        break;
                    }

                    // Если не конец текста и не перевод строки - то слэш просто внутри строки коментария, и ничего делать не надо
                    break;
                }
                else if (umba::TheFlags(charClass).oneOf(CharClass::linefeed))
                {
                    //TODO: !!! Разобраться с continuation
                    if (!parsingCommentHandlerLambda(commentTokenId, tokenStartIt, it, commentStartIt, it))
                        return false;
                    if (!parsingHandlerLambda(UMBA_TOKENIZER_TOKEN_LINEFEED, it, it+1)) // Перевод строки мы всегда отдельно выплёвываем
                        return false;
                    st = TokenizerInternalState::stInitial;
                }
                else
                {
                    // Иначе - ничего не делаем
                }

            } break;


            //------------------------------
            explicit_readmultilinecomment:
            case TokenizerInternalState::stReadMultilineLineComment:
            {
                // auto nextOperatorIdx = tokenTrieFindNext(operatorsTrie, operatorIdx, (token_type)ch);
                if (multiLineCommentEndStr.empty())
                    return unexpectedHandlerLambda(it, itEnd, __FILE__, __LINE__);

                for(std::size_t i=0; i!=2; ++i)
                {
                    if (multiLineCommentEndStr[commentEndMatchIndex]==*it)
                    {
                        if (!commentEndMatchIndex)
                            commentEndStartIt = it;

                        ++commentEndMatchIndex;

                        if (commentEndMatchIndex>=multiLineCommentEndStr.size())
                        {
                            // Нашли
                            if (!parsingCommentHandlerLambda(commentTokenId, tokenStartIt, it+1, commentStartIt, commentEndStartIt)) // выплюнули текст коментария
                                return false;
                            
                            commentEndMatchIndex = 0;
                            st = TokenizerInternalState::stInitial;
                        }

                        break;
                    }

                    commentEndMatchIndex = 0;
                }

            } break;


            //------------------------------
            case TokenizerInternalState::stContinuationWaitLinefeed:
            {
                // stReadSingleLineComment, stInitial
                if (umba::TheFlags(charClass).oneOf(CharClass::linefeed))
                {
                    if (!parsingHandlerLambda(UMBA_TOKENIZER_TOKEN_LINE_CONTINUATION, tokenStartIt, it+1))
                        return false;

                    st = stEscapeSaved;
                    tokenStartIt = it+1;

                    // Если у нас был однострочный комент, и linefeed сразу после слеша, то выплёвываем коментарий (уже), выплёвываем continuation,
                    // и затем не возвращаемся в предыдущее состояние, а в самое начальное
                    // таким образом, если следующий после continuation токен (помимо пробелов) будет не коментарий
                    // то всё будет отработано нормально, но можно выдать варнинг, что однострок использует continuation
                    if (st==TokenizerInternalState::stReadSingleLineComment)
                    {
                        st = TokenizerInternalState::stInitial;
                    }
                    break;
                }
                else
                {
                    if (!parsingHandlerLambda(UMBA_TOKENIZER_TOKEN_ESCAPE, tokenStartIt, it))
                        return false;
                    st = TokenizerInternalState::stInitial;
                    goto explicit_initial;
                }
            }


            //------------------------------
            default:
            {
                // У нас тут необработанное состояние, вообще-то мы сюда не должны попадать
                return unexpectedHandlerLambda(it, itEnd, __FILE__, __LINE__);
                //return processUnclassifiedCharsRawLambda(it, itEnd);
                
            }

        } // end switch

        return true;
    }
    #include "umba/warnings/pop.h"


protected:

    void checkLineStart(payload_type tokenType) const
    {
        if (tokenType==UMBA_TOKENIZER_TOKEN_LINEFEED)
            curPosAtLineBeginning = true; // Для поддержки однострочного комента только в начале строки
        else if (tokenType==UMBA_TOKENIZER_TOKEN_SPACE) {} // Пробелы - пропускаем, они ничего не меняют
        else
            curPosAtLineBeginning = false; // Для поддержки однострочного комента только в начале строки
    }


public:

    static
    std::basic_string_view<value_type> makeStringView( const umba::iterator::TextPositionCountingIterator<CharType> b, const umba::iterator::TextPositionCountingIterator<CharType> &e)
    {
        return umba::iterator::makeStringView(b,e);
    }

    template<typename GenericIteratorType>
    static
    std::basic_string_view<value_type> makeStringView(GenericIteratorType b, GenericIteratorType e)
    {
        return std::basic_string_view<value_type>(&*b, distance( b, e ));
    }

    static
    std::basic_string_view<typename StringType::value_type> makeStringView(const StringType &str)
    {
        return std::basic_string_view<value_type>(&str[0], str.size());
    }


//------------------------------
protected: // methods - хандлеры из "грязного" проекта, где я наговнякал первую версию, выносим лямбды.
           // Это переходники к обработчикам, которые будут в дочернем классе
           // Нормальные имена потом сделаем, если захочется
           // Все хелперы и вся обработка трогают только то, что отвечает за состояние парсера. Все входные таблицы/деревья,
           // опции и тп - оно не должно трогать, поэтому состояние - mutable, а методы логически константные

    //! Эта "лямбда" вызывается при распознавании очередного токена. Тут надо вызвать метода класса-наследника, пока не будем
    /* @param b, e - итераторы начала и конца текста токена

       Итераторы передаем по значению. Предполагается, что у нас будет использоваться TextPositionCountingIterator, он жирный,
       но, теоретически, можно использовать и обычные итераторы или указатели, если не нужна информация о строке и позиции в ней.
       В принципе, TextPositionCountingIterator можно сделать полегче на 8 байт (24 vs 32 сейчас на x86)

       Для некоторых типов токенов нужно отдавать дополнительную информацию:
       - для строковых литералов - разобранную строку
       - для цифровых - пока хз что
       - для коментариев - текст без маркеров коментария

       Т.е. нужна вторая пара итераторов

       во внешний обработчик будем передавать string_view с полным варантом всегда
       и string_view с доп инфой.
       если доп инфы нет - второй string_view будет пустой - begin==end

    */
    [[nodiscard]] // Сменили void на bool, и теперь надо заставить везде проверять результат
    bool parsingHandlerLambda(payload_type tokenType, InputIteratorType inputDataBegin, InputIteratorType inputDataEnd) const
    {
        if (tokenType!=UMBA_TOKENIZER_TOKEN_CTRL_FIN && inputDataBegin==inputDataEnd)
            return true;

        MessagesStringType msg;
        bool bRes = static_cast<const TBase*>(this)->hadleToken( curPosAtLineBeginning, tokenType, inputDataBegin, inputDataEnd
                                                               , EmptyData() // std::basic_string_view<value_type>()
                                                               , msg
                                                               );
        checkLineStart(tokenType);

        if (!bRes)
        {
            reportHandleTokenErrorLambda(tokenType, inputDataBegin, inputDataEnd, msg);
        }

        return bRes;
    }

    [[nodiscard]]
    bool parsingNumberHandlerLambda(payload_type tokenType, InputIteratorType inputDataBegin, InputIteratorType inputDataEnd, InputIteratorType itEnd) const
    {
        if (tokenType!=UMBA_TOKENIZER_TOKEN_CTRL_FIN && inputDataBegin==inputDataEnd)
            return true;
        MessagesStringType msg;
        bool bRes = static_cast<const TBase*>(this)->hadleToken( curPosAtLineBeginning, tokenType, inputDataBegin, inputDataEnd
                                                               , makeIntegerLiteralData(itEnd)
                                                               , msg
                                                               );
        checkLineStart(tokenType);

        if (!bRes)
        {
            reportHandleTokenErrorLambda(tokenType, inputDataBegin, inputDataEnd, msg);
        }

        return bRes;
    }

    [[nodiscard]]
    bool parsingFloatNumberHandlerLambda(payload_type tokenType, InputIteratorType inputDataBegin, InputIteratorType inputDataEnd, InputIteratorType itEnd) const
    {
        if (tokenType!=UMBA_TOKENIZER_TOKEN_CTRL_FIN && inputDataBegin==inputDataEnd)
            return true;
        MessagesStringType msg;
        bool bRes = static_cast<const TBase*>(this)->hadleToken( curPosAtLineBeginning, tokenType, inputDataBegin, inputDataEnd
                                                               , makeFloatLiteralData(itEnd)
                                                               , msg
                                                               );
        checkLineStart(tokenType);

        if (!bRes)
        {
            reportHandleTokenErrorLambda(tokenType, inputDataBegin, inputDataEnd, msg);
        }

        return bRes;
    }

    [[nodiscard]] // Сменили void на bool, и теперь надо заставить везде проверять результат
    bool parsingCommentHandlerLambda( payload_type tokenType, InputIteratorType inputDataBegin, InputIteratorType inputDataEnd
                                    , InputIteratorType parsedDataBegin, InputIteratorType parsedDataEnd
                                    ) const
    {
        if (tokenType!=UMBA_TOKENIZER_TOKEN_CTRL_FIN && inputDataBegin==inputDataEnd)
            return true;
        MessagesStringType msg;
        bool bRes = static_cast<const TBase*>(this)->hadleToken( curPosAtLineBeginning, tokenType, inputDataBegin, inputDataEnd
                                                               , CommentData{makeStringView(parsedDataBegin, parsedDataEnd)}
                                                               , msg
                                                               );
        checkLineStart(tokenType);

        if (!bRes)
        {
            reportHandleTokenErrorLambda(tokenType, inputDataBegin, inputDataEnd, msg);
        }

        return bRes;
    }

    [[nodiscard]] // Сменили void на bool, и теперь надо заставить везде проверять результат
    bool parsingStringLiteralHandlerLambda( payload_type tokenType, InputIteratorType inputDataBegin, InputIteratorType inputDataEnd
                             , std::basic_string_view<value_type> parsedData
                             ) const
    {
        if (tokenType!=UMBA_TOKENIZER_TOKEN_CTRL_FIN && inputDataBegin==inputDataEnd)
            return true;
        MessagesStringType msg;
        bool bRes = static_cast<const TBase*>(this)->hadleToken(curPosAtLineBeginning, tokenType
                                                               , inputDataBegin, inputDataEnd
                                                               , StringLiteralData{parsedData}
                                                               , msg
                                                               );
        // TokenParsedData StringLiteralData
        checkLineStart(tokenType);

        if (!bRes)
        {
            reportHandleTokenErrorLambda(tokenType, inputDataBegin, inputDataEnd, msg);
        }

        return bRes;
    }

    [[nodiscard]] // !!! Пока IdentifierData задаем как вьюшку от итераторов, но вообще идентификатор надо бы сохранять в буфере, чтобы корректно обрабатывать linefeed escap'ы
    bool parsingIdentifierHandlerLambda( InputIteratorType inputDataBegin, InputIteratorType inputDataEnd
                                       ) const
    {
        if (inputDataBegin==inputDataEnd)
            return true;
        MessagesStringType msg;
        bool bRes = static_cast<const TBase*>(this)->hadleToken(curPosAtLineBeginning, UMBA_TOKENIZER_TOKEN_IDENTIFIER
                                                               , inputDataBegin, inputDataEnd
                                                               , IdentifierData{makeStringView(inputDataBegin, inputDataEnd)}
                                                               , msg
                                                               );
        // TokenParsedData StringLiteralData
        checkLineStart(UMBA_TOKENIZER_TOKEN_IDENTIFIER);

        if (!bRes)
        {
            reportHandleTokenErrorLambda(UMBA_TOKENIZER_TOKEN_IDENTIFIER, inputDataBegin, inputDataEnd, msg);
        }

        return bRes;
    }


    bool unexpectedHandlerLambda(InputIteratorType it, InputIteratorType itEnd, const char* srcFile, int srcLine) const
    {
        return static_cast<const TBase*>(this)->hadleUnexpected(it, itEnd, srcFile, srcLine);
    }

    // Дополнительное сообщение о неизвестном операторе, перед вызовом unexpectedHandlerLambda - типа чуть чуть улучшили диагностику
    void reportPossibleUnknownOperatorLambda(InputIteratorType b, InputIteratorType e) const
    {
        static_cast<const TBase*>(this)->reportPossibleUnknownOperator(b, e);
    }

    void reportPossibleUnknownOperatorIntegerOverflowLambda(InputIteratorType b, InputIteratorType e) const
    {
        //static_cast<const TBase*>(this)->reportPossibleUnknownOperator(b, e);
    }

    // Либо предупреждение, либо сообщение об ошибке от парсера литералов
    void reportStringLiteralMessageLambda(bool bErr, InputIteratorType it, const MessagesStringType &msg) const
    {
        static_cast<const TBase*>(this)->reportStringLiteralMessage(bErr, it, msg);
    }

    void reportHandleTokenErrorLambda(payload_type tokenType, InputIteratorType inputDataBegin, InputIteratorType inputDataEnd, const MessagesStringType &msg) const
    {
        static_cast<const TBase*>(this)->reportHandleTokenError(tokenType, inputDataBegin, inputDataEnd, msg);
    }


}; // class TokenizerBaseImpl
#include "umba/warnings/pop.h"



} // namespace tokenizer
} // namespace umba

