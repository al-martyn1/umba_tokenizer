#pragma once

#ifndef UMBA_TOKENIZER_H__DCAEEDE0_7624_4E47_9919_08460EF65A3B__
    #error "Do not include this file directly, include 'umba/tokenizer.h header instead'"
#endif

#include "umba/warnings/push_disable_rel_inc_contains_dbldot.h"
#include "umba/rule_of_five.h"
#include "umba/string_plus.h"
#include "umba/warnings/pop.h"



// umba::tokenizer::
namespace umba {
namespace tokenizer {



//----------------------------------------------------------------------------
enum class StringLiteralParsingResult : unsigned
{
    okContinue    , // Нормас, продолжаем парсить
    okStop        , // Нормас, обнаружен конец литерала
    warnContinue  , // Предупреждение - некорректная строка, но обработку можно продолжить
    warnStop      , // Предупреждение - некорректная строка, и производим нормальную обработку парсинга
    error           // Ошибка, останавливает парсинг
};

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
template<typename CharType>
struct ITokenizerLiteralCharInserter
{
    using difference_type = std::ptrdiff_t;
    using value_type = CharType;

    virtual ~ITokenizerLiteralCharInserter() {}

    // Тут такой вопрос - может, сделать перегрузки для разных типов char?
    virtual void insert(CharType ch) = 0;
};

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
template<typename CharType>
struct TokenizerLiteralCharNulInserterImpl : public ITokenizerLiteralCharInserter<CharType>
{
    virtual void insert(CharType ch) override
    {
        UMBA_USED(ch);
    }
};

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
template< typename CharType
        , typename StringType = std::basic_string<CharType>
        >
struct TokenizerLiteralCharStringRefInserter : public ITokenizerLiteralCharInserter<CharType>
{

protected:
    StringType  &strAppendTo;
    bool        insertionDisabled = false;

public:

    TokenizerLiteralCharStringRefInserter(StringType &str) : strAppendTo(str) {}

    UMBA_RULE_OF_FIVE(TokenizerLiteralCharStringRefInserter, delete, default, default, default, default);

    void  clear() { strAppendTo.clear(); }
    const StringType& str() const { return strAppendTo; }
    void  disableInsertion(bool bDisable) { insertionDisabled = bDisable; }

    virtual void insert(CharType ch) override
    {
        if (!insertionDisabled)
            strAppendTo.append(1, ch);
    }
};

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
template< typename CharType
        , typename StringType = std::basic_string<CharType>
        >
struct TokenizerLiteralCharStringInserter : public ITokenizerLiteralCharInserter<CharType>
{
protected:
    StringType  strAppendTo;
    bool        insertionDisabled = false;

public:

    UMBA_RULE_OF_FIVE(TokenizerLiteralCharStringInserter, default, default, default, default, default);

    void  clear() { strAppendTo.clear(); }
    const StringType& str() const { return strAppendTo; }
    void  disableInsertion(bool bDisable) { insertionDisabled = bDisable; }

    virtual void insert(CharType ch) override
    {
        if (!insertionDisabled)
            strAppendTo.append(1, ch);
    }
};

//----------------------------------------------------------------------------





//----------------------------------------------------------------------------

// Вообще, LiteralProcessor'ы должны не только проверять корректность строки для токенизера,
// но и уметь разбирать, может, даже в то же самое время, как и парсить


// Токенизер получает и хранит сырые указатели на литерал парсеры, но не владеет ими. Пользовательский
// код сам должен обеспечивать достаточное время жизни объектам парсеров, хоть в куче, хоть на стеке.
// Можно было бы попробовать заморочиться, и сделать, чтобы всё работало в компайл-тайме, чтобы запихывать
// эффективно в микроконтроллер, но одна из идей токенизера - это возможность настройки в рантайме, например,
// для раскрашивания кода произвольных языков программирования в текстовом редакторе.

// Хорошо ли использовать для сообщений тот же CharType, что и для обрабатываемых данных?
// Или сообщения будем всегда, например, в std::string помещать?
// Наверное, второе, а потом где-то выше уровнем, можно и локализованное сообщение подогнать.
// Исходим из того, что MessageStringType всегда строка однобайтовых символов


//----------------------------------------------------------------------------
// Или не задаём в компайл-тайме inserter?
// template< typename CharType
//         , typename InputIteratorType    = umba::iterator::TextPositionCountingIterator<CharType>
//         , typename InserterIteratorType = umba::null_insert_iterator<CharType>
//         >
template< typename CharType
        , typename MessageStringType    = std::string // std::basic_string<CharType>
        , typename InputIteratorType    = umba::iterator::TextPositionCountingIterator<CharType>
        , typename InputIteratorTraits  = umba::iterator::TextIteratorTraits<InputIteratorType>
        >
struct ITokenizerLiteralParser
{
    using value_type = CharType;

    virtual ~ITokenizerLiteralParser() {}

    virtual void reset() = 0;
    virtual StringLiteralParsingResult parseChar(InputIteratorType it, InputIteratorType itEnd, ITokenizerLiteralCharInserter<CharType> *pInserter, MessageStringType *pMsg) = 0;

protected:

    static
    void setMessage(MessageStringType *pMsg, const MessageStringType &msg)
    {
        if (pMsg)
           *pMsg = msg;
    }

    static
    void insertChar(ITokenizerLiteralCharInserter<CharType> *pInserter, CharType ch)
    {
        if (pInserter)
            pInserter->insert(ch);
    }

}; // struct ITokenizerLiteralParser

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
//! Литерал парсер, который тупо заканчивает разбор на закрывающей кавычке, никаких escape-последовательностей не умеет, может обрабатывать как quot, так и apos литералы, quot может быть внутри apos литерала, и наоборот, но quot не может быть внутри quot литерала
/*! Литерал парсер... Что я тут хотел написать?
*/

template< typename CharType
        , typename MessageStringType    = std::string // std::basic_string<CharType>
        , typename InputIteratorType    = umba::iterator::TextPositionCountingIterator<CharType>
        , typename InputIteratorTraits  = umba::iterator::TextIteratorTraits<InputIteratorType>
        >
class SimpleQuotedStringLiteralParser : public ITokenizerLiteralParser<CharType, MessageStringType, InputIteratorType, InputIteratorTraits>
{

protected:

    enum State
    {
        stInitial,
        stRead
    };

    CharType     quotStart  = 0;
    CharType     quotEnd    = 0;
    bool         quotPaired = false;
    std::size_t  quotLevel  = 0;
    State        st         = stInitial;

    static
    CharType getQuotEnd(CharType qs)
    {
        switch(qs)
        {
            case (CharType)'<': return '>';
            case (CharType)'{': return '}';
            case (CharType)'[': return ']';
            case (CharType)'(': return ')';
            default: return qs;
        }
    }

public:

    UMBA_RULE_OF_FIVE(SimpleQuotedStringLiteralParser, default, default, default, default, default);

    void reset()
    {
        quotStart  = 0;
        quotEnd    = 0;
        quotPaired = false;
        quotLevel  = 0;
        st         = stInitial;
    }

    virtual StringLiteralParsingResult parseChar(InputIteratorType it, InputIteratorType itEnd, ITokenizerLiteralCharInserter<CharType> *pInserter, MessageStringType *pMsg) override
    {
        UMBA_USED(itEnd);
        UMBA_USED(pMsg);
        if (st==stInitial)
        {
            quotStart  = *it;
            quotEnd    = getQuotEnd(quotStart);
            quotPaired = quotStart!=quotEnd;
            quotLevel  = 1u;
            st         = stRead;
        }
        else
        {
            if (!quotPaired)
            {
                if (*it==quotEnd)
                {
                    return StringLiteralParsingResult::okStop;
                }
                else
                {
                    this->insertChar(pInserter, *it);
                }
            }
            else
            {
                if (*it==quotStart)
                {
                    ++quotLevel;
                }
                else if (*it==quotEnd)
                {
                    --quotLevel;
                    if (quotLevel==0)
                    {
                        return StringLiteralParsingResult::okStop;
                    }
                    else
                    {
                        this->insertChar(pInserter, *it);
                    }
                }
            }

        }

        return StringLiteralParsingResult::okContinue;
    }


}; // class SimpleQuotedStringLiteralParser

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
//! SimpleQuoted, хотя и CppEscaped - потому, что он не парсит сырые литералы, и всякие другие новые модные плюсовые строковые литералы
//! ExtraEscapes позволяют заменять кастомные escape последовательности на произвольные строки
#include "umba/warnings/push_disable_padding_added.h"
template< typename CharType
        , typename MessageStringType    = std::string // std::basic_string<CharType>
        , typename InputIteratorType    = umba::iterator::TextPositionCountingIterator<CharType>
        , typename InputIteratorTraits  = umba::iterator::TextIteratorTraits<InputIteratorType>
        , typename ExtraEscapesMapType  = std::unordered_map<CharType, std::basic_string<CharType> >
        >
class CppEscapedSimpleQuotedStringLiteralParser : public ITokenizerLiteralParser<CharType, MessageStringType, InputIteratorType, InputIteratorTraits>
{

    using InterfaceParentType = ITokenizerLiteralParser<CharType, MessageStringType, InputIteratorType>;

protected:

    // Про плюсовые литералы ссылок
    // String literal - https://en.cppreference.com/w/cpp/language/string_literal
    // Пользовательские литералы в C++11 - https://habr.com/ru/articles/140357/
    // Escape-последовательности - https://learn.microsoft.com/ru-ru/cpp/c-language/escape-sequences?view=msvc-170
    // Строковые и символьные литералы (C++) - https://learn.microsoft.com/ru-ru/cpp/cpp/string-and-character-literals-cpp?view=msvc-170
    // Numeric, boolean, and pointer literals - https://learn.microsoft.com/en-us/cpp/cpp/numeric-boolean-and-pointer-literals-cpp?view=msvc-170

    // Юникодные литералы поддерживаем, но юникодные HEX последовательности не поддерживаем пока при выводе в в inserter, если у нас парсер char, а не wchar_t.

    enum State
    {
        stInitial                   ,
        stWideWaitQuot              ,
        stReadChars                 ,
        stWaitEscapeData            ,
        stReadEscapeNumericSequence
    };

    CharType             quotType              = 0;
    State                st                    = stInitial;
    bool                 wideLiteral           = false;
    std::uint_least32_t  hexCode               = 0; // also for octal code too
    unsigned             numReadedHexItems     = 0; // also for octal code too
    unsigned             hexBase               = 0; // also for octal code too

    bool                 allowEscapedLfContinuation = true ;
    bool                 warnOnUnknownEscape        = true ;
    bool                 disableStandardEscapes     = false;
    ExtraEscapesMapType  extraEscapes;


    CharType getKnownEcapedCharCode(CharType ch) const
    {
        // Совсем-совсем базовые последовательности не запрещаются
        switch(ch)
        {
            case (CharType)'\'': return (CharType)'\'';
            case (CharType)'\"': return (CharType)'\"';
            case (CharType)'\\': return (CharType)'\\';
        }

        if (disableStandardEscapes)
            return (CharType)0; // если ничего не найдено

        switch(ch)
        {
            case (CharType)'n': return (CharType)'\n';
            case (CharType)'t': return (CharType)'\t';
            case (CharType)'r': return (CharType)'\r';
            case (CharType)'b': return (CharType)'\b';
            case (CharType)'f': return (CharType)'\f';
            case (CharType)'v': return (CharType)'\v';
            case (CharType)'a': return (CharType)'\a';
            case (CharType)'?': return (CharType)'?';
            default: return (CharType)0; // если ничего не найдено
        }
    }

    auto getExtraEscapeCharSequence(CharType ch) const
    {
        auto it = extraEscapes.find(ch);

        // Секреты auto и decltype - https://habr.com/ru/articles/206458/
        // https://stackoverflow.com/questions/25732386/what-is-stddecay-and-when-it-should-be-used
        if (it==extraEscapes.end())
            return std::decay_t<decltype(it->second)>();
        return it->second;
    }


    bool isValidHexDigit(CharType ch) const
    {
        int d = utils::charToDigit(ch);
        if (d<0)
            return false;

        if ((unsigned)d<hexBase)
            return true;

        return false;
    }

    void initHexSequence(unsigned base, int d=0)
    {
        UMBA_ASSERT(base==8u || base==16u);
        hexCode           = (base==8u) ? (unsigned)d : 0u; // воьмеричная - есть сразу первая цифра, для HEX'а первой цифры нет
        numReadedHexItems = (base==8u) ? 1u : 0u;
        hexBase           = base;
    }

    bool addHexSequenceDigit(int d)
    {
        if (d<0)
            return false;
        if ((unsigned)d>=hexBase)
            return false;

        hexCode*=hexBase;
        hexCode+=(unsigned)d;

        ++numReadedHexItems;

        return true;
    }

    bool addHexSequenceChar(CharType ch)
    {
        return addHexSequenceDigit(utils::charToDigit(ch));
    }

    bool canAddHexDigit() const
    {
        if (hexBase==8u)
        {
            return numReadedHexItems < 3u;
        }
        else
        {
            unsigned maxHexDigits = wideLiteral ? 8u : 2u;
            return numReadedHexItems < maxHexDigits;
        }
    }


public:

    UMBA_RULE_OF_FIVE(CppEscapedSimpleQuotedStringLiteralParser, default, default, default, default, default);

    CppEscapedSimpleQuotedStringLiteralParser& setContinuationMode(bool allowEscapedLfContinuation_=true)
    {
        allowEscapedLfContinuation = allowEscapedLfContinuation_;
        return *this;
    }

    CppEscapedSimpleQuotedStringLiteralParser& setWarnOnUnknownEscape(bool warn=true)
    {
        warnOnUnknownEscape = warn;
        return *this;
    }

    CppEscapedSimpleQuotedStringLiteralParser& setDisableStandardEscapes(bool disable=true)
    {
        disableStandardEscapes = disable;
        return *this;
    }

    CppEscapedSimpleQuotedStringLiteralParser& setExtraEscapes(const ExtraEscapesMapType &extra)
    {
        extraEscapes = extra;
    }



    void reset()
    {
        quotType          = 0;
        st                = stInitial;
        wideLiteral       = false;
        hexCode           = 0;
        numReadedHexItems = 0;
        hexBase           = 0;
    }

    // \a     Звонок (предупреждение)
    // \b     Backspace
    // \f     Подача страницы
    // \n     Новая строка
    // \r     Возврат каретки
    // \t     Горизонтальная табуляция
    // \v     Вертикальная табуляция
    // \'     Одиночная кавычка
    // \"     Двойная кавычка
    // \\     Обратная косая черта
    // \?     Литерал вопросительного знака
    // \ooo     Символ ASCII в восьмеричной нотации
    // \x hh     Символ ASCII в шестнадцатеричной нотации
    // \x hhhh     Символ юникода в шестнадцатеричном формате, если эта escape-последовательность используется в многобайтовой знаковой константе или строковом литерале юникода.

    #include "umba/warnings/push_disable_spectre_mitigation.h"
    virtual StringLiteralParsingResult parseChar(InputIteratorType it, InputIteratorType itEnd, ITokenizerLiteralCharInserter<CharType> *pInserter, MessageStringType *pMsg) override
    {
        UMBA_USED(itEnd);
        CharType ch = *it;

        switch(st)
        {
            case stInitial:
            {
                 if (ch=='L') // а маленькая L считается за маркер юникодного литерала?
                 {
                     st = stWideWaitQuot;
                     wideLiteral = true;
                 }
                 else if (ch=='\'' || ch=='\"')
                 {
                     st = stReadChars;
                     quotType = ch;
                 }
                 else
                 {
                     //!!! В MSVC работает, в GCC надо через явное указание базы
                     InterfaceParentType::setMessage(pMsg, "unrecognised string literal type");
                     return StringLiteralParsingResult::error;
                 }
            }
            break;

            case stWideWaitQuot:
            {
                 if (ch=='\'' || ch=='\"')
                 {
                     st = stReadChars;
                     quotType = ch;
                 }
                 else
                 {
                     InterfaceParentType::setMessage(pMsg, "unrecognised wide string literal type");
                     return StringLiteralParsingResult::error;
                 }
            }
            break;

            ReadCharsLabel:
            case stReadChars:
            {
                if (ch=='\\')
                {
                    st = stWaitEscapeData;
                }
                else if (ch==quotType) // сравниваем с той кавычкой, что была на входе, противоположные кавычки '<->" внутри допустимы, они не заканчивают строку
                {
                    return StringLiteralParsingResult::okStop;
                }
                else if (ch=='\r' || ch=='\n')
                {
                    InterfaceParentType::setMessage(pMsg, (quotType=='\"') ? "missing terminating quot ('\"')" : "missing terminating apos (\"'\")");
                    return StringLiteralParsingResult::error;
                }
                else // просто символ
                {
                    InterfaceParentType::insertChar(pInserter, ch);
                    return StringLiteralParsingResult::okContinue;
                }
            }
            break;

            case stWaitEscapeData:
            {
                auto extraEsc = getExtraEscapeCharSequence(ch);
                CharType knownEsc = getKnownEcapedCharCode(ch);
                int d = utils::charToDigit(ch);

                if (!extraEsc.empty())
                {
                    st = stReadChars;
                    for(auto ech : extraEsc)
                        InterfaceParentType::insertChar(pInserter, ech);
                    return StringLiteralParsingResult::okContinue;
                }
                else if (knownEsc!=0)
                {
                    st = stReadChars;
                    InterfaceParentType::insertChar(pInserter, knownEsc);
                    return StringLiteralParsingResult::okContinue;
                }
                else if (ch=='x' || ch=='X') // А большой X допустим в шестнадцатиричных esc-последовательностях?
                {
                    st = stReadEscapeNumericSequence;
                    initHexSequence(16);
                    return StringLiteralParsingResult::okContinue;
                }
                else if (d>=0 && d<8)
                {
                    st = stReadEscapeNumericSequence;
                    initHexSequence(8, d);
                    return StringLiteralParsingResult::okContinue;
                }
                else if (ch=='\n' || ch=='\r')
                {
                    if (allowEscapedLfContinuation)
                    {
                        st = stReadChars;
                        return StringLiteralParsingResult::okContinue;
                    }
                    else
                    {
                        InterfaceParentType::setMessage(pMsg, (quotType=='\"') ? "missing terminating quot ('\"')" : "missing terminating apos (\"'\")");
                        return StringLiteralParsingResult::error;
                    }
                }
                else // вроде все варианты проверили
                {
                    st = stReadChars;
                    InterfaceParentType::insertChar(pInserter, ch);
                    if (warnOnUnknownEscape)
                        InterfaceParentType::setMessage(pMsg, "unknown escape sequence char");
                    return StringLiteralParsingResult::warnContinue;
                }
            }
            break;

            case stReadEscapeNumericSequence:
            {
                if (isValidHexDigit(ch) && canAddHexDigit())
                {
                    addHexSequenceChar(ch);
                    return StringLiteralParsingResult::okContinue;
                }
                goto ReadCharsLabel;
            }
            break;

        }

        return StringLiteralParsingResult::okContinue;
    }
    #include "umba/warnings/pop.h"


}; // class CppEscapedSimpleQuotedStringLiteralParser
#include "umba/warnings/pop.h"
//----------------------------------------------------------------------------





//----------------------------------------------------------------------------

} // namespace tokenizer
} // namespace umba

