#pragma once

#ifndef UMBA_TOKENIZER_H__DCAEEDE0_7624_4E47_9919_08460EF65A3B__
    #error "Do not include this file directly, include 'umba/tokenizer.h header instead'"
#endif

#include "umba/warnings/push_disable_rel_inc_contains_dbldot.h"
#include "umba/rule_of_five.h"
#include "umba/string_plus.h"
#include "umba/warnings/pop.h"

//
#include <array>
#include <initializer_list>
#include <unordered_map>
#include <functional>

/*

Фильтры инсталлируются в токенизер в начало цепочки.
Для этого у tokenizer'а есть шаблонный метод:

  template<typename FilterType, typename... FilterTypeArgs >
  void installTokenFilter( FilterTypeArgs... args)

Мы не передаём экземпляр фильтра в installTokenFilter, мы его создаём внутри метода installTokenFilter,
и передаём параметры для инициализации в метод installTokenFilter

Сам фильтр должен иметь конструктор

  Filter(Tokenizer::token_handler_type prevHandler, FilterTypeArgs... args )

Схематоз такой: сначала добавляется финальный обработчик, затем добавляются фильтры в цепочку обработчиков,
в начало, при этом существующий обработчик вставляется после добавляемого.



template< typename CharType            // Input chars type
        , typename CharClassTableType  = std::array<CharClass, 128>
        , typename TrieVectorType      = std::vector<TrieNode>
        , typename StringType          = std::basic_string<CharType>  // Тип строки, которая хранит входные символы
        , typename MessagesStringType  = std::string  // Тип строки, которая используется для сообщений (в том числе и от внешних суб-парсеров)
        , typename InputIteratorType   = umba::iterator::TextPositionCountingIterator<CharType>
        >


using token_handler_type                         = std::function<bool( bool
                                                                     , payload_type
                                                                     , InputIteratorType
                                                                     , InputIteratorType
                                                                     , std::basic_string_view<value_type>
                                                                     , messages_string_type&)
                                                                     >;

TokenFilter должен
а) создаваться в изначальном готовом для работы начальном состоянии
б) по токену FIN должны сбрасываться все буфера, и фильтр должен врзвращаться в начальное состояние

*/

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
#define UMBA_TOKENIZER_TOKEN_FILTERS_DECLARE_USING_DEPENDENT_TYPES(className)                \
            /*using tokeniser_type             = className                             ;*/   \
            using tokeniser_type             = TokenizerType                             ;   \
            using token_handler_type         = typename className ::token_handler_type   ;   \
                                                                                             \
            using char_type                  = typename className ::char_type            ;   \
            using value_type                 = typename className ::value_type           ;   \
            using char_class_table_type      = typename className ::char_class_table_type;   \
            using trie_vector_type           = typename className ::trie_vector_type     ;   \
            using string_type                = typename className ::string_type          ;   \
            using iterator_type              = typename className ::iterator_type        ;   \
            using messages_string_type       = typename className ::messages_string_type ;   \
            using token_parsed_data_type     = typename className ::token_parsed_data_type;  \
            using empty_data_type            = typename className ::empty_data_type      ;   \
            using comment_data_type          = typename className ::comment_data_type    ;   \
            using identifier_data_type       = typename className ::identifier_data_type ;   \
            using string_literal_data_type   = typename className ::string_literal_data_type                ; \
            using integer_numeric_literal_data_type = typename className ::integer_numeric_literal_data_type; \
            using float_numeric_literal_data_type   = typename className ::float_numeric_literal_data_type


//----------------------------------------------------------------------------
// umba::tokenizer::filters::
namespace umba {
namespace tokenizer {
namespace filters {

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
//! Тестовый фильтр, который ничего не делает
template<typename TokenizerType>
struct SimplePassTroughFilter
{

    UMBA_TOKENIZER_TOKEN_FILTERS_DECLARE_USING_DEPENDENT_TYPES(TokenizerType);
    using payload_type             = umba::tokenizer::payload_type                ;

protected:

    token_handler_type nextTokenHandler;


public:

    SimplePassTroughFilter(token_handler_type curTokenHandler)
    : nextTokenHandler(curTokenHandler)
    {}

    UMBA_RULE_OF_FIVE(SimplePassTroughFilter, default, default, default, default, default);

    bool operator()( TokenizerType           &tokenizer
                   , bool                    lineStartFlag
                   , payload_type            payloadToken
                   , iterator_type           &b
                   , iterator_type           &e
                   , token_parsed_data_type  &parsedData // std::variant<...>
                   , messages_string_type    &msg
                   )
    {
        if (nextTokenHandler)
            return nextTokenHandler(lineStartFlag, payloadToken, b, e, parsedData, msg);
        return true;
    }

}; // struct SimplePassTroughFilter

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
//! Хранит информацию для вызова tokenHandler'а
template<typename TokenizerType>
struct TokenInfo
{
    UMBA_TOKENIZER_TOKEN_FILTERS_DECLARE_USING_DEPENDENT_TYPES(TokenizerType);
    using payload_type             = umba::tokenizer::payload_type                ;


    bool                                 lineStartFlag;
    payload_type                         payloadToken;
    iterator_type                        b;
    iterator_type                        e;
    // std::basic_string_view<value_type>   strValue;
    token_parsed_data_type               parsedData;

    UMBA_RULE_OF_FIVE(TokenInfo, default, default, default, default, default);

    TokenInfo( bool                    lineStartFlag_
             , payload_type            payloadToken_
             , iterator_type           b_
             , iterator_type           e_
             , token_parsed_data_type  parsedData_ // std::variant<...>
             )
    : lineStartFlag(lineStartFlag_)
    , payloadToken(payloadToken_)
    , b(b_)
    , e(e_)
    //, strValue(strValue_)
    , parsedData(parsedData_)
    {}

}; // struct TokenInfo

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
//! Простой фильтр, который буферизирует одинаковые токены. Он хранит только итератор первого токена в последовательности
template<typename TokenizerType, umba::tokenizer::payload_type CollectingPayloadValue>
struct TokenCollectingFilter
{

    UMBA_TOKENIZER_TOKEN_FILTERS_DECLARE_USING_DEPENDENT_TYPES(TokenizerType);
    using payload_type      = umba::tokenizer::payload_type;
    using token_info_type   = TokenInfo<TokenizerType>;

protected:

    token_handler_type nextTokenHandler;

    token_info_type  sequenceStartInfo;

    bool reset(bool res=true)
    {
        sequenceStartInfo = token_info_type{};
        sequenceStartInfo.payloadToken = UMBA_TOKENIZER_TOKEN_NUL; // Для надёжности )
        return res;
    }

    void saveSequenceStartInfo(bool lineStartFlag, payload_type payloadToken, iterator_type &b, iterator_type &e, token_parsed_data_type parsedData)
    {
        UMBA_ASSERT(payloadToken!=UMBA_TOKENIZER_TOKEN_NUL); // если не равно, то всё нормас
        sequenceStartInfo = token_info_type{lineStartFlag, payloadToken, b, e, parsedData};
    }

    static
    bool isCollectingPayloadToken(payload_type payloadToken)
    {
        return payloadToken==UMBA_TOKENIZER_TOKEN_RAW_CHAR;
    }

    bool isCollectingMode() const
    {
        return isCollectingPayloadToken(sequenceStartInfo.payloadToken);
    }


    // По ссылке мы передаём token_parsed_data_type только для того, чтобы не плодить копии
    // По неконстантной ссылке - чтобы хэндлер могли поменять там значение и передать изменённое дальше
    // Возврат значения по ссылке тут у нас не используется

    // bool callNextTokenHandler( TokenizerType &tokenizer, bool lineStartFlag, payload_type payloadToken, iterator_type &b, iterator_type &e, token_parsed_data_type &parsedData, messages_string_type &msg) const
    bool callNextTokenHandler( TokenizerType &tokenizer, bool lineStartFlag, payload_type payloadToken, iterator_type &b, iterator_type &e, token_parsed_data_type parsedData, messages_string_type &msg) const
    {
        if (!nextTokenHandler)
            return true;

        return nextTokenHandler(tokenizer, lineStartFlag, payloadToken, b, e, parsedData, msg);
    }

public:

    TokenCollectingFilter(token_handler_type curTokenHandler)
    : nextTokenHandler(curTokenHandler)
    {
        reset();
    }

    UMBA_RULE_OF_FIVE(TokenCollectingFilter, default, default, default, default, default);

    bool operator()( TokenizerType           &tokenizer
                   , bool                    lineStartFlag
                   , payload_type            payloadToken
                   , iterator_type           &b
                   , iterator_type           &e
                   , token_parsed_data_type  &parsedData // std::variant<...>
                   , messages_string_type    &msg
                   )
    {
        if (payloadToken==UMBA_TOKENIZER_TOKEN_CTRL_RST)
        {
            return reset(callNextTokenHandler( tokenizer, lineStartFlag, payloadToken, b, e, parsedData, msg));
        }

        if (payloadToken==UMBA_TOKENIZER_TOKEN_CTRL_FIN)
        {
            if (isCollectingMode()) // режим коллекционирования токенов
            {
                // режим коллекционирования был включен - надо сбросить коллекцию
                auto tmpB = sequenceStartInfo.b; // начало коллекции - было сохранено
                auto tmpE = b;                   // конец коллекции - оно же начало FIN
    
                if (!callNextTokenHandler( tokenizer, sequenceStartInfo.lineStartFlag, sequenceStartInfo.payloadToken, tmpB, tmpE, sequenceStartInfo.parsedData, msg))
                {
                    // tmpB, tmpE могли быть скорректированы, надо их поместить в b и e для возврата
                    b = tmpB;
                    e = tmpE;
                    return reset(false);
                }
            }

            // Теперь, вне зависимости от предыдущего режима просто прокидываем FIN дальше
            return reset(callNextTokenHandler( tokenizer, lineStartFlag, payloadToken, b, e, parsedData, msg));
        }

        // FIN обработали, теперь нормальная работа

        if (isCollectingMode()==isCollectingPayloadToken(payloadToken))
        {
            // режим не меняется
            if (!isCollectingMode()) // не режим коллекционирования, просто прокидываем дальше
                return callNextTokenHandler( tokenizer, lineStartFlag, payloadToken, b, e, parsedData, msg);
            // в режиме коллекционирования ничего не делаем
            return true;
        }
        else
        {
            // режим меняется
            if (!isCollectingMode())
            {
                // был режим неколлекционирования
                // сохраняем данные начала последовательности
                saveSequenceStartInfo(lineStartFlag, payloadToken, b, e, parsedData);

                // Дальше прокидывать ничего не надо
                return true;
            }
            else
            {
                // был режим коллекционирования

                auto tmpB = sequenceStartInfo.b; // начало колелкции - было сохранено
                auto tmpE = b;                   // конец коллекции - оно же начало текущего

                // Пробрасываем коллекцию
                if (!callNextTokenHandler( tokenizer, sequenceStartInfo.lineStartFlag, sequenceStartInfo.payloadToken, tmpB, tmpE, sequenceStartInfo.parsedData, msg))
                {
                    // tmpB, tmpE могли быть скорректированы, надо их поместить в b и e для возврата
                    b = tmpB;
                    e = tmpE;
                    return reset(false);
                }

                // Теперь, вне зависимости от предыдущего режима просто прокидываем то, что пришло - дальше
                return reset(callNextTokenHandler( tokenizer, lineStartFlag, payloadToken, b, e, parsedData, msg));
            }

        }

    }

}; // struct TokenCollectingFilter

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
//! Фильтр, который буферизирует токены UMBA_TOKENIZER_TOKEN_RAW_CHAR
template<typename TokenizerType>
using RawCharsCollectingFilter = TokenCollectingFilter<TokenizerType, UMBA_TOKENIZER_TOKEN_RAW_CHAR>;

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
//! Предоставляет функционал буферизации токенов
/*! Возможно, тут стоит использовать boost::small_vector.
    Хотя, он тоже лезет в кучу, а единожды увеличившийся стандартный вектор не склонен к уменьшению объема
    аллоцированной памяти, если не вызывать shrink_to_fit
 */
template<typename TokenizerType, typename VectorType=std::vector<TokenInfo<TokenizerType> > >
struct FilterBase
{
    UMBA_TOKENIZER_TOKEN_FILTERS_DECLARE_USING_DEPENDENT_TYPES(TokenizerType);
    using payload_type             = umba::tokenizer::payload_type                ;

protected:

    token_handler_type     nextTokenHandler;
    VectorType             tokenBuffer;

    void clearTokenBuffer()
    {
        tokenBuffer.clear();
    }

    bool tokenBufferPushBack(bool lineStartFlag, payload_type payloadToken, iterator_type &b, iterator_type &e, token_parsed_data_type parsedData, bool res=true)
    {
        tokenBuffer.emplace_back(lineStartFlag, payloadToken, b, e, parsedData);
        return res;
    }

    bool flushTokenBuffer(TokenizerType &tokenizer, messages_string_type &msg, iterator_type &b, iterator_type &e, bool bClear=true)
    {
        if (!nextTokenHandler)
            return true;

        for(const auto &tki : tokenBuffer)
        {
            auto tmpB = tki.b;
            auto tmpE = tki.e;
            auto parsedDataCopy = tki.parsedData;
            if (!nextTokenHandler(tokenizer, tki.lineStartFlag, tki.payloadToken, tmpB, tmpE, parsedDataCopy /*strValue*/, msg))
            {
                if (bClear)
                   clearTokenBuffer();
                b = tmpB;
                e = tmpE;
                if (bClear)
                    clearTokenBuffer();
                return false;
            }
        }

        if (bClear)
           clearTokenBuffer();

        return true;
    }

    bool reset(bool res=true)
    {
        clearTokenBuffer();
        return res;
    }

    bool flushTokenBufferAndCallNextHandler(TokenizerType &tokenizer, bool lineStartFlag, payload_type payloadToken, iterator_type &b, iterator_type &e, token_parsed_data_type &parsedData, messages_string_type &msg, bool bClear=true)
    {
        if (!nextTokenHandler)
            return true;

        if (!flushTokenBuffer(tokenizer, msg, b, e, bClear))
            return false;
        bool res = nextTokenHandler(tokenizer, lineStartFlag, payloadToken, b, e, parsedData /*strValue*/, msg);
        if (bClear)
           clearTokenBuffer();
        return res;
    }

    bool callNextTokenHandler( TokenizerType &tokenizer, bool lineStartFlag, payload_type payloadToken, iterator_type &b, iterator_type &e, token_parsed_data_type &parsedData, messages_string_type &msg) const
    {
        if (!nextTokenHandler)
            return true;

        return nextTokenHandler(tokenizer, lineStartFlag, payloadToken, b, e, parsedData, msg);
    }


public:

    UMBA_RULE_OF_FIVE(FilterBase, default, default, default, default, default);

    FilterBase(token_handler_type curTokenHandler)
    : nextTokenHandler(curTokenHandler)
    {}

}; // struct FilterBase

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
template<typename TokenizerType, typename VectorType=std::vector<TokenInfo<TokenizerType> > >
struct SimpleSuffixGluingFilter : FilterBase<TokenizerType, VectorType>
{
    using TBase = FilterBase<TokenizerType, VectorType>;

    UMBA_TOKENIZER_TOKEN_FILTERS_DECLARE_USING_DEPENDENT_TYPES(TBase);
    using payload_type             = umba::tokenizer::payload_type        ;

    UMBA_RULE_OF_FIVE(SimpleSuffixGluingFilter, default, default, default, default, default);


    enum class GluingOption
    {
        glueAll,
        glueNumbers,
        glueStrings
    };

    GluingOption gluingOption = GluingOption::glueAll;


    SimpleSuffixGluingFilter(token_handler_type curTokenHandler, GluingOption gOpt=GluingOption::glueAll)
    : TBase(curTokenHandler), gluingOption(gOpt)
    {}


protected:

    static
    bool isNumberLiteral(payload_type payloadToken)
    {
        return payloadToken>=UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_FIRST && payloadToken<=UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_LAST;
    }

    static
    bool isStringLiteral(payload_type payloadToken)
    {
        return payloadToken>=UMBA_TOKENIZER_TOKEN_STRING_LITERAL_FIRST && payloadToken<=UMBA_TOKENIZER_TOKEN_STRING_LITERAL_LAST;
    }

    const
    bool glueNumbers() const
    {
        return gluingOption==GluingOption::glueAll || gluingOption==GluingOption::glueNumbers;
    }

    const
    bool glueStrings() const
    {
        return gluingOption==GluingOption::glueAll || gluingOption==GluingOption::glueStrings;
    }


public:

    bool operator()( TokenizerType           &tokenizer
                   , bool                    lineStartFlag
                   , payload_type            payloadToken
                   , iterator_type           &b
                   , iterator_type           &e
                   , token_parsed_data_type  &parsedData // std::variant<...>
                   , messages_string_type    &msg
                   )
    {
        if (payloadToken==UMBA_TOKENIZER_TOKEN_CTRL_RST)
        {
            return this->reset(this->callNextTokenHandler( tokenizer, lineStartFlag, payloadToken, b, e, parsedData, msg));
        }

        if (payloadToken==UMBA_TOKENIZER_TOKEN_CTRL_FIN)
        {
            return this->flushTokenBufferAndCallNextHandler(tokenizer, lineStartFlag, payloadToken, b, e, parsedData, msg, true /* bClear */ );
        }

        if (this->tokenBuffer.empty())
        {
            if (isNumberLiteral(payloadToken) && glueNumbers()) // Если числовой литерал и разрешено приклеивать к ним суффиксы
            {
                this->tokenBuffer.emplace_back(lineStartFlag, payloadToken, b, e, parsedData /* strValue */ );
                return true;
            }
            else if (isStringLiteral(payloadToken) && glueStrings()) // Если строковый литерал и разрешено приклеивать к ним суффиксы
            {
                this->tokenBuffer.emplace_back(lineStartFlag, payloadToken, b, e, parsedData /* strValue */ );
                return true;
            }
            else
            {
                return this->callNextTokenHandler(tokenizer, lineStartFlag, payloadToken, b, e, parsedData /* strValue */ , msg);
            }
        }

        // В буфере что-то есть

        if (payloadToken!=UMBA_TOKENIZER_TOKEN_IDENTIFIER) // Что у нас пришло?
        {
            // У нас пришел не идентификатор, значит, склейка не состоится
            if (!this->flushTokenBuffer(tokenizer, msg, b, e)) // сбрасываем буферизированное (с очисткой буфера)
                 return false;

            // Пробрасываем пришедшее на текущем шаге
            return this->callNextTokenHandler(tokenizer, lineStartFlag, payloadToken, b, e, parsedData /* strValue */ , msg);
        }


        // в буфере лежит токен числового литерала

        UMBA_ASSERT(!this->tokenBuffer.empty()); // точно ли ы буфере что-то есть?
        TokenInfo prevTokenInfo = this->tokenBuffer[0]; // инфа по числовому или строковому литералу
        UMBA_ASSERT(prevTokenInfo.e==b); // Начало текущего токена должно совпадать с концом предыдущего

        auto literalStartIter = prevTokenInfo.b;
        auto suffixStartIter  = prevTokenInfo.e; // началом суффикса является конец числового литерала
        auto literalEndIter   = e;

        auto updatePayloadDataAndCallNextHandler = [&](auto payloadData) -> bool
        {
            payloadData.hasSuffix      = true;
            payloadData.suffixStartPos = suffixStartIter;
            token_parsed_data_type payloadDataCopy = payloadData;
            return this->callNextTokenHandler( tokenizer, prevTokenInfo.lineStartFlag, prevTokenInfo.payloadToken
                                         , literalStartIter, literalEndIter, payloadDataCopy, msg
                                         );
        };


        bool res = true;

        if (isNumberLiteral(prevTokenInfo.payloadToken))
        {
            if (prevTokenInfo.payloadToken&UMBA_TOKENIZER_TOKEN_FLOAT_FLAG)
            {
                res = updatePayloadDataAndCallNextHandler(std::get<typename TokenizerType::FloatNumericLiteralData>(prevTokenInfo.parsedData));
            }
            else
            {
                res = updatePayloadDataAndCallNextHandler(std::get<typename TokenizerType::IntegerNumericLiteralData>(prevTokenInfo.parsedData));
            }
        }
        else if (isStringLiteral(prevTokenInfo.payloadToken))
        {
            res = updatePayloadDataAndCallNextHandler(std::get<typename TokenizerType::StringLiteralData>(prevTokenInfo.parsedData));
        }
        else
        {
            UMBA_ASSERT(0);
        }

        this->clearTokenBuffer();
        return res;

    }

}; // struct SimpleSuffixGluingFilter

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
//! Вставляет маркеры начала и конца C/C++ препроцессора. Не требует буферизации, так что FilterBase не нужен
/*! Возможно, тут стоит использовать boost::small_vector.
    Хотя, он тоже лезет в кучу, а единожды увеличившийся стандартный вектор не склонен к уменьшению объема
    аллоцированной памяти, если не вызывать shrink_to_fit
 */
template<typename TokenizerType >
struct CcPreprocessorFilter
{
    UMBA_TOKENIZER_TOKEN_FILTERS_DECLARE_USING_DEPENDENT_TYPES(TokenizerType);
    using payload_type             = umba::tokenizer::payload_type           ;

protected:

    enum State
    {
        stNormal,
        stWaitPreprocessorKeyword,
        stPreprocessor
    };

    State st                          = stNormal;
    bool  isStartAngleBracketOperator = false;
    bool  inDefine                    = false; //TODO: !!! Вообще, по уму, надо бы сделать энум со значениями inDefine, inPragma, inError, inWarning, inCondition

    token_handler_type     nextTokenHandler;


    static
    auto makePreprocessorKeywords()
    {
        std::unordered_map<string_type, payload_type> m;
        m[string_plus::make_string<string_type>("include")]             = UMBA_TOKENIZER_TOKEN_CC_PP_INCLUDE | UMBA_TOKENIZER_TOKEN_CTRL_FLAG;
        m[string_plus::make_string<string_type>("define")]              = UMBA_TOKENIZER_TOKEN_CC_PP_DEFINE  | UMBA_TOKENIZER_TOKEN_CTRL_FLAG;
        m[string_plus::make_string<string_type>("undef")]               = UMBA_TOKENIZER_TOKEN_CC_PP_UNDEF;

        m[string_plus::make_string<string_type>("line")]                = UMBA_TOKENIZER_TOKEN_CC_PP_LINE;

        m[string_plus::make_string<string_type>("error")]               = UMBA_TOKENIZER_TOKEN_CC_PP_ERROR  ;
        m[string_plus::make_string<string_type>("warning")]             = UMBA_TOKENIZER_TOKEN_CC_PP_WARNING;

        m[string_plus::make_string<string_type>("pragma")]              = UMBA_TOKENIZER_TOKEN_CC_PP_PRAGMA;

        m[string_plus::make_string<string_type>("if")]                  = UMBA_TOKENIZER_TOKEN_CC_PP_IF;
        m[string_plus::make_string<string_type>("elif")]                = UMBA_TOKENIZER_TOKEN_CC_PP_ELIF;
        m[string_plus::make_string<string_type>("else")]                = UMBA_TOKENIZER_TOKEN_CC_PP_ELSE;
        m[string_plus::make_string<string_type>("endif")]               = UMBA_TOKENIZER_TOKEN_CC_PP_ENDIF;
        m[string_plus::make_string<string_type>("ifdef")]               = UMBA_TOKENIZER_TOKEN_CC_PP_IFDEF;
        m[string_plus::make_string<string_type>("ifndef")]              = UMBA_TOKENIZER_TOKEN_CC_PP_IFNDEF;
        m[string_plus::make_string<string_type>("elifdef")]             = UMBA_TOKENIZER_TOKEN_CC_PP_ELIFDEF;
        m[string_plus::make_string<string_type>("elifndef")]            = UMBA_TOKENIZER_TOKEN_CC_PP_ELIFNDEF;

        m[string_plus::make_string<string_type>("defined")]             = UMBA_TOKENIZER_TOKEN_CC_PP_DEFINED;
        m[string_plus::make_string<string_type>("__has_include")]       = UMBA_TOKENIZER_TOKEN_CC_PP_HAS_INCLUE;
        m[string_plus::make_string<string_type>("__has_cpp_attribute")] = UMBA_TOKENIZER_TOKEN_CC_PP_HAS_CPP_ATTRIBUTE;

        m[string_plus::make_string<string_type>("export")]              = UMBA_TOKENIZER_TOKEN_CC_PP_EXPORT;
        m[string_plus::make_string<string_type>("import")]              = UMBA_TOKENIZER_TOKEN_CC_PP_IMPORT;
        m[string_plus::make_string<string_type>("module")]              = UMBA_TOKENIZER_TOKEN_CC_PP_MODULE;

        return m;
    }

    static
    const auto& getPreprocessorKeywords()
    {
        static auto m = makePreprocessorKeywords();
        return m;
    }

    static
    payload_type getPreprocessorKeywordToken(const string_type &ident)
    {
        const auto& m = getPreprocessorKeywords();
        auto it = m.find(ident);
        if (it==m.end())
            return payload_invalid;
        return it->second;
    }

    bool reset(TokenizerType &tokenizer, bool res=true)
    {
        restoreAngleBracketsAndHashState(tokenizer);

        st                          = stNormal;
        isStartAngleBracketOperator = false;
        inDefine                    = false;

        return res;
    }

    void restoreAngleBracketsAndHashState(TokenizerType &tokenizer)
    {
        if (isStartAngleBracketOperator)
        {
            // Устанавливаем признак оператора обратно
            tokenizer.setResetCharClassFlags('<', umba::tokenizer::CharClass::opchar, umba::tokenizer::CharClass::none);
        }

        // Ничего не устанавливаем, сбрасываем string_literal_prefix
        tokenizer.setResetCharClassFlags('<', umba::tokenizer::CharClass::none, umba::tokenizer::CharClass::string_literal_prefix);

        tokenizer.setResetCharClassFlags('#', umba::tokenizer::CharClass::none, umba::tokenizer::CharClass::opchar); // Ничего не устанавливаем, сбрасываем opchar
    }

    bool callNextTokenHandler( TokenizerType &tokenizer, bool lineStartFlag, payload_type payloadToken, iterator_type &b, iterator_type &e, token_parsed_data_type &parsedData, messages_string_type &msg) const
    {
        if (!nextTokenHandler)
            return true;

        return nextTokenHandler(tokenizer, lineStartFlag, payloadToken, b, e, parsedData, msg);
    }

public:

    UMBA_RULE_OF_FIVE(CcPreprocessorFilter, default, default, default, default, default);

    CcPreprocessorFilter(token_handler_type curTokenHandler)
    : nextTokenHandler(curTokenHandler)
    {}

    bool operator()( TokenizerType           &tokenizer
                   , bool                    lineStartFlag
                   , payload_type            payloadToken
                   , iterator_type           &b
                   , iterator_type           &e
                   , token_parsed_data_type  &parsedData // std::variant<...>
                   , messages_string_type    &msg
                   )
    {
        if (payloadToken==UMBA_TOKENIZER_TOKEN_CTRL_RST)
        {
            return reset(tokenizer, callNextTokenHandler( tokenizer, lineStartFlag, payloadToken, b, e, parsedData, msg));
        }

        switch(st)
        {
            case stNormal:
            {
                if (payloadToken==UMBA_TOKENIZER_TOKEN_CTRL_FIN)
                {
                    if (!nextTokenHandler(tokenizer, lineStartFlag, payloadToken, b, e, parsedData, msg))
                        return reset(tokenizer, false);
                    return reset(tokenizer, true);
                }

                if (lineStartFlag && b!=e && *b==(value_type)'#')
                {
                    reset(tokenizer);

                    if (!nextTokenHandler(tokenizer, lineStartFlag, UMBA_TOKENIZER_TOKEN_CTRL_CC_PP_START, e, e, token_parsed_data_type(typename TokenizerType::EmptyData()) /* strValue */ , msg))
                        return false;
                    if (!nextTokenHandler(tokenizer, lineStartFlag, payloadToken, b, e, parsedData /* strValue */ , msg))
                        return false;
                    st = stWaitPreprocessorKeyword;

                    return true;
                }

                break;
            }

            case stWaitPreprocessorKeyword:
            {
                if (payloadToken==UMBA_TOKENIZER_TOKEN_CTRL_FIN)
                {
                    return reset(tokenizer, false);
                }

                if (payloadToken==UMBA_TOKENIZER_TOKEN_IDENTIFIER)
                {
                    st = stPreprocessor;

                    payload_type ppKewordId = getPreprocessorKeywordToken(string_type(tokenizer.makeStringView(b,e)));

                    if (ppKewordId!=payload_invalid)
                    {
                        if (ppKewordId&UMBA_TOKENIZER_TOKEN_CTRL_FLAG)
                        {
                            // Сигналим контрольным
                            token_parsed_data_type emptyVal = empty_data_type();
                            if (!nextTokenHandler(tokenizer, lineStartFlag, ppKewordId, e, e, emptyVal, msg))
                                return false;
                        }

                        if (ppKewordId==UMBA_TOKENIZER_TOKEN_CTRL_CC_PP_INCLUDE)
                        {
                            // Запоминаем предыдущее состояние символа '<' - opchar или нет
                            isStartAngleBracketOperator = (tokenizer.getCharClass('<') & umba::tokenizer::CharClass::opchar) != 0;

                            // устанавливаем string_literal_prefix, сбрасываем opchar
                            tokenizer.setResetCharClassFlags('<', umba::tokenizer::CharClass::string_literal_prefix, umba::tokenizer::CharClass::opchar);
                        }
                        else if (ppKewordId==UMBA_TOKENIZER_TOKEN_CTRL_CC_PP_DEFINE)
                        {
                            // устанавливаем opchar, ничего не сбрасываем - # и ## - годные операторы внутри define'а
                            tokenizer.setResetCharClassFlags('#', umba::tokenizer::CharClass::opchar, umba::tokenizer::CharClass::none);
                            inDefine = true;
                        }

                        if (ppKewordId&UMBA_TOKENIZER_TOKEN_CTRL_FLAG)
                        {
                            // Сбрасываем флаг контрольного токена
                            ppKewordId &= ~UMBA_TOKENIZER_TOKEN_CTRL_FLAG;
                        }

                        // Пуляем найденным токеном
                        token_parsed_data_type emptyData = empty_data_type();
                        if (!nextTokenHandler(tokenizer, lineStartFlag, ppKewordId, b, e, emptyData /* strValue */ , msg)) // Сигналим про дефайн
                            return false;
                    }
                    else
                    {
                        if (!nextTokenHandler(tokenizer, lineStartFlag, payloadToken, b, e, parsedData /* strValue */ , msg)) // пробрасываем токен какой получили
                            return false;
                    }

                    return true;
                }
                else if (payloadToken==UMBA_TOKENIZER_TOKEN_CTRL_FIN || payloadToken==UMBA_TOKENIZER_TOKEN_LINEFEED)
                {
                    reset(tokenizer);

                    token_parsed_data_type emptyData = empty_data_type();
                    if (!nextTokenHandler(tokenizer, lineStartFlag, UMBA_TOKENIZER_TOKEN_CTRL_CC_PP_END, e, e, emptyData /* strValue */ , msg))
                        return false;

                    if (!nextTokenHandler(tokenizer, lineStartFlag, payloadToken, b, e, parsedData /* strValue */ , msg)) // пробрасываем токен
                        return false;

                    return true;
                }

                break;
            }

            case stPreprocessor:
            {
                if (payloadToken==UMBA_TOKENIZER_TOKEN_CTRL_FIN || payloadToken==UMBA_TOKENIZER_TOKEN_LINEFEED)
                {
                    reset(tokenizer);

                    token_parsed_data_type emptyData = empty_data_type();
                    if (!nextTokenHandler(tokenizer, lineStartFlag, UMBA_TOKENIZER_TOKEN_CTRL_CC_PP_END, e, e, emptyData /* strValue */ , msg))
                        return false;

                    if (!nextTokenHandler(tokenizer, lineStartFlag, payloadToken, b, e, parsedData /* strValue */ , msg)) // пробрасываем токен
                        return false;

                    return true;
                }

                // Внутри дефайнов нет никаких ключевых слов
                if (inDefine)
                {
                    break;
                }

                // У нас что-то кроме дефайна

                // И у нас пришел идентификатор
                // Мы либо в условных операторах, либо всякие прагмы и прочий трэш. В трэше ключевые слова вроде не используются, поэтому пока по-простому

                if (payloadToken==UMBA_TOKENIZER_TOKEN_IDENTIFIER)
                {
                    payload_type ppKewordId = getPreprocessorKeywordToken(string_type(tokenizer.makeStringView(b,e)));

                    if ( TheValue(ppKewordId).oneOf( UMBA_TOKENIZER_TOKEN_CC_PP_DEFINED
                                                   , UMBA_TOKENIZER_TOKEN_CC_PP_HAS_INCLUE
                                                   , UMBA_TOKENIZER_TOKEN_CC_PP_HAS_CPP_ATTRIBUTE
                                                   )
                       )
                    {
                        return nextTokenHandler(tokenizer, lineStartFlag, ppKewordId, b, e, parsedData /* strValue */ , msg);
                    }
                    else
                    {
                        return nextTokenHandler(tokenizer, lineStartFlag, payloadToken, b, e, parsedData /* strValue */ , msg);
                    }
                }

                break;
            }

        } // switch(st)

        if (payloadToken==UMBA_TOKENIZER_TOKEN_CTRL_FIN)
        {
            reset(tokenizer);
            //st = stNormal;
        }

        // прокидываем текущий токен
        return nextTokenHandler(tokenizer, lineStartFlag, payloadToken, b, e, parsedData /* strValue */ , msg);

    }


}; // struct CcPreprocessorFilter

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
template<typename TokenizerType, typename VectorType=std::vector<TokenInfo<TokenizerType> > >
struct SimpleSequenceComposingFilter : FilterBase<TokenizerType, VectorType>
{
    using TBase = FilterBase<TokenizerType, VectorType>;

    UMBA_TOKENIZER_TOKEN_FILTERS_DECLARE_USING_DEPENDENT_TYPES(TBase);
    using payload_type             = umba::tokenizer::payload_type        ;

    // First argument is index in sequence
    using extra_check_function_type = std::function<bool(std::size_t,TokenizerType&,bool,payload_type,iterator_type,iterator_type,token_parsed_data_type&)>;

    UMBA_RULE_OF_FIVE(SimpleSequenceComposingFilter, default, default, default, default, default);


    payload_type                resultPayloadToken = 0;
    std::size_t                 resultPayloadDataIndex   = 0;
    std::vector<payload_type>   payloadsMatchList;
    extra_check_function_type   extraCheckFunction;

    // https://www.scs.stanford.edu/~dm/blog/param-pack.html
    // https://selfboot.cn/2024/05/07/variadic_arguments_in_c++/
    // https://dev.to/pauljlucas/variadic-functions-in-c-2alg

    SimpleSequenceComposingFilter( token_handler_type                   curTokenHandler
                                 , payload_type                         resultPayloadToken_
                                 , std::size_t                          resultPayloadDataIndex_ /* обычно 0 */
                                 , const std::vector<payload_type>      &payloadsList_
                                 )
    : TBase(curTokenHandler)
    , resultPayloadToken(resultPayloadToken_)
    , resultPayloadDataIndex(resultPayloadDataIndex_)
    , payloadsMatchList(payloadsList_)
    {
        UMBA_ASSERT(!payloadsMatchList.empty());
        UMBA_ASSERT(resultPayloadDataIndex<payloadsMatchList.size());
    }

    SimpleSequenceComposingFilter( token_handler_type                   curTokenHandler
                                 , payload_type                         resultPayloadToken_
                                 , std::size_t                          resultPayloadDataIndex_ /* обычно 0 */
                                 , const std::vector<payload_type>      &payloadsList_
                                 , extra_check_function_type            extraCheckFunction_
                                 )
    : TBase(curTokenHandler)
    , resultPayloadToken(resultPayloadToken_)
    , resultPayloadDataIndex(resultPayloadDataIndex_)
    , payloadsMatchList(payloadsList_)
    , extraCheckFunction(extraCheckFunction_)
    {
        UMBA_ASSERT(!payloadsMatchList.empty());
        UMBA_ASSERT(resultPayloadDataIndex<payloadsMatchList.size());
    }

    bool reset(bool res=true)
    {
        TBase::reset();
        return res;
    }

protected:


public:

    bool operator()( TokenizerType           &tokenizer
                   , bool                    lineStartFlag
                   , payload_type            payloadToken
                   , iterator_type           &b
                   , iterator_type           &e
                   , token_parsed_data_type  &parsedData // std::variant<...>
                   , messages_string_type    &msg
                   )
    {
        if (payloadToken==UMBA_TOKENIZER_TOKEN_CTRL_RST)
        {
            return reset(this->callNextTokenHandler( tokenizer, lineStartFlag, payloadToken, b, e, parsedData, msg));
        }

        if (payloadToken==UMBA_TOKENIZER_TOKEN_CTRL_FIN)
        {
            return this->flushTokenBufferAndCallNextHandler(tokenizer, lineStartFlag, payloadToken, b, e, parsedData, msg, true /* bClear */ );
        }

        // this->tokenBuffer.size() хранит всегода меньше элементов, чем в образце payloadsList.size()
        UMBA_ASSERT(this->tokenBuffer.size()<payloadsMatchList.size()); // если payloadsList пустой - тоже выстрелит

        // Тут надо матчер вызывать
        if (payloadsMatchList[this->tokenBuffer.size()]!=payloadToken)
        {
            // пришедшая нагрузка не соответствует паттерну
            // флушим, прокидываем текущие аргументы и очищаем буфер
            return this->flushTokenBufferAndCallNextHandler(tokenizer, lineStartFlag, payloadToken, b, e, parsedData, msg, true /* bClear */ );
        }

        if (extraCheckFunction)
        {
            // using extra_check_function_type = std::function<bool(std::size_t,TokenizerType&,bool,payload_type,iterator_type,iterator_type,token_parsed_data_type)>;
            if (!extraCheckFunction(this->tokenBuffer.size(), tokenizer, lineStartFlag, payloadToken, b, e, parsedData))
            {
                // пришедшая нагрузка не прошла дополнительную проверку
                // флушим, прокидываем текущие аргументы и очищаем буфер
                return this->flushTokenBufferAndCallNextHandler(tokenizer, lineStartFlag, payloadToken, b, e, parsedData, msg, true /* bClear */ );
            }
        }

        // Иначе - кладём в буфер
        this->tokenBufferPushBack(lineStartFlag, payloadToken, b, e, parsedData);

        if (this->tokenBuffer.size()<payloadsMatchList.size())
            return true; // Ещё не набрали полностью паттерн


        auto tki = this->tokenBuffer.front();
        //auto tmpB = this->tokenBuffer.front().b;
        if (!this->callNextTokenHandler(tokenizer, tki.lineStartFlag, resultPayloadToken, tki.b, e, this->tokenBuffer[resultPayloadDataIndex].parsedData /*strValue*/, msg))
        {
             b = tki.b;
             this->reset();
             return false;
        }

        this->reset();
        return true;

    }

}; // struct SimpleSequenceComposingFilter

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
template<typename TokenizerType, typename VectorType=std::vector<TokenInfo<TokenizerType> > >
struct RepeatedTokenComposingFilter : SimpleSequenceComposingFilter<TokenizerType, VectorType>
{
    using TBase = SimpleSequenceComposingFilter<TokenizerType, VectorType>;
    UMBA_TOKENIZER_TOKEN_FILTERS_DECLARE_USING_DEPENDENT_TYPES(TBase);
    using payload_type             = umba::tokenizer::payload_type        ;
    UMBA_RULE_OF_FIVE(RepeatedTokenComposingFilter, delete, default, default, default, default);

    RepeatedTokenComposingFilter( token_handler_type curTokenHandler
                                , payload_type       resultPayloadToken
                                , std::size_t        nRepetitions
                                , payload_type       srcPayloadToken
                                )
    : TBase( curTokenHandler
           , resultPayloadToken
           , 0 // данные берем из первого токена, всё равно это обычно для всяких скобок будет использоваться
           , std::vector<payload_type>(nRepetitions, srcPayloadToken)
           )
    {}

}; // struct RepeatedTokenComposingFilter

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
#define UMBA_TOKENIZER_DECLARE_REPEATED_TOKEN_COMPOSING_FILTER_CLASS(FilterClassName, resultPayloadToken, nRepetitions, srcPayloadToken)              \
            template<typename TokenizerType, typename VectorType=std::vector<TokenInfo<TokenizerType> > >                                             \
            struct FilterClassName : umba::tokenizer::filters::RepeatedTokenComposingFilter<TokenizerType, VectorType>                                \
            {                                                                                                                                         \
                using TBase = umba::tokenizer::filters::RepeatedTokenComposingFilter<TokenizerType, VectorType>;                                      \
                UMBA_TOKENIZER_TOKEN_FILTERS_DECLARE_USING_DEPENDENT_TYPES(TBase);                                                                    \
                using payload_type             = umba::tokenizer::payload_type        ;                                                               \
                UMBA_RULE_OF_FIVE(FilterClassName, delete, default, default, default, default);                                                       \
                                                                                                                                                      \
                FilterClassName(token_handler_type curTokenHandler)                                                                                   \
                : TBase(curTokenHandler, resultPayloadToken, nRepetitions, srcPayloadToken)                                                           \
                {}                                                                                                                                    \
            }

//----------------------------------------------------------------------------

UMBA_TOKENIZER_DECLARE_REPEATED_TOKEN_COMPOSING_FILTER_CLASS(DblSquareBracketOpenComposingFilter , UMBA_TOKENIZER_TOKEN_DBLSQUARE_BRACKET_OPEN , 2, UMBA_TOKENIZER_TOKEN_SQUARE_BRACKET_OPEN );
UMBA_TOKENIZER_DECLARE_REPEATED_TOKEN_COMPOSING_FILTER_CLASS(DblSquareBracketCloseComposingFilter, UMBA_TOKENIZER_TOKEN_DBLSQUARE_BRACKET_CLOSE, 2, UMBA_TOKENIZER_TOKEN_SQUARE_BRACKET_CLOSE);

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
template<typename TokenizerType, typename VectorType=std::vector<TokenInfo<TokenizerType> > >
struct IdentifierToKeywordConversionFilter : FilterBase<TokenizerType, VectorType>
{
    using TBase = FilterBase<TokenizerType, VectorType>;

    UMBA_TOKENIZER_TOKEN_FILTERS_DECLARE_USING_DEPENDENT_TYPES(TBase);
    using payload_type             = umba::tokenizer::payload_type        ;
    // using string_type              = typename TokenizerType::string_type  ;

    UMBA_RULE_OF_FIVE(IdentifierToKeywordConversionFilter, default, default, default, default, default);

    // Config
    payload_type                                  matchTo            = UMBA_TOKENIZER_TOKEN_NUL;
    std::unordered_map<string_type, payload_type> keywordsMap;
    bool                                          caseSensitive      = true;

    payload_type                                  contextToken       = UMBA_TOKENIZER_TOKEN_NUL;

    // State
    bool                                          contextTokenFound  = false;

    IdentifierToKeywordConversionFilter( token_handler_type                                  curTokenHandler
                                       , payload_type                                        matchTo_
                                       , const std::unordered_map<string_type, payload_type> keywordsMap_
                                       , bool                                                caseSensitive_=true
                                 )
    : TBase(curTokenHandler)
    , matchTo      (matchTo_      )
    , keywordsMap  (keywordsMap_  )
    , caseSensitive(caseSensitive_)
    {
    }

    IdentifierToKeywordConversionFilter( token_handler_type                                  curTokenHandler
                                       , payload_type                                        matchTo_
                                       , payload_type                                        contextToken_
                                       , const std::unordered_map<string_type, payload_type> keywordsMap_
                                       , bool                                                caseSensitive_=true
                                 )
    : TBase(curTokenHandler)
    , matchTo      (matchTo_      )
    , keywordsMap  (keywordsMap_  )
    , caseSensitive(caseSensitive_)
    , contextToken (contextToken_ )
    {
    }

    bool reset(bool res = true)
    {
        contextTokenFound  = false;
        return TBase::reset(res);
    }

    bool operator()( TokenizerType           &tokenizer
                   , bool                    lineStartFlag
                   , payload_type            payloadToken
                   , iterator_type           &b
                   , iterator_type           &e
                   , token_parsed_data_type  &parsedData // std::variant<...>
                   , messages_string_type    &msg
                   )
    {
        if (payloadToken==UMBA_TOKENIZER_TOKEN_CTRL_RST)
        {
            return reset(this->callNextTokenHandler( tokenizer, lineStartFlag, payloadToken, b, e, parsedData, msg));
        }

        if (payloadToken==UMBA_TOKENIZER_TOKEN_CTRL_FIN)
        {
            return this->callNextTokenHandler( tokenizer, lineStartFlag, payloadToken, b, e, parsedData, msg);
        }

        if (contextToken!=UMBA_TOKENIZER_TOKEN_NUL)
        {
            // Используется контекст для преобразования
            if (!contextTokenFound)
            {
                // Контекст пока не найден
                if (payloadToken==contextToken)
                   contextTokenFound = true;

                // Просто прокидываем дальше
                return this->callNextTokenHandler(tokenizer, lineStartFlag, payloadToken, b, e, parsedData, msg);
            }
            else // Контекст найден ранее
            {
                if ( payloadToken==UMBA_TOKENIZER_TOKEN_LINEFEED
                  || payloadToken==UMBA_TOKENIZER_TOKEN_SPACE
                  || payloadToken==UMBA_TOKENIZER_TOKEN_TAB
                  || payloadToken==UMBA_TOKENIZER_TOKEN_FORM_FEED
                  || payloadToken==UMBA_TOKENIZER_TOKEN_LINE_CONTINUATION
                  || payloadToken==UMBA_TOKENIZER_TOKEN_OPERATOR_MULTI_LINE_COMMENT
                  || (payloadToken>=UMBA_TOKENIZER_TOKEN_OPERATOR_SINGLE_LINE_COMMENT_FIRST && payloadToken<=UMBA_TOKENIZER_TOKEN_OPERATOR_SINGLE_LINE_COMMENT_LAST)
                  //|| (payloadToken>= && payloadToken<=)
                  //|| payloadToken==
                   )
                {
                    // Эти токены просто прокидываем дальше даже пр наличии контекста
                    return this->callNextTokenHandler(tokenizer, lineStartFlag, payloadToken, b, e, parsedData, msg);
                }

                // Дальше используем обычную обработку
            }
        }

        // В любом случае сбрасываем признак, что ранее был найден контекстный токен
        contextTokenFound = false;

        if (payloadToken!=matchTo)
        {
            if (payloadToken==contextToken) // Но токен является контекстным, устанавливаем признак
               contextTokenFound = true;
            return this->callNextTokenHandler(tokenizer, lineStartFlag, payloadToken, b, e, parsedData, msg);
        }

        auto identifierData = std::get<typename TokenizerType::IdentifierData>(parsedData);
        string_type identifierStr = string_type(identifierData.data);
        //string_type identifier = umba::string_plus::make_string<string_type>(umba::iterator::makeString(tringLiteralData.suffixStartPos, itEnd));

        if (!caseSensitive)
            umba::string_plus::tolower(identifierStr);

        typename std::unordered_map<string_type, payload_type>::const_iterator kit = keywordsMap.find(identifierStr);

        if (kit==keywordsMap.end())
        {
            // Преобразование не найдено
            // Поэтому проверяем текущий токен, вдруг он контекстный
            if (contextToken!=UMBA_TOKENIZER_TOKEN_NUL && payloadToken==contextToken)
            {
                contextTokenFound = true;
            }
            return this->callNextTokenHandler(tokenizer, lineStartFlag, payloadToken, b, e, parsedData, msg);
        }
        else
        {
            return this->callNextTokenHandler(tokenizer, lineStartFlag, kit->second, b, e, parsedData, msg);
        }

    }


}; // struct IdentifierToKeywordConversionFilter

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
// Конвертирует токены из диапазона в один токен, и устанавливает нагрузку в размер офсета от стартового, добавяя дополнительное смещение
// Предназначено для операторов переменной длины - сводим к одному токену, а длину загружаем в token_parsed_data_type parsedData
// Минимальная длина - 1 (это фиксированный офсет по умолчанию)
template<typename TokenizerType, typename VectorType=std::vector<TokenInfo<TokenizerType> > >
struct TokenRangeConversionFilter : FilterBase<TokenizerType, VectorType>
{
    using TBase = FilterBase<TokenizerType, VectorType>;

    UMBA_TOKENIZER_TOKEN_FILTERS_DECLARE_USING_DEPENDENT_TYPES(TBase);
    using payload_type             = umba::tokenizer::payload_type        ;
    // using string_type              = typename TokenizerType::string_type  ;

    payload_type     rangeBegin  ;
    payload_type     rangeSize   ; 
    payload_type     targetToken ; 
    std::size_t      lenghtExtra ;


    UMBA_RULE_OF_FIVE(TokenRangeConversionFilter, default, default, default, default, default);

    TokenRangeConversionFilter( token_handler_type                                  curTokenHandler_
                              , payload_type                                        rangeBegin_
                              , payload_type                                        rangeSize_
                              , payload_type                                        targetToken_
                              , std::size_t                                         lenghtExtra_      = 0
                              )
    : TBase(curTokenHandler_  )
    , rangeBegin (rangeBegin_ )
    , rangeSize  (rangeSize_  )
    , targetToken(targetToken_)
    , lenghtExtra(lenghtExtra_)
    {
    }

    bool reset(bool res = true)
    {
        return TBase::reset(res);
    }

    bool operator()( TokenizerType           &tokenizer
                   , bool                    lineStartFlag
                   , payload_type            payloadToken
                   , iterator_type           &b
                   , iterator_type           &e
                   , token_parsed_data_type  &parsedData // std::variant<...>
                   , messages_string_type    &msg
                   )
    {
        if (payloadToken==UMBA_TOKENIZER_TOKEN_CTRL_RST)
        {
            return reset(this->callNextTokenHandler( tokenizer, lineStartFlag, payloadToken, b, e, parsedData, msg));
        }

        if (payloadToken==UMBA_TOKENIZER_TOKEN_CTRL_FIN)
        {
            return this->callNextTokenHandler( tokenizer, lineStartFlag, payloadToken, b, e, parsedData, msg);
        }

        if (payloadToken>=rangeBegin && payloadToken<(rangeBegin+rangeSize))
        {
            std::size_t lenght = payloadToken-rangeBegin+1;
            lenght += lenghtExtra;

            payloadToken = targetToken;

            typename TokenizerType::IntegerNumericLiteralData lenghtData;

            lenghtData.data        = lenght;
            lenghtData.fOverflow   = false ;
            lenghtData.hasSuffix   = false ;
            // lenghtData.suffixStartPos - ничего не задаём

            parsedData = lenghtData;
        }

        return this->callNextTokenHandler(tokenizer, lineStartFlag, payloadToken, b, e, parsedData, msg);
    }


}; // struct TokenRangeConversionFilter

//----------------------------------------------------------------------------

} // namespace filters
} // namespace tokenizer
} // namespace umba



