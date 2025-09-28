/*! \file
    \brief Коллекция токенов.
 */
#pragma once

#include "../tokenizer.h"
//
#include "tokenizer_log.h"
#include "types.h"
//
#include "umba/text_position_info.h"
#include "umba/assert.h"

//
#include <vector>
#include <utility>
//#-sort
#include <exception>
#include <stdexcept>
//#+sort


//----------------------------------------------------------------------------
// umba::tokenizer::
namespace umba {
namespace tokenizer {

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
// template<typename TokenBuilder> class TokenCollection;

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
#include "umba/pushpack1.h"
template< typename PayloadType
        , typename CharType
        //, typename InputIteratorType
        //, typename TokenParsedDataType
        >
struct TokenCollectionItem
{

public:

    using string_type      = std::basic_string< CharType, std::char_traits<CharType>, std::allocator<CharType> >;
    using ConstCharTypePtr = const CharType*;
    //using TextPositionInfo = umba::TextPositionInfoNoFileId;
    using TextPositionInfo = umba::TextPositionInfo;

    template<typename TokenBuilder> friend class TokenCollection;


protected:

    // Нефик иметь к этому полю свободный доступ
    // Также, токенов не может быть больше четырех гиглв
    small_size_t           parsedDataIndex = small_size_t(-1); 


public:

    static constexpr inline std::size_t invalidParsedDataIndex = std::size_t(-1);

    //TextPositionInfo       textPosition;
    small_size_t           tokenLineNumber; // Не больше 2**32 строк
    std::size_t            tokenOffset ; // От начала файла
    super_small_size_t     textLen     ; // Токены не могут быть длиной больше 64К char'ов - это и так овердофига
    PayloadType            tokenType   ;
    bool                   bLineStart  ;

    UMBA_RULE_OF_FIVE_DEFAULT(TokenCollectionItem);

    TokenCollectionItem( small_size_t  pdi
                       , small_size_t  lineNo
                       , std::size_t   offs
                       , small_size_t  textLen_
                       , PayloadType   tt
                       , bool          bls
                       )
    : parsedDataIndex(pdi)
    , tokenLineNumber(lineNo)
    , tokenOffset (offs)
    , textLen     (super_small_size_t(textLen_))
    , tokenType   (tt)
    , bLineStart  (bls)
    {
        if (textLen_>=65535u)
            textLen = 65535u;
    } 

    std::size_t getParsedDataIndex() const
    {
        return parsedDataIndex==small_size_t(-1) ? invalidParsedDataIndex : std::size_t(parsedDataIndex);
    }


    string_type getText(const string_type &allText) const
    {
       std::size_t textEndOffset = tokenOffset + std::size_t(textLen);
       if (tokenOffset>=allText.size())
           return string_type();

       if (textEndOffset>allText.size())
           textEndOffset = allText.size();
       
       return string_type(allText, tokenOffset, textEndOffset-tokenOffset);
    }

    PayloadType getTokenType() const { return tokenType; }

    TextPositionInfo getPositionInfo(const std::string &text, std::size_t fileId) const
    {
        TextPositionInfo tpi;

        std::size_t lineOffset = tokenOffset;

        // Если мы находимся в конце строки, сдвигаемся с него в сторону начала текста
        if (lineOffset!=0 && text[lineOffset]=='\r')
            --lineOffset;
        if (lineOffset!=0 && text[lineOffset]=='\n')
            --lineOffset;

        while(lineOffset!=0 && (text[lineOffset]!='\n' && text[lineOffset]!='\r'))
            --lineOffset;

        std::size_t nextLineOffset = lineOffset;
        while(nextLineOffset<text.size() && (text[lineOffset]!='\n' && text[lineOffset]!='\r'))
            ++nextLineOffset;

        tpi.lineOffset   = lineOffset;
        tpi.symbolOffset = tokenOffset - lineOffset;
        tpi.lineLen      = nextLineOffset - lineOffset;
        tpi.lineNumber   = tokenLineNumber;
        tpi.fileId       = fileId;

        return tpi;
    }
    

    bool isTokenFin() const             { return tokenType==UMBA_TOKENIZER_TOKEN_CTRL_FIN; }
    bool isTokenSpaceExact() const      { return tokenType==UMBA_TOKENIZER_TOKEN_SPACE; }
    bool isTokenTab() const             { return tokenType==UMBA_TOKENIZER_TOKEN_TAB; }
    bool isTokenSpace() const           { return isTokenSpaceExact() || isTokenTab(); }
    bool isTokenLineFeed() const        { return tokenType==UMBA_TOKENIZER_TOKEN_LINEFEED; }
    bool isTokenFormFeed() const        { return tokenType==UMBA_TOKENIZER_TOKEN_FORM_FEED; }
    bool isTokenAnyLineFeed() const     { return isTokenLineFeed() || isTokenFormFeed(); } //!< Перевод строки или форм фид. FF тоже считаем за перевод строки
    bool isTokenAnySpace() const        { return isTokenSpace() || isTokenAnyLineFeed(); } //!< Любые пробелы и переводы строки, в тч FF

    // TODO: Ещё забыт вертикальный TAB
    // TODO: Символы меньше пробела - сейчас по идее, просто пропускаются, наверное надо сделать опцию,
    // чтобы они вызывали ошибку

    // bool isTokenSpace() const
    // {
    //     return tokenType
    // }


// #define UMBA_TOKENIZER_TOKEN_CTRL_FIN                                                 (UMBA_TOKENIZER_TOKEN_CTRL_FLAG|0x0000u) /* empty token, finalizes processing current text */
// //#define UMBA_TOKENIZER_TOKEN_UNEXPECTED                                               (UMBA_TOKENIZER_TOKEN_CTRL_FLAG|0x0001u)
//  
// // Для сброса состояния фильтров
// #define UMBA_TOKENIZER_TOKEN_CTRL_RST                                                 (UMBA_TOKENIZER_TOKEN_CTRL_FLAG|0x0001u)
//  
//  
//  
// #define UMBA_TOKENIZER_TOKEN_NUL                                                      0x0000u
// #define UMBA_TOKENIZER_TOKEN_UNEXPECTED                                               0x0001u
// #define UMBA_TOKENIZER_TOKEN_RAW_CHAR                                                 0x0002u
// #define UMBA_TOKENIZER_TOKEN_LINEFEED                                                 0x0003u
// #define UMBA_TOKENIZER_TOKEN_SPACE                                                    0x0004u
// #define UMBA_TOKENIZER_TOKEN_TAB                                                      0x0005u
// #define UMBA_TOKENIZER_TOKEN_FORM_FEED                                                0x0006u /* Надо добавить поддержку FF */
// #define UMBA_TOKENIZER_TOKEN_IDENTIFIER                                               0x0007u
// #define UMBA_TOKENIZER_TOKEN_SEMIALPHA                                                0x0008u
// #define UMBA_TOKENIZER_TOKEN_ESCAPE                                                   0x0009u
// #define UMBA_TOKENIZER_TOKEN_LINE_CONTINUATION                                        0x000Au








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

#include "umba/packpop.h"


template< typename PayloadType
        , typename CharType
        //, typename TokenParsedDataType
        //, typename InputIteratorType
        >
using TokenCollectionList = std::vector< TokenCollectionItem<PayloadType, CharType /* , TokenParsedDataType, InputIteratorType */ > >;

template< typename TokenParsedDataType >
using TokenParsedDataCollectionList = std::vector<TokenParsedDataType>; // TODO: !!! Надо переделать на unordered_map, чтобы можно было удалять элементы



//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
/*!
    Хандлер, который мы предаём в токенизер, когда хотим использовать TokenCollection.

 */

template< typename PayloadType
        , typename InputIteratorType
        , typename TokenParsedDataType
        >
struct TokenCollectionTokenHandler
{
    using CharType   = typename InputIteratorType::value_type;
    using value_type = CharType;
    using char_type  = CharType;
    using TokenCollectionItemType = TokenCollectionItem<PayloadType, CharType>;


    explicit TokenCollectionTokenHandler(TokenCollectionList<PayloadType, CharType> &tcl, TokenParsedDataCollectionList<TokenParsedDataType> &tpdcl)
    : tokenCollectionList(tcl), tokenParsedDataCollectionList(tpdcl)
    {}

    // TODO: запретить все остальные конструкторы
    // TODO: запретить копирование

    UMBA_RULE_OF_FIVE_COPY_DEFAULT(TokenCollectionTokenHandler);
    UMBA_RULE_OF_FIVE_MOVE_DEFAULT(TokenCollectionTokenHandler);


    template<typename TokenizerType>
    bool operator()( TokenizerType &tokenizer, bool bLineStart, PayloadType tokenType
                   , InputIteratorType b, InputIteratorType e
                   , TokenParsedDataType parsedData
                   , typename TokenizerType::messages_string_type &errMsg
                   )
    {
        UMBA_USED(errMsg);
        UMBA_USED(tokenizer);

        if (tokenType==UMBA_TOKENIZER_TOKEN_CTRL_RST)
            return true;

        small_size_t parsedDataIndex = small_size_t(-1);
        if (parsedData.index()!=0)
        {
            auto idx = tokenParsedDataCollectionList.size(); 
            if (idx==std::size_t(small_size_t(-1)))
                throw std::runtime_error("TokenCollection: tokenParsedDataCollectionList too large"); // 4 гига токенов с данными - это перебор
            tokenParsedDataCollectionList.emplace_back(parsedData);
            parsedDataIndex = (small_size_t)idx;
        }

        using ConstCharTypePtr = typename TokenCollectionItemType::ConstCharTypePtr;


        ConstCharTypePtr pb = b.getRawValueTypePointer();
        //ConstCharTypePtr pe = 0;
        std::size_t distanceCharT = 0;
        if (pb)
        {
            if (b==e)
            {
                if (!b.isEndIterator() && !b.isEndReached())
                    distanceCharT = b.symbolLength();
            }
            else
            {
                for(auto i=b; i!=e; ++i)
                {
                    distanceCharT += i.symbolLength();
                }
            }

            //pe = pb + distanceCharT;
        }

        auto tpi = b.getPosition(false); // do not findLineLen


        // TokenCollectionItem( std::size_t   pdi
        //                    , small_size_t  lineNo
        //                    , std::size_t   offs
        //                    , std::size_t   textStartOffset_
        //                    , small_size_t  textLen_
        //                    , PayloadType   tt
        //                    , bool          bls
        //                    )
        
        tokenCollectionList.emplace_back( TokenCollectionItemType
                                          ( parsedDataIndex
                                          , small_size_t(tpi.lineNumber) // tokenLineNumber
                                          // , tpi.lineOffset + tpi.symbolOffset // tokenOffset
                                          , b.getOffsetFromStart()
                                          , small_size_t(distanceCharT)
                                          , tokenType, bLineStart
                                          ) 
                                        );
    
        return true; // Никогда тут не возвращаем ошибки
    }


protected:

    TokenCollectionList<PayloadType, CharType>            &tokenCollectionList;
    TokenParsedDataCollectionList<TokenParsedDataType>    &tokenParsedDataCollectionList;

}; // struct TokenCollectionTokenHandler

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
struct FullTokenPosition
{
    using file_id_type      = umba::TextPositionInfo::file_id_type;
    using token_pos_type    = std::size_t;

    token_pos_type          pos    = 0u;
    file_id_type            fileId = (file_id_type)-1;
};


//----------------------------------------------------------------------------
//! Буферизирует токены. Производит отложенную токенизацию/токенизацию по запросу.
template<typename TokenizerType>
class TokenCollection
{

public:

    using token_pos_type                          = typename FullTokenPosition::token_pos_type; // std::size_t;
    using tokenizer_type                          = TokenizerType; // typename TokenBuilder::tokenizer_type;
    using payload_type                            = umba::tokenizer::payload_type;
    using iterator_type                           = typename tokenizer_type::iterator_type;
    using token_parsed_data_type                  = typename tokenizer_type::token_parsed_data_type;
    using token_empty_data_type                   = typename tokenizer_type::token_empty_data_type;
    using shared_log_type                         = umba::tokenizer::log::SharedParserErrorLog;
    using string_type                             = typename tokenizer_type::string_type; // Input chars string type
    using char_type                               = typename string_type::value_type;
    using TokenCollectionItemType                 = TokenCollectionItem<payload_type, char_type>;
    using token_collection_item_type              = TokenCollectionItemType;
    using token_handler_type                      = TokenCollectionTokenHandler<payload_type, iterator_type, token_parsed_data_type>;
    using token_collection_list_type              = TokenCollectionList<payload_type, char_type>;
    using token_parsed_data_collection_list_type  = TokenParsedDataCollectionList<token_parsed_data_type>;
    using file_id_type                            = umba::TextPositionInfo::file_id_type;


protected:

    mutable token_collection_list_type                m_tokenCollectionList;
    mutable token_parsed_data_collection_list_type    m_tokenParsedDataCollectionList;

    shared_log_type                  m_log;
    string_type                      m_text;
    iterator_type                    m_inputIt;
    iterator_type                    m_inputEndIt;
    token_pos_type                   m_nextTokenPos = 0;
    bool                             m_lastTokenizeResult = false;
    tokenizer_type                   m_tokenizer;
    file_id_type                     m_fileId = file_id_type(-1);

    TokenizerType initTokenizerHandlers(TokenizerType tokenizer) const
    {
        tokenizer.tokenHandler                      = token_handler_type(m_tokenCollectionList, m_tokenParsedDataCollectionList);
        tokenizer.unexpectedHandler                 = umba::tokenizer::log::TokenizerUnexpectedErrorHandler(m_log);
        tokenizer.reportUnknownOperatorHandler      = umba::tokenizer::log::TokenizerUnknownOperatorWarningHandler(m_log);
        tokenizer.reportStringLiteralMessageHandler = umba::tokenizer::log::TokenizerStringLiteralMessageHandler(m_log);
        return tokenizer;
    }

public:

    UMBA_RULE_OF_FIVE_COPY_DELETE(TokenCollection);

    TokenCollection() {}

    TokenCollection(TokenCollection &&tc)
    : m_tokenCollectionList(std::exchange(tc.m_tokenCollectionList, token_collection_list_type()))
    , m_tokenParsedDataCollectionList(std::exchange(tc.m_tokenParsedDataCollectionList, token_parsed_data_collection_list_type()))
    , m_log       (std::exchange(tc.m_log , shared_log_type()))
    , m_text      (std::exchange(tc.m_text, string_type()))
    , m_inputIt   (std::exchange(tc.m_inputIt   , iterator_type()))
    , m_inputEndIt(std::exchange(tc.m_inputEndIt, iterator_type()))
    , m_tokenizer (std::exchange(tc.m_tokenizer , tokenizer_type()))
    , m_fileId    (std::exchange(tc.m_fileId    , file_id_type(-1)))
    {
        // Нельзя перемещать набитые токенами объекты
        UMBA_ASSERT(tc.m_tokenCollectionList.empty());
        UMBA_ASSERT(tc.m_tokenParsedDataCollectionList.empty());
    }

    TokenCollection& operator=(TokenCollection &&tc)
    {
        // Нельзя перемещать набитые токенами объекты
        UMBA_ASSERT(tc.m_tokenCollectionList.empty());
        UMBA_ASSERT(tc.m_tokenParsedDataCollectionList.empty());

        m_tokenCollectionList = std::exchange(tc.m_tokenCollectionList, token_collection_list_type());
        m_tokenParsedDataCollectionList = std::exchange(tc.m_tokenParsedDataCollectionList, token_parsed_data_collection_list_type());
        m_log        = std::exchange(tc.m_log, shared_log_type());
        m_text       = std::exchange(tc.m_text, string_type());
        m_inputIt    = std::exchange(tc.m_inputIt   , iterator_type());
        m_inputEndIt = std::exchange(tc.m_inputEndIt, iterator_type());
        m_tokenizer  = std::exchange(tc.m_tokenizer , tokenizer_type());
        m_fileId     = std::exchange(tc.m_fileId    , file_id_type(-1));
        return *this;
    }

    // Конфигуратор нам нужен потому, что токенизеры устроены так:
    // сначала устанавливается финальный обработчик
    // затем перед ним устанавливаются фильтры.
    // Финальный хендлер у нас тут свой
    template<typename TokenizerConfigurator>
    explicit TokenCollection( tokenizer_type tkn
                            , TokenizerConfigurator tknConfigurator
                            , shared_log_type log
                            , const string_type &text
                            , file_id_type fileId=file_id_type(-1)
                            , std::size_t lineNumber = 0
                            )
    : m_tokenCollectionList()
    , m_tokenParsedDataCollectionList()
    , m_log(log)
    , m_text(text)
    //, m_inputIt   (iterator_type(m_text.data(), m_text.size()/*, std::size_t(fileId)*/))
    , m_inputIt(m_text.data(), m_text.size(), std::size_t(fileId))
    , m_inputEndIt(iterator_type()) // !!! Надо наверное что-то придумать с итератором конца. Или не надо?
    //, m_tokenizer(tknConfigurator(initTokenizerHandlers(std::move(tkn))))
    , m_tokenizer(std::move(tknConfigurator(std::move(initTokenizerHandlers(std::move(tkn))))))
    , m_fileId(fileId)
    {
        m_inputIt.setLineNumber(lineNumber);
    }

    void setLineNumber(std::size_t lineNumber)
    {
        m_inputIt.setLineNumber(lineNumber);
    }

    void setFileId(file_id_type fileId)
    {
        m_fileId = fileId;
        m_inputIt.setFileId(fileId);
    }

    file_id_type getFileId() const
    {
        return m_fileId;
    }      

    shared_log_type getLog() const
    {
        return m_log;
    }

    tokenizer_type& getTokenizer()
    {
        return m_tokenizer;
    }


    /* Стратегия работы с индексом токена такая:

       - внутренний индекс указывает на элемент, который будет возвращен при следующем вызове getToken().

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

    string_type& getText() { return m_text; }
    const string_type& getText() const { return m_text; }

    const char_type* getTextPointer(const TokenCollectionItemType *ptki) const
    {
        UMBA_ASSERT(ptki);
        return m_text.data()+ptki->tokenOffset;
    }

    std::string getTokenTextLine(const TokenCollectionItemType *ptki) const
    {
        UMBA_ASSERT(ptki);

        // Самому лень считать, а в итераторе уже сделан поиск начала строки
        auto posIt = umba::iterator::TextPositionCountingIterator<char>(m_text.data(), m_text.size(), m_fileId, ptki->tokenOffset);
        #if defined(DEBUG) || defined(_DEBUG)
        auto pStr = m_text.data() + std::ptrdiff_t(ptki->tokenOffset);
        UMBA_USED(pStr);
        #endif
        return umba::iterator::makeString(posIt.getLineStartIterator(), posIt.getLineEndIterator());
    }

    std::string getTokenTextLine(const TextPositionInfo &textPosInfo) const
    {
        auto posIt = umba::iterator::TextPositionCountingIterator<char>(m_text.data(), m_text.size(), textPosInfo.fileId, textPosInfo.lineOffset+textPosInfo.symbolOffset);
        #if defined(DEBUG) || defined(_DEBUG)
        auto pStr = m_text.data() + std::ptrdiff_t(textPosInfo.lineOffset+textPosInfo.symbolOffset);
        UMBA_USED(pStr);
        #endif
        return umba::iterator::makeString(posIt.getLineStartIterator(), posIt.getLineEndIterator());
    }

    bool getLastTokenizeResult() const
    {
        return m_lastTokenizeResult;
    }

    std::size_t getNumFetchedTokens() const
    {
        return m_tokenCollectionList.size();
    }

    const token_parsed_data_type* getTokenParsedDataPtr(const TokenCollectionItemType *ptki) const
    {
        UMBA_ASSERT(ptki);

        std::size_t parsedDataIndex = ptki->parsedDataIndex;
        if (parsedDataIndex>=m_tokenParsedDataCollectionList.size())
            return 0; // Нет данных

        return &m_tokenParsedDataCollectionList[parsedDataIndex];
    }

    token_parsed_data_type getTokenParsedData(const TokenCollectionItemType *ptki) const
    {
        UMBA_ASSERT(ptki);

        std::size_t parsedDataIndex = ptki->parsedDataIndex;
        if (parsedDataIndex>=m_tokenParsedDataCollectionList.size())
            return token_parsed_data_type{token_empty_data_type{}}; // Нет данных

        return m_tokenParsedDataCollectionList[parsedDataIndex];
    }


    TextPositionInfo getTokenPositionInfo(const TokenCollectionItemType *ptki) const
    {
        UMBA_ASSERT(ptki);

        UMBA_ASSERT(m_fileId!=file_id_type(-1));

        TextPositionInfo tpi = ptki->getPositionInfo(m_text, m_fileId);

        return tpi;
    }

    const TokenCollectionItemType* getTokenBysPos(token_pos_type pos) const
    {
        UMBA_ASSERT(pos<m_tokenCollectionList.size());
        if (pos>=m_tokenCollectionList.size())
            throw std::runtime_error("TokenCollection::getTokenBysPos: invalid pos");

        auto pRes = &m_tokenCollectionList[pos];
        return pRes;
    }

    const token_parsed_data_type* getTokenParsedData(token_pos_type pos) const
    {
        return getTokenParsedData(getTokenBysPos(pos));
    }

    TextPositionInfo getTokenPositionInfo(token_pos_type pos) const
    {
        return getTokenPositionInfo(getTokenBysPos(pos));
    }

    FullTokenPosition getFullTokenPosition(token_pos_type pos) const
    {
        UMBA_ASSERT(m_fileId!=file_id_type(-1));
        return FullTokenPosition{pos, m_fileId};
    }

    // const TokenCollectionItemType* getToken(token_pos_type *pTokenPos=0)
    // {
    //     if (m_nextTokenPos<m_tokenCollectionList.size()) // Ничего вычитывать со входа не нужно
    //     {
    //         auto pRes = &m_tokenCollectionList[m_nextTokenPos];
    //         if (pTokenPos)
    //            *pTokenPos = m_nextTokenPos;
    //  
    //         if (pRes->isTokenFin()) // если наткнулись на финализирующий токен, то двигаться по входу дальше не надо, всегда будем возвращать FIN
    //             return pRes;
    //  
    //         // Таки перемещаем позицию на следующий токен
    //         ++m_nextTokenPos;
    //  
    //         return pRes;
    //     }
    //  
    //     // Требуется вычитка следующего токена, поэтому вызываем "тяжелую" основную реализацию, и нам пофик, что она возвращает
    //     // TODO: может не надо в getTokenImpl проверять доступность? Или пофик, одно условие, зато peekToken() упрощается
    //  
    //     return getTokenImpl(pTokenPos);
    // }


    string_type getTokenText(const TokenCollectionItemType *ptki) const
    {
        UMBA_ASSERT(ptki);
        return ptki->getText(m_text);
    }


    token_pos_type getNextTokenPos() const
    {
        return m_nextTokenPos;
    }

    token_pos_type getCurTokenPos() const
    {
        UMBA_ASSERT(m_nextTokenPos!=0); // Если getToken() не вызывался, это ошибка использования API
        return m_nextTokenPos-1;
    }

    void setTokenPos(token_pos_type pos)
    {
        m_nextTokenPos = pos; // +1;
    }

    // Удаляет все вычитанное ранее, начиная с тукущей позиции
    void clearFetched()
    {
        std::size_t posFromTextStart = 0;
        std::size_t tokenLineNumber  = 0;
        if (!m_tokenCollectionList.empty())
        {
            posFromTextStart = m_tokenCollectionList.back().tokenOffset;
            tokenLineNumber  = m_tokenCollectionList.back().tokenLineNumber;
        }
        while(m_nextTokenPos<m_tokenCollectionList.size())
        {
            const auto &tci = m_tokenCollectionList.back();
            posFromTextStart = tci.tokenOffset;
            tokenLineNumber  = tci.tokenLineNumber;

            auto pdi = tci.getParsedDataIndex();
            if (pdi!=tci.invalidParsedDataIndex)
            {
                if (pdi==m_tokenParsedDataCollectionList.size())
                    m_tokenParsedDataCollectionList.pop_back();
            }

            m_tokenCollectionList.pop_back();
        }


        // m_tokenizer.resetFilters(); // не нужно - делается при установке сырого режима
        m_inputIt = iterator_type(m_text.data(), m_text.size(), std::size_t(m_fileId), posFromTextStart);
        m_inputIt.setLineNumber(tokenLineNumber);
    }

    //! Возвращает TokenCollectionItemType, в опциональном выходном параметре возвращается текущий индекс элемента
    const TokenCollectionItemType* getToken(token_pos_type *pTokenPos=0)
    {
        if (m_nextTokenPos<m_tokenCollectionList.size()) // Ничего вычитывать со входа не нужно
        {
            if (pTokenPos)
               *pTokenPos = m_nextTokenPos;

            //auto pRes = &m_tokenCollectionList[m_nextTokenPos];
            auto pRes = getTokenBysPos(m_nextTokenPos);

            return pRes->isTokenFin() ? pRes : (m_nextTokenPos++, pRes);

            // if (pRes->isTokenFin()) // если наткнулись на финализирующий токен, то двигаться по входу дальше не надо, всегда будем возвращать FIN
            //     return pRes;
            //  
            // // Таки перемещаем позицию на следующий токен
            // ++m_nextTokenPos;
            //  
            // return pRes;
        }

        // Требуется вычитка следующего токена, поэтому вызываем "тяжелую" основную реализацию, и нам пофик, что она возвращает
        // TODO: может не надо в getTokenImpl проверять доступность? Или пофик, одно условие, зато peekToken() упрощается

        return getTokenImpl(pTokenPos);
    }

    //! Не смещает указатель токенов, поэтому получать текущую позицию не надо
    const TokenCollectionItemType* peekToken()
    {
        token_pos_type tokenPos = 0;
        const TokenCollectionItemType* pToken = getTokenImpl(&tokenPos);
        if (pToken)
            setTokenPos(tokenPos);
        return pToken;
    }


    std::size_t getNumberOfTokensTotal() const
    {
        return m_tokenCollectionList.size();
    }

    std::size_t getNumberOfTokenDataTotal() const
    {
        return m_tokenParsedDataCollectionList.size();
    }

    std::size_t getBytesOfTokensTotal() const
    {
        return getNumberOfTokensTotal()*sizeof(typename token_collection_list_type::value_type);
    }

    // selfSize()
    std::size_t getBytesOfTokenDataTotal() const
    {
        std::size_t totalSize = 0;
        for(auto &&v: m_tokenParsedDataCollectionList)
        {
            // See Перегрузка лямбды - https://en.cppreference.com/w/cpp/utility/variant/visit
            std::visit( [&](const auto &v)
                        {
                            totalSize += v.selfSize();
                        }
                        , v
                      );
        }

        return totalSize;
        //return getNumberOfTokenDataTotal()*sizeof(typename token_parsed_data_collection_list_type::value_type);
    }


    // using token_collection_item_type              = TokenCollectionItemType;
    // using token_handler_type                      = TokenCollectionTokenHandler<payload_type, iterator_type, token_parsed_data_type>;
    // using token_collection_list_type              = TokenCollectionList<payload_type, char_type>;
    // using token_parsed_data_collection_list_type  = TokenParsedDataCollectionList<token_parsed_data_type>;
    // using file_id_type                            = umba::TextPositionInfo::file_id_type;



protected:

    //! Возвращает следующий токен, сдвигая указатель (или считывет следующий токен из источника, 
    //! если указатель за пределами текущего буферизированного набора токенов)
    //! Взвращает 0, если следующего токена нет - достигнут конец входной последовательности, или произошла ошибка
    //! Надо понимать, что в нормальном случае 0 не должен возвращаться - по достижении конца входа
    //! возвращается токен UMBA_TOKENIZER_TOKEN_CTRL_FIN, а не ноль
    const TokenCollectionItemType* getTokenImpl(token_pos_type *pTokenPos=0)
    {
        if (m_nextTokenPos<m_tokenCollectionList.size())
        {
            if (pTokenPos)
               *pTokenPos = m_nextTokenPos;
            return &m_tokenCollectionList[m_nextTokenPos++];
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

            m_lastTokenizeResult = m_tokenizer.tokenize(m_inputIt, m_inputEndIt);
        }

        if (!m_lastTokenizeResult) // При ошибке возвращаем 0
            return 0;

        // Достигли конца. Не важно, изменилось ли у нас количество токенов в буфере, или нет -
        // по достижении конца входного текста нам надо вызвать финализацию
        if (m_inputIt==m_inputEndIt)
        {
            m_lastTokenizeResult = m_tokenizer.tokenizeFinalize(m_inputIt, m_inputEndIt);
            // Тут в нормальном случае должен капнуть токен UMBA_TOKENIZER_TOKEN_CTRL_FIN
        }

        if (!m_lastTokenizeResult) // При финализации произошли ошибки
            return 0;

        // Пробуем взять токен из буфера
        if (m_nextTokenPos<m_tokenCollectionList.size())
        {
            if (pTokenPos)
               *pTokenPos = m_nextTokenPos;
            return &m_tokenCollectionList[m_nextTokenPos++];
        }

        // Таки не получилось, неизвестная ошибка
        // TODO: надо как-то просигналить,
        // потому что снаружы мы генерим только ошибки синтаксического анализа
        // а ошибки лексера генерируются им самим. Но раз лексер вернул true, то там ошибки не было,
        // но, тем не менее, финальный токен не появился в массиве токенов - что-то пошло не так.
        // unexpected-что?

        m_lastTokenizeResult = false;

        UMBA_ASSERT_FAIL();

        return 0;
    }

}; // class TokenCollection




//----------------------------------------------------------------------------

} // namespace tokenizer
} // namespace umba

