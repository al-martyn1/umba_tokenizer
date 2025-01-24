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
        //, typename InputIteratorType
        //, typename TokenParsedDataType
        >
struct TokenCollectionItem
{

protected:

    friend TokenCollection;
    std::size_t  parsedDataIndex = std::size_t(-1); // Нефик иметь к этому полю свободный доступ


public:

    PayloadType               tokenType ;
    // InputIteratorType         b;
    // InputIteratorType         e;

    bool                      bLineStart;

    // struct tag_umba_text_position_info
    // umba_text_position_info_line_offset_type    lineOffset  ; //!< From data origin to line start
    // umba_text_position_info_symbol_offset_type  symbolOffset; //!< From line start
    // umba_text_position_info_symbol_offset_type  lineLen     ; //!< From start to first line end symbol or to end of text
    // umba_text_position_info_line_number_type    lineNumber  ; //!< Zero based line number
    // umba_text_position_info_file_id_type        fileId      ; //!< FileID

    // Размер umba_text_position_info = 5*sizeof(size_t)
    // для 32 бит: 5*4 = 20
    // для 64 бит: 5*8 = 40

    // Если убрать fileId - минус одно поле - 16/32

    // class TextPositionCountingIterator
    // const CharType*     m_pData     = 0;
    // std::size_t         m_dataSize  = 0;
    // std::size_t         m_dataIndex = 0;
    //  
    // TextPositionInfo    m_positionInfo;

    // Размер TextPositionCountingIterator
    // для 32 бит: 4 + 4 + 4 + 20 = 32
    // для 64 бит: 8 + 8 + 8 + 40 = 64


    // !!! У нас итераторы довольно жирные (32/64 байта), там овердофига всего хранится
    // допустим, токенов у нас в 4 раза меньше, чем текста - 
    // если много кода, то много мелких токенов - операторов и тп
    // но идентификаторы обычно длиннее 4х символов
    // если есть коментарии или строковые литералы - то они разбавляют плотность

    // Но пусть токенов в 4 раза меньше, чем текста
    // на 100Кб текста у нас 25К токенов - 800000 байт x86 или 1600000 байт x64. В 8/16 раз больше. Что-то овердофига

    // Для вывода сообщений об ошибке нам итераторы не нужны, только инфа о положении ошибки в тексте,
    // то есть 20/40
    // на 100Кб текста у нас 25К токенов - 500000 байт x86 или 1000000 байт x64. уже поменьше

    // Как бы нам сэкономить на хранении итераторов?
    // Итератора `e`?
    // Итератора `b`?
    // И нужно ли они нам вообще?

    // Нам нужна только позиция в файле для вывода сообщения об ошибке

    // А если нам нужно интерактивно показывать границы ошибки? Нужны обе позиции

    // Итераторы нам не нужны вроде бы ни в каком случае

    // Если нам нужна позиция окончания - то её можно сделать дельтой на минималках от стартовой позиции
    // При этом 
    // umba_text_position_info_line_offset_type    lineOffset  ; //!< From data origin to line start
    // umba_text_position_info_symbol_offset_type  symbolOffset; //!< From line start
    // umba_text_position_info_symbol_offset_type  lineLen     ; //!< From start to first line end symbol or to end of text
    // umba_text_position_info_line_number_type    lineNumber  ; //!< Zero based line number
    // umba_text_position_info_file_id_type        fileId      ; //!< FileID

    // Дельта на минималках:
    // lineOffset - дельта 16 бит - все строки дельты в сумме не могут быть больше 64К
    // symbolOffset - дельта 16 бит - смещение от начала строки - не может быть больше 64К
    // lineLen      - не может быть больше 64К
    // 




}; // struct TokenCollectionItem

template< typename PayloadType,
        //, typename TokenParsedDataType
        //, typename InputIteratorType
        >
using TokenCollectionList = std::vector< TokenCollectionItem<PayloadType /* , TokenParsedDataType, InputIteratorType */ > >;

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
    explicit TokenCollectionTokenHandler(TokenCollectionList &tcl, TokenParsedDataCollectionList &tpdcl) : tokenCollectionList(tcl), tokenParsedDataCollectionList(tpdcl) {}

    // TODO: запретить все остальные конструкторы
    // TODO: запретить копирование

    UMBA_RULE_OF_FIVE_COPY_DELETE(TokenCollectionTokenHandler);


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

        tokenCollectionList.emplace_back( TokenCollectionItem
                                          < PayloadType /* , TokenParsedDataType, InputIteratorType */ >
                                          { parsedDataIndex
                                          , tokenType
                                          // , b
                                          // , e
                                          , bLineStart
                                          } 
                                        );
    
        return true; // Никогда тут не возвращаем ошибки
    }


protected:

    TokenCollectionList              &tokenCollectionList;
    TokenParsedDataCollectionList    &tokenParsedDataCollectionList;

}; // struct TokenCollectionTokenHandler

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
//! Буферизирует токены. Производит отложенную токенизацию/токенизацию по запросу.
template<typename TokenizerType>
class TokenCollection
{

public:

    using token_pos_type         = std::size_t;
    using tokenizer_type         = TokenizerType; // typename TokenBuilder::tokenizer_type;
    using payload_type           = umba::tokenizer::payload_type;
    using iterator_type          = typename tokenizer_type::iterator_type;
    using token_parsed_data_type = typename tokenizer_type::token_parsed_data_type;
    using shared_log_type        = umba::tokenizer::log::SharedParserErrorLog;
    using string_type            = typename tokenizer_type::string_type; // Input chars string type


protected:

    mutable
    TokenCollectionList              m_tokenCollectionList;
    mutable
    TokenParsedDataCollectionList    m_tokenParsedDataCollectionList;
    tokenizer_type                   m_tokenizer;
    shared_log_type                  m_log;
    string_type                      m_text;
    iterator_type                    m_inputIt;
    iterator_type                    m_inputEndIt;
    token_pos_type                   m_tokenPos = token_pos_type(-1);
    bool                             m_lastTokenizeResult = false;


public:

    UMBA_RULE_OF_FIVE_COPY_MOVE_DELETE(TokenCollection);

    explicit TokenCollection(const tokenizer_type &tkn, shared_log_type log, const string_type &text)
    : m_tokenCollectionList()
    , m_tokenParsedDataCollectionList()
    , m_tokenizer(tkn)
    , m_log(log)
    , m_text(text)
    , m_inputIt   (iterator_type(m_text.data(), m_text.size()))
    , m_inputEndIt(iterator_type()) // !!! Надо наверное что-то придумать с итератором конца. Или не надо?
    {}


    /* Стратегия работы с индексом токена такая:

       - getToken() - возвращает текущий элемент, и передвигает позицию на следующий элемент.
         Также в выходном параметре getToken() возвращает текущий индекс в массиве токенов, 

       - peekToken() - возвращает текущий элемент, позиция остаётся без изменений.

       - нам нужно, чтобы peekToken() и getToken() возвращали одно и то же - значит, getToken()
         должен менять позицию постинкрементом.

       - getTokenPos() - должен возвращать позицию токена, которую вернул предыдущий getToken().
         getToken() следует вызывать только после вызова getTokenPos(), и использовать только тогда,
         когда индекс токена нужен редко. Если индекс токена нужен всегда, его следует получать
         через возвращаемый параметр метода getToken().

       - getNextTokenPos() - возвращает индекс следующего токена. Зачем бы это надо? Но пусть будет для ясности

    */

    bool getLastTokenizeResult() const
    {
        return m_lastTokenizeResult;
    }


    const token_parsed_data_type* getTokenParsedData(const TokenCollectionItem *ptki) const
    {
        if (!ptki)
             return 0; // или assert?

        std::size_t parsedDataIndex = ptki->parsedDataIndex;
        if (parsedDataIndex>=m_tokenParsedDataCollectionList.size())
            return 0;

        return &m_tokenParsedDataCollectionList[parsedDataIndex];
    }

    #if 0
    const token_parsed_data_type* getTokenParsedData() const
    {
        if (m_tokenPos>=m_tokenCollectionList.size())
            return 0; // к невалидному индексу данные не возвращаем - сначала надо получить токен инфу - вычитать по необходимости входной поток
                      // TODO: !!! может тут ассерт кинуть?

        return getTokenParsedData(&m_tokenCollectionList[m_tokenPos]);
    }
    #endif


    // возвращает следующий токен, сдвигая указатель (или считывет следующий токен из источника, 
    // если указатель за пределами текущего буферизированного набора токенов)
    // Взвращает 0, если следующего токена нет - достигнут конец последовательности, или произошла ошибка
    const TokenCollectionItem* getToken(token_pos_type *pTokenPos=0)
    {
        if (m_tokenPos<m_tokenCollectionList.size())
        {
            if (pTokenPos)
               *pTokenPos = m_tokenPos;
            return m_tokenCollectionList[m_tokenPos++];
        }

        // TODO: Вот тут надо вычитать следующий токен из входного текста
        //  
        // if (m_inputIt==m_inputEndIt) // Дальше двигать некуда
        // {
        //  
        // }

        auto prevTclSize = m_tokenCollectionList.size();

        m_lastTokenizeResult = true;
        for(; m_lastTokenizeResult
           && prevTclSize==m_tokenCollectionList.size()
           && m_inputIt!=m_inputEndIt
            ; ++m_inputIt
           )
        {
            // if (!tokenizer.tokenize(it, itEnd))
            // {
            //     m_lastTokenizeResult = false;
            // }

            m_lastTokenizeResult = tokenizer.tokenize(it, itEnd);
        }

        if (!m_lastTokenizeResult) // При ошибке возвращаем 0
            return 0;

        // Достигли конца. Не важно, изменилось ли у нас количество токенов в буфере, или нет -
        // по достижении конца входного текста нам надо вызвать финализацию
        if (m_inputIt==m_inputEndIt)
        {
            m_lastTokenizeResult = tokenizer.tokenizeFinalize(itEnd);
        }

        if (!m_lastTokenizeResult) // При финализации произошли ошибки
            return 0;

        // Пробуем взять токен из буфера
        if (m_tokenPos<m_tokenCollectionList.size())
        {
            if (pTokenPos)
               *pTokenPos = m_tokenPos;
            return m_tokenCollectionList[m_tokenPos++];
        }

        // Таки не получилось, неизвестная ошибка - 
        // TODO: надо как-то просигналить
        m_lastTokenizeResult = false;

        return 0;
    }


// - getToken() 

//   находимся в конце буфера токенов);
// - getTokenPos() возвращает текущую позицию в буфере токенов;
// - reset(pos) устанавливает позицию в буфере (аргумент должен быть получен из mark());
//   seekTokenPos() 
// - peekToken() возвращает следующий токен без сдвига позиции в буфере.




        // , typename CharType            // Input chars type
        // , typename UserDataType        = void*
        // , typename CharClassTableType  = std::array<CharClass, 128>
        // , typename TrieVectorType      = std::vector<TrieNode>
        // , typename StringType          = std::basic_string<CharType>  // Тип строки, которая хранит входные символы


// umba::tokenizer::log::
// using SharedParserErrorLog = std::shared_ptr<ParserErrorLog>;


    // using user_data_type           = UserDataType;
    // using char_type                = CharType;
    // using value_type               = CharType;
    // using char_class_table_type    = CharClassTableType;
    // using trie_vector_type         = TrieVectorType;
    // using string_type              = StringType;
    // using iterator_type            = InputIteratorType;
    // using iterator_traits_type     = InputIteratorTraits;
    // using messages_string_type     = MessagesStringType;





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

