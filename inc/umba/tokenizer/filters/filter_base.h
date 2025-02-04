#pragma once

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
//! Предоставляет функционал буферизации токенов
/*! Возможно, тут стоит использовать boost::small_vector.
    Хотя, он тоже лезет в кучу, а единожды увеличившийся стандартный вектор не склонен к уменьшению объема
    аллоцированной памяти, если не вызывать shrink_to_fit
 */
template<typename TokenizerType, typename VectorType=std::vector<TokenInfo<TokenizerType> > >
struct FilterBase
{
    UMBA_TOKENIZER_TOKEN_FILTERS_DECLARE_USING_DEPENDENT_TYPES(TokenizerType);
    using payload_type               = umba::tokenizer::payload_type;
    using token_buffer_vector_type   = VectorType;

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




} // namespace filters
} // namespace tokenizer
} // namespace umba



