/*! \file
    \brief Шаблон класса парсера
 */


#include "../utils.h"
#include "../parser_base2.h"
#include "types.h"
#include "umba/tokenizer/lexers/XXX.h"


//----------------------------------------------------------------------------
// #include "umba/tokenizer/parsers/XXX/parser.h"
// umba::tokenizer::XXX::
namespace umba {
namespace tokenizer {
namespace XXX {

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
template<typename TokenizerType, typename FilenameSetUserDataType=void*>
class Parser : public umba::tokenizer::ParserBase2<TokenizerType, FilenameSetUserDataType>
{

public: // types

    using BaseClass                = umba::tokenizer::ParserBase2<TokenizerType, FilenameSetUserDataType>;
    using tokenizer_type           = typename BaseClass::tokenizer_type             ;
    using TokenCollectionType      = typename BaseClass::TokenCollectionType        ;
    using TokenCollectionItemType  = typename BaseClass::TokenCollectionItemType    ;
    using TokenInfoType            = typename BaseClass::TokenInfoType              ;
    using TokenPosType             = typename BaseClass::TokenPosType               ;
    using token_parsed_data_type   = typename BaseClass::token_parsed_data_type     ;
    using shared_log_type          = typename BaseClass::shared_log_type            ;
    using string_type              = typename BaseClass::string_type                ;
    using FilenameSetType          = typename BaseClass::FilenameSetType            ;
    using SharedFilenameSetType    = typename BaseClass::SharedFilenameSetType      ;
    using filename_set_type        = typename BaseClass::filename_set_type          ;
    using shared_filename_set_type = typename BaseClass::shared_filename_set_type   ;
    using payload_type             = typename BaseClass::payload_type               ;

    using plid_list                = std::initializer_list<payload_type>;

protected:

    XXX_ParsingContext                 ctx;


public:

    UMBA_RULE_OF_FIVE_COPY_DELETE(Parser);
    UMBA_RULE_OF_FIVE_MOVE_DEFAULT(Parser);

    Parser() {}

    explicit
    Parser(std::shared_ptr<TokenCollectionType> tc, SharedFilenameSetType pFilenameSet, shared_log_type a_log)
    : BaseClass(tc, pFilenameSet, a_log)
    , ctx()
    {
    }

    XXX_data getParsedData() const
    {
        return ctx.rootNs;
    }

    FullTokenPosition getFullPos(TokenPosType pos) const  { return BaseClass::getFullPos(pos); }
    FullTokenPosition getFullPos() const                  { return BaseClass::getFullPos();    }
    payload_type getTokenType() const                     { return BaseClass::getTokenType(); }
    void readNextToken(ParserWaitForTokenFlags readFlags=ParserWaitForTokenFlags::none)   { BaseClass::readNextToken(readFlags); }
    string_type extractIdentifierName() const { return BaseClass::extractIdentifierName(); }
    string_type extractLiteral() const        { return BaseClass::extractLiteral() ;}

    const token_parsed_data_type* getTokenParsedDataPtr() const { return BaseClass::getTokenParsedDataPtr(); }
    token_parsed_data_type getTokenParsedData() const           { return BaseClass::getTokenParsedData(); }

    template <typename... TArgs>
    bool isTokenOneOf(TArgs&&... args) const
    {
        return ((getTokenType() == args) || ...);
    }

    void logSimpleMessage(const std::string &msgId, const std::string &msg) const { BaseClass::logSimpleMessage(msgId, msg); }
    void logSimpleMessage(TokenPosType pos, const std::string &msgId, const std::string &msg) const { BaseClass::logSimpleMessage(pos, msgId, msg); }
    void logSimpleMessage(TokenPosType pos, payload_type tokenType, const std::string &msgId, const std::string &msg) const { BaseClass::logSimpleMessage(pos, tokenType, msgId, msg); }
    void logSimpleUnexpected(payload_type payloadUnexpected) const { BaseClass::logSimpleUnexpected(payloadUnexpected); }
    void logSimpleUnexpected() const                               { BaseClass::logSimpleUnexpected(); }
    void logMessage(const std::string &msgId, const std::string &msg, const typename umba::FormatMessage<std::string>::macros_map_type &args ) { BaseClass::logMessage(msgId, msg, args ); }
    void logMessage(const std::string &msgId, const std::string &msg) { BaseClass::logMessage(msgId, msg ); }

    bool expectedReachedMsg(std::initializer_list<payload_type> payloadExpectedList, const std::string &msg=std::string()) const
    {
        return BaseClass::expectedReachedMsg(payloadExpectedList, msg);
    }

    bool expectedReachedMsg(payload_type payloadExpected, const std::string &msg=std::string()) const
    {
        return BaseClass::expectedReachedMsg(payloadExpected, msg);
    }

    bool checkExactTokenType(std::initializer_list<payload_type> payloadExpectedList, const std::string &msg=std::string()) const
    {
        return BaseClass::checkExactTokenType(payloadExpectedList, msg);
    }

    bool checkExactTokenType(payload_type payloadExpected, const std::string &msg=std::string()) const
    {
        return BaseClass::checkExactTokenType(payloadExpected, msg);
    }

    template<typename TokenHandler>
    bool readHomogeneousTokensList( payload_type tokenToken, payload_type tokenSep
                                  , bool readNextOnStart
                                  , TokenHandler handler
                                  , bool initialWaitSep = false
                                  )
    {
         return BaseClass::readHomogeneousTokensList(tokenToken, tokenSep, readNextOnStart, handler, initialWaitSep);
    }





}; // class Parser

//----------------------------------------------------------------------------


} // namespace XXX
} // namespace tokenizer
} // namespace umba
// umba::tokenizer::XXX::
// #include "umba/tokenizer/parsers/XXX/parser.h"

