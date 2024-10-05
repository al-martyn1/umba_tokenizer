#pragma once

#ifndef UMBA_TOKENIZER_H__DCAEEDE0_7624_4E47_9919_08460EF65A3B__
    #error "Do not include this file directly, include 'umba/tokenizer.h header instead'"
#endif

#include  <functional>

// umba::tokenizer::
namespace umba {
namespace tokenizer {


template< typename CharType            // Input chars type
        , typename UserDataType        = void*
        , typename CharClassTableType  = std::array<CharClass, 128>
        , typename TrieVectorType      = std::vector<TrieNode>
        , typename StringType          = std::basic_string<CharType>  // Тип строки, которая хранит входные символы
        , typename MessagesStringType  = std::string  // Тип строки, которая используется для сообщений (в том числе и от внешних суб-парсеров)
        , typename InputIteratorType   = umba::iterator::TextPositionCountingIterator<CharType>
        , typename InputIteratorTraits = umba::iterator::TextIteratorTraits<InputIteratorType>
        >
class TokenizerFunctionHandlers : public TokenizerBaseImpl< TokenizerFunctionHandlers< CharType, UserDataType, CharClassTableType, TrieVectorType, StringType, MessagesStringType, InputIteratorType, InputIteratorTraits >
                                                          , CharType
                                                          , UserDataType
                                                          , CharClassTableType
                                                          , TrieVectorType
                                                          , StringType
                                                          , MessagesStringType
                                                          , InputIteratorType
                                                          , InputIteratorTraits
                                                          >
{

//------------------------------
public: // depending types

    using TBase = TokenizerBaseImpl< TokenizerFunctionHandlers
                                   , CharType
                                   , UserDataType
                                   , CharClassTableType
                                   , TrieVectorType
                                   , StringType
                                   , MessagesStringType
                                   , InputIteratorType
                                   , InputIteratorTraits
                                   >;

    using TSelf = TokenizerFunctionHandlers< CharType
                                           , UserDataType
                                           , CharClassTableType
                                           , TrieVectorType
                                           , StringType
                                           , MessagesStringType
                                           , InputIteratorType
                                           , InputIteratorTraits
                                           >;

    friend TBase;

    using user_data_type           = typename TBase::user_data_type       ;
    using char_type                = typename TBase::char_type            ;
    using value_type               = typename TBase::value_type           ;
    using char_class_table_type    = typename TBase::char_class_table_type;
    using trie_vector_type         = typename TBase::trie_vector_type     ;
    using string_type              = typename TBase::string_type          ;
    using iterator_type            = typename TBase::iterator_type        ;
    using iterator_traits_type     = typename TBase::iterator_traits_type ;
    using messages_string_type     = typename TBase::messages_string_type ;

    using ITokenizerLiteralParser  = typename TBase::ITokenizerLiteralParser;

    using token_parsed_data_type   = typename TBase::token_parsed_data_type;


//------------------------------
public: // ctors and op=

    TokenizerFunctionHandlers() = default;
    TokenizerFunctionHandlers(const TokenizerFunctionHandlers &) = delete;
    TokenizerFunctionHandlers& operator=(const TokenizerFunctionHandlers &) = delete;
    TokenizerFunctionHandlers(TokenizerFunctionHandlers &&) = default;
    TokenizerFunctionHandlers& operator=(TokenizerFunctionHandlers &&) = default;


//------------------------------
public: // handler types

    using token_handler_type                         = std::function<bool( TSelf &tokenizer
                                                                         , bool
                                                                         , payload_type
                                                                         , InputIteratorType&
                                                                         , InputIteratorType&
                                                                         , token_parsed_data_type // std::basic_string_view<value_type>
                                                                         , messages_string_type&
                                                                         )
                                                                    >;
    using unexpected_handler_type                    = std::function<bool(TBase &tokenizer, InputIteratorType, InputIteratorType, const char*, int)>;
    using report_unknown_operator_handler_type       = std::function<void(TBase &tokenizer, InputIteratorType,InputIteratorType)>;
    using report_string_literal_message_handler_type = std::function<void(TBase &tokenizer, bool, InputIteratorType, const MessagesStringType &)>;
    using report_handle_token_error                  = std::function<void(TBase &tokenizer, payload_type, InputIteratorType, InputIteratorType, const MessagesStringType &)>;

    // void reportHandleTokenErrorLambda(payload_type tokenType, InputIteratorType inputDataBegin, InputIteratorType inputDataEnd, const MessagesStringType &msg) const
    // {
    //     static_cast<const TBase*>(this)->reportHandleTokenError(tokenType, inputDataBegin, inputDataEnd, msg);
    // }


// void parsingFailedMessageLambda(payload_type tokenType, InputIteratorType inputDataBegin, InputIteratorType inputDataEnd, const MessagesStringType &msg) const


//------------------------------
public: // handlers

    token_handler_type                          tokenHandler                     ;
    unexpected_handler_type                     unexpectedHandler                ;
    report_unknown_operator_handler_type        reportUnknownOperatorHandler     ;
    report_string_literal_message_handler_type  reportStringLiteralMessageHandler;
    report_handle_token_error                   reportHandleTokenErrorHandler    ;



    template<typename FilterType, typename... FilterCtorArgs >
    void installTokenFilter( FilterCtorArgs... args)
    {
        tokenHandler = FilterType(tokenHandler, std::forward<FilterCtorArgs>(args)...);
    }


//------------------------------
#if defined(UMBA_GCC_COMPILER_USED) // С GCC какая-то проблемка.
public:
#else
protected: // handler methods, called from base
// public:
#endif


    bool hadleToken( bool bLineStart, payload_type tokenType
                   , InputIteratorType &inputDataBegin, InputIteratorType &inputDataEnd
                   , token_parsed_data_type parsedData // std::basic_string_view<value_type> parsedData
                   , MessagesStringType &msg
                   ) const
    {
        if (tokenHandler)
           return tokenHandler(const_cast<TSelf&>(*this), bLineStart, tokenType, inputDataBegin, inputDataEnd, parsedData, msg);
        return true;
    }

    bool hadleUnexpected(InputIteratorType it, InputIteratorType itEnd, const char* srcFile, int srcLine) const
    {
        if (unexpectedHandler)
            return unexpectedHandler(const_cast<TSelf&>(*this), it, itEnd, srcFile, srcLine);
        return false;
    }

    // Дополнительное сообщение о неизвестном операторе, перед вызовом unexpectedHandlerLambda - типа чуть чуть улучшили диагностику
    void reportPossibleUnknownOperator(InputIteratorType b, InputIteratorType e) const
    {
        if (reportUnknownOperatorHandler)
            reportUnknownOperatorHandler(const_cast<TSelf&>(*this), b, e);
    }

    // Либо предупреждение, либо сообщение об ошибке от парсера литералов
    void reportStringLiteralMessage(bool bErr, InputIteratorType it, const MessagesStringType &msg) const
    {
        if (reportStringLiteralMessageHandler)
            reportStringLiteralMessageHandler(const_cast<TSelf&>(*this), bErr, it, msg);
    }

    void reportHandleTokenError(payload_type tokenType, InputIteratorType inputDataBegin, InputIteratorType inputDataEnd, const MessagesStringType &msg) const
    {
        if (reportHandleTokenErrorHandler)
            reportHandleTokenErrorHandler(const_cast<TSelf&>(*this), tokenType, inputDataBegin, inputDataEnd, msg);
    }



}; // class TokenizerFunctionHandlers



} // namespace tokenizer
} // namespace umba

