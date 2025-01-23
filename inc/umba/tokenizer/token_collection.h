#pragma once

#include "umba/tokenizer.h"

//
#include "tokenizer_log.h"

//
#include <vector>


//----------------------------------------------------------------------------
// umba::tokenizer::
namespace umba {
namespace tokenizer {

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
template<typename TokenBuilder> class TokenCollection;

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
template< typename PayloadType,
        , typename InputIteratorType
        , typename TokenParsedDataType
        >
struct TokenCollectionItem
{
    bool                      bLineStart;
    PayloadType               tokenType ;
    InputIteratorType         b;
    InputIteratorType         e;

protected:
    std::size_t               parsedDataIndex = std::size_t(-1); // Нефик иметь к этому полю свободный доступ

}; // struct TokenCollectionItem

template< typename PayloadType,
        , typename InputIteratorType
        , typename TokenParsedDataType
        >
using TokenCollectionList = std::vector< TokenCollectionItem<PayloadType, InputIteratorType, TokenParsedDataType> >;

template< typename TokenParsedDataType >
using TokenParsedDataCollectionList = std::vector<TokenParsedDataType>;



//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
template< typename PayloadType,
        , typename InputIteratorType
        , typename TokenParsedDataType
        >
struct TokenCollectionTokenHandler
{
    explicit TokenCollectionTokenHandler(TokenCollectionTokenHandler &tcl, TokenParsedDataCollectionList &tpdcl) : tokenCollectionList(tcl), tokenParsedDataCollectionList(tpdcl) {}

    // TODO: запретить все остальные конструкторы
    // TODO: запретить копирование

    template<typename TokenizerType>
    bool operator()(TokenizerType &tokenizer, bool bLineStart, PayloadType tokenType, InputIteratorType b, InputIteratorType e, TokenParsedDataType parsedData, typename TokenizerType::messages_string_type &errMsg)
    {
        if (tokenType==UMBA_TOKENIZER_TOKEN_CTRL_RST)
            return true;

        std::size_t parsedDataIndex = std::size_t(-1);
        if (parsedData.index()!=0)
        {
            auto idx = tokenParsedDataCollectionList.size();
            tokenParsedDataCollectionList.emplace_back(parsedData);
            parsedDataIndex = idx;
        }

        tokenCollectionList.emplace_back( TokenCollectionItem<PayloadType, InputIteratorType, TokenParsedDataType>{bLineStart, tokenType, b, e, parsedDataIndex} );
    
        return true; // Никогда тут не возвращаем ошибки
    }


protected:

    TokenCollectionList              &tokenCollectionList;
    TokenParsedDataCollectionList    &tokenParsedDataCollectionList;

}; // struct TokenCollectionTokenHandler

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
template<typename TokenBuilder>
class TokenCollection
{

public:

    using token_pos_type         = std::size_t;
    using tokenizer_type         = typename TokenBuilder::tokenizer_type;
    using payload_type           = umba::tokenizer::payload_type;
    using iterator_type          = typename tokenizer_type::iterator_type;
    using token_parsed_data_type = typename tokenizer_type::token_parsed_data_type;
    using shared_log_type        = umba::tokenizer::log::SharedParserErrorLog;


protected:

    TokenCollectionList              m_tokenCollectionList;
    TokenParsedDataCollectionList    m_tokenParsedDataCollectionList;
    tokenizer_type                   m_tokenizer;
    shared_log_type                  m_log;


// umba::tokenizer::log::
// using SharedParserErrorLog = std::shared_ptr<ParserErrorLog>;


    using 
    
    using user_data_type           = UserDataType;
    using char_type                = CharType;
    using value_type               = CharType;
    using char_class_table_type    = CharClassTableType;
    using trie_vector_type         = TrieVectorType;
    using string_type              = StringType;
    using iterator_type            = InputIteratorType;
    using iterator_traits_type     = InputIteratorTraits;
    using messages_string_type     = MessagesStringType;




// - getToken() возвращает следующий токен, сдвигая указатель (или считывет следующий токен из источника, если мы 
//   находимся в конце буфера токенов);
// - getTokenPos() возвращает текущую позицию в буфере токенов;
// - reset(pos) устанавливает позицию в буфере (аргумент должен быть получен из mark());
// - peekToken() возвращает следующий токен без сдвига позиции в буфере.

getPayloadData

};


// template< typename CharType            // Input chars type
//         , typename UserDataType        = void*
//         , typename CharClassTableType  = std::array<CharClass, 128>
//         , typename TrieVectorType      = std::vector<TrieNode>
//         , typename StringType          = std::basic_string<CharType>  // Тип строки, которая хранит входные символы
//         , typename MessagesStringType  = std::string  // Тип строки, которая используется для сообщений (в том числе и от внешних суб-парсеров)
//         , typename InputIteratorType   = umba::iterator::TextPositionCountingIterator<CharType>
//         , typename InputIteratorTraits = umba::iterator::TextIteratorTraits<InputIteratorType>
//         >
// class TokenizerFunctionHandlers : public TokenizerBaseImpl< TokenizerFunctionHandlers< CharType, UserDataType, CharClassTableType, TrieVectorType, StringType, MessagesStringType, InputIteratorType, InputIteratorTraits >
//                                                           , CharType
//                                                           , UserDataType
//                                                           , CharClassTableType
//                                                           , TrieVectorType
//                                                           , StringType
//                                                           , MessagesStringType
//                                                           , InputIteratorType
//                                                           , InputIteratorTraits
//                                                           >


    // tokenizer.tokenHandler = [&](auto &tokenizer, bool bLineStart, payload_type tokenType, InputIteratorType b, InputIteratorType e, token_parsed_data_type parsedData, messages_string_type &errMsg) -> bool
    //                          {
    //                              if (tokenType==UMBA_TOKENIZER_TOKEN_CTRL_RST || tokenType==UMBA_TOKENIZER_TOKEN_CTRL_FIN)
    //                                  return true;



//----------------------------------------------------------------------------

} // namespace tokenizer
} // namespace umba

