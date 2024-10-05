#pragma once

#ifndef UMBA_TOKENIZER_H__DCAEEDE0_7624_4E47_9919_08460EF65A3B__
    #error "Do not include this file directly, include 'umba/tokenizer.h header instead'"
#endif

#include <unordered_set>
#include <exception>
#include <stdexcept>
#include <memory>
#include <initializer_list>


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
        , typename TokenizerType       = Tokenizer< CharType, UserDataType, CharClassTableType, TrieVectorType, StringType, MessagesStringType, InputIteratorType, InputIteratorTraits >
        >
class TokenizerBuilder
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
    using tokenizer_type           = TokenizerType;

    using ITokenizerLiteralParser  = umba::tokenizer::ITokenizerLiteralParser<CharType, MessagesStringType, InputIteratorType, InputIteratorTraits>;


//------------------------------
protected: // fileds

    char_class_table_type    charClassTable;

    TrieBuilder              numbersTrieBuilder;
    TrieBuilder              bracketsTrieBuilder;
    TrieBuilder              operatorsTrieBuilder;
    TrieBuilder              literalsTrieBuilder ;

    std::string              multiLineCommentEndStr; // Конец многострочного коментария ищем как строку, с забеганием вперёд

#if defined(NDEBUG)
    std::unordered_map<payload_type, string_type>  addedTokens;
#else
    std::map<payload_type, string_type>            addedTokens;
#endif
    std::vector<std::shared_ptr<ITokenizerLiteralParser> >   literalParsersStorage;
    //ITokenizerLiteralParser


//------------------------------
protected: // methods - helpers

    // Вообще-то надо бы через enable_if провить, есть ли метод resize, но пока так

    template<std::size_t N>
    static
    void checkReserveCharClassTable(std::array<CharClass, N> &cct)
    {
        UMBA_USED(cct);
        UMBA_ASSERT(cct.size()>=128);
    }

    template<typename ContainerType>
    static
    void checkReserveCharClassTable(ContainerType &cct)
    {
        if (cct.size()<128)
            cct.resize(128, CharClass::none);
    }

    bool isCharTableValidSizeAndNonZero() const
    {
        if (charClassTable.size()<128)
            return false;
        for(auto cc : charClassTable)
        {
            if (cc!=CharClass::none)
                return true;
        }
        return false;
    }

    void addTokenToKnownSet(payload_type tk, const string_type &tkStr, bool allowExisting=false)
    {
        if (addedTokens.find(tk)!=addedTokens.end())
        {
            if (!allowExisting)
                throw std::runtime_error("token already used: " + getTokenizerTokenStr<std::string>(tk));
            return;
        }
        addedTokens[tk] = tkStr;
    }

public:

    TokenizerBuilder& generateStandardCharClassTable()
    {
        checkReserveCharClassTable(charClassTable);
        generation::generateCharClassTable(charClassTable, false /* !addOperatorChars */, false /* addBrackets */ );
        return *this;
    }

    template<typename UpdaterType>
    TokenizerBuilder& generateStandardCharClassTable(UpdaterType updater)
    {
        checkReserveCharClassTable(charClassTable);
        generation::generateCharClassTable(charClassTable, false /* !addOperatorChars */, false /* addBrackets */ );
        updater(charClassTable);
        return *this;
    }

    template<typename GeneratorType>
    TokenizerBuilder& generateCustomCharClassTable(GeneratorType generator)
    {
        checkReserveCharClassTable(charClassTable);
        generator(charClassTable);
        return *this;
    }


protected:

    umba::tokenizer::CharClass setResetCharClassFlagsByIndex(std::size_t idx, umba::tokenizer::CharClass setFlags, umba::tokenizer::CharClass clrFlags)
    {
        UMBA_ASSERT(idx<charClassTable.size());

        auto res = charClassTable[idx];
        charClassTable[idx] |=  setFlags;
        charClassTable[idx] &= ~clrFlags;
        return res;
    }

public:

    //! Возвращает предыдущее состояние флагов
    umba::tokenizer::CharClass setResetCharClassFlags(CharType ch, umba::tokenizer::CharClass setFlags, umba::tokenizer::CharClass clrFlags)
    {
        return setResetCharClassFlagsByIndex(umba::tokenizer::charToCharClassTableIndex(ch), setFlags, clrFlags);
    }

    //! Меняет (set/reset) флаги для тех символов, текущее состояние флагов которых содержит все флаги из condFlags
    void conditionalFlagsAndSetResetCharClassFlags(umba::tokenizer::CharClass condFlags, umba::tokenizer::CharClass setFlags, umba::tokenizer::CharClass clrFlags)
    {
        for(std::size_t idx=0; idx!=charClassTable.size(); ++idx)
        {
            if ((charClassTable[idx]&condFlags)==condFlags)
            {
                setResetCharClassFlagsByIndex(idx, setFlags, clrFlags);
            }
        }
    }

    //! Меняет (set/reset) флаги для тех символов, текущее состояние флагов которых содержит хотя бы один флаг из condFlags
    void conditionalFlagsOrSetResetCharClassFlags(umba::tokenizer::CharClass condFlags, umba::tokenizer::CharClass setFlags, umba::tokenizer::CharClass clrFlags)
    {
        for(std::size_t idx=0; idx!=charClassTable.size(); ++idx)
        {
            if ((charClassTable[idx]&condFlags)!=0)
            {
                setResetCharClassFlagsByIndex(idx, setFlags, clrFlags);
            }
        }
    }

    umba::tokenizer::CharClass getCharClass(CharType ch) const
    {
        std::size_t idx = umba::tokenizer::charToCharClassTableIndex(ch); // clamp 127
        UMBA_ASSERT(idx<charClassTable.size());
        return charClassTable[idx];
    }

    TokenizerBuilder& setCharClassFlags(CharType ch, CharClass clsFlags)
    {
        generation::setCharClassFlags(charClassTable, ch, clsFlags);
        return *this;
    }

    TokenizerBuilder& addBrackets(const StringType &bracketsPair, payload_type pairBaseToken)
    {
        UMBA_ASSERT(isCharTableValidSizeAndNonZero()); // need call generateStandardCharClassTable/generateCustomCharClassTable first

        if (bracketsPair.size()!=2)
            throw std::runtime_error("requires exact brackets pair");

        //TODO: !!! Проверить, что pairBaseToken один из:
        // UMBA_TOKENIZER_TOKEN_CURLY_BRACKETS
        // UMBA_TOKENIZER_TOKEN_ROUND_BRACKETS
        // UMBA_TOKENIZER_TOKEN_ANGLE_BRACKETS
        // UMBA_TOKENIZER_TOKEN_SQUARE_BRACKETS
        // или 0, для автоопределения, и сделать автоопределение

        addTokenToKnownSet(pairBaseToken  , string_type(bracketsPair[0], pairBaseToken  ));
        addTokenToKnownSet(pairBaseToken+1, string_type(bracketsPair[1], pairBaseToken+1));

        generation::setCharClassFlagsForBracePair(charClassTable, bracketsPair);
        bracketsTrieBuilder.addTokenSequence(bracketsPair[0], pairBaseToken  );
        bracketsTrieBuilder.addTokenSequence(bracketsPair[1], pairBaseToken+1);

        return *this;
    }

    TokenizerBuilder& addNumbersPrefix(const StringType &prefix, payload_type tokenId, bool allowUseExistingToken=false) //
    {
        UMBA_ASSERT(isCharTableValidSizeAndNonZero()); // need call generateStandardCharClassTable/generateCustomCharClassTable first

        if (prefix.empty())
            throw std::runtime_error("number literal prefix can't be empty");

        if (!umba::TheFlags(charClassTable[charToCharClassTableIndex(prefix[0])]).oneOf(CharClass::digit))
            throw std::runtime_error("number literal prefix must starts with digit");

        //TODO: !!! Подумать, как сделать, чтобы числа можно было начинать с символов @ # $
        //TODO: !!! Проверить tokenId на вхождение в диапазон, или сделать автоопределение

        addTokenToKnownSet(tokenId, prefix, allowUseExistingToken);
        // numbersTrieBuilder.addTokenSequence(prefix, tokenId); // Не понятно, с чего я продублировал тут и на следующей строке в if'е - переглючило, наверное. Но может, был какой-то скрытый смысл
        if (numbersTrieBuilder.addTokenSequence(prefix, tokenId).payload!=tokenId)
            throw std::runtime_error("number literal prefix already used");

        return *this;
    }

    TokenizerBuilder& addSingleLineComment(const StringType &seq, payload_type tokenId, bool allowUseExistingToken=false)
    {
        UMBA_ASSERT(isCharTableValidSizeAndNonZero()); // need call generateStandardCharClassTable/generateCustomCharClassTable first

        if (seq.empty())
            throw std::runtime_error("single line comment sequence can't be empty");

        //TODO: !!! Проверить tokenId на вхождение в диапазон, или сделать автоопределение

        addTokenToKnownSet(tokenId, seq, allowUseExistingToken);
        if (operatorsTrieBuilder.addTokenSequence(seq, tokenId).payload!=tokenId)
            throw std::runtime_error("single line comment sequence already used");

        generation::setCharClassFlags(charClassTable, seq, CharClass::opchar);

        return *this;
    }

    TokenizerBuilder& setMultiLineComment(const StringType &seqStart, const StringType &seqEnd, payload_type tokenId=payload_invalid)
    {
        UMBA_ASSERT(isCharTableValidSizeAndNonZero()); // need call generateStandardCharClassTable/generateCustomCharClassTable first

        if (seqStart.empty())
            throw std::runtime_error("multi line comment start sequence can't be empty");

        if (seqEnd.empty())
            throw std::runtime_error("multi line comment end sequence can't be empty");

        if (tokenId==payload_invalid)
            tokenId = UMBA_TOKENIZER_TOKEN_OPERATOR_MULTI_LINE_COMMENT_START;

        addTokenToKnownSet(tokenId, seqStart);
        if (operatorsTrieBuilder.addTokenSequence(seqStart, tokenId).payload!=tokenId)
            throw std::runtime_error("multiline comment start sequence already used");

        // for(auto it=seqStart; it!=seqEnd; ++it)
        //     generation::setCharClassFlags(charClassTable, seq, CharClass::opchar);
        generation::setCharClassFlags(charClassTable, seqStart, CharClass::opchar);

        multiLineCommentEndStr = seqEnd;

        return *this;
    }

    TokenizerBuilder& addOperator(const StringType &seq, payload_type tokenId, bool allowUseExistingToken=false)
    {
        UMBA_ASSERT(isCharTableValidSizeAndNonZero()); // need call generateStandardCharClassTable/generateCustomCharClassTable first

        if (seq.empty())
            throw std::runtime_error("operator sequence can't be empty");

        //TODO: !!! Проверить tokenId на вхождение в диапазон, или сделать автоопределение

        addTokenToKnownSet(tokenId, seq, allowUseExistingToken);
        if (operatorsTrieBuilder.addTokenSequence(seq, tokenId).payload!=tokenId)
            throw std::runtime_error("operator sequence already used");

        generation::setCharClassFlags(charClassTable, seq, CharClass::opchar);

        return *this;
    }

    template<typename StringContainerType>
    TokenizerBuilder& addOperators(payload_type tokenId, const StringContainerType ops)
    {
        for(const auto &op : ops)
        {
            addOperator(op, tokenId++);
        }

        return *this;
    }

    TokenizerBuilder& addStringLiteralParser(const StringType &seq, ITokenizerLiteralParser *pParser, payload_type tokenId=payload_invalid)
    {
        UMBA_ASSERT(isCharTableValidSizeAndNonZero()); // need call generateStandardCharClassTable/generateCustomCharClassTable first

        if (seq.empty())
            throw std::runtime_error("string literal prefix sequence can't be empty");

        generation::setCharClassFlags(charClassTable, seq[0], CharClass::string_literal_prefix);
        if (tokenId==payload_invalid)
            tokenId = UMBA_TOKENIZER_TOKEN_STRING_LITERAL;

        literalsTrieBuilder.addTokenSequence(seq, tokenId).payloadExtra = reinterpret_cast<payload_type>(pParser);

        return *this;
    }

    template<typename LiteralParserType, typename... FilterCtorArgs >
    TokenizerBuilder& addStringLiteralParser(const StringType &seq, payload_type tokenId, FilterCtorArgs... args)
    {
        std::shared_ptr<LiteralParserType>        tmpParser          = std::make_shared<LiteralParserType>(std::forward<FilterCtorArgs>(args)...);
        std::shared_ptr<ITokenizerLiteralParser>  tmpParserInterface = std::static_pointer_cast<ITokenizerLiteralParser>(tmpParser);
        addStringLiteralParser(seq, tmpParserInterface.get(), tokenId);
        literalParsersStorage.emplace_back(tmpParserInterface);
        return *this;
    }

    template<typename LiteralParserType>
    TokenizerBuilder& addStringLiteralParser(const StringType &seq, payload_type tokenId=payload_invalid)
    {
        std::shared_ptr<LiteralParserType>        tmpParser          = std::make_shared<LiteralParserType>();
        std::shared_ptr<ITokenizerLiteralParser>  tmpParserInterface = std::static_pointer_cast<ITokenizerLiteralParser>(tmpParser);
        addStringLiteralParser(seq, tmpParserInterface.get(), tokenId);
        literalParsersStorage.emplace_back(tmpParserInterface);
        return *this;
    }

    template<typename LiteralParserType>
    TokenizerBuilder& addStringLiteralParser(payload_type tokenId, std::initializer_list<StringType> prefixList)
    {
        UMBA_ASSERT(tokenId!=payload_invalid);
        std::shared_ptr<LiteralParserType>        tmpParser          = std::make_shared<LiteralParserType>();
        std::shared_ptr<ITokenizerLiteralParser>  tmpParserInterface = std::static_pointer_cast<ITokenizerLiteralParser>(tmpParser);
        literalParsersStorage.emplace_back(tmpParserInterface);

        auto b = std::begin(prefixList);
        auto e = std::end(prefixList);
        for(auto it=b; it!=e; ++it, ++tokenId)
        {
            addStringLiteralParser(*it, tmpParserInterface.get(), tokenId);
        }

        return *this;
    }

    // Признак строкового литерала вторичен по отношению к признаку оператора, и когда они пересекаются, то признак стр. литерала должен включаться руками в необходимых случаях
    // Пример: C++ #include<bla-bla> - символы '<' и '>' - операторы, но в директиве препроцессора include они выступают маркерами строкового литерала специального типа
    TokenizerBuilder& clearStringLiteralFlagForOpChars()
    {
        conditionalFlagsAndSetResetCharClassFlags(CharClass::opchar, CharClass::none /* setFlags */ , CharClass::string_literal_prefix /* clrFlags */ );
        return *this;
    }


protected:

    tokenizer_type makeTokenizerImpl() const
    {
        tokenizer_type tokenizer;

        tokenizer.setCharClassTable(charClassTable);

        typename tokenizer_type::trie_vector_type  numbersTrie  ;
        typename tokenizer_type::trie_vector_type  bracketsTrie ;
        typename tokenizer_type::trie_vector_type  operatorsTrie;
        typename tokenizer_type::trie_vector_type  literalsTrie ;

        numbersTrieBuilder  .buildTokenTrie(numbersTrie  );
        bracketsTrieBuilder .buildTokenTrie(bracketsTrie );
        operatorsTrieBuilder.buildTokenTrie(operatorsTrie);
        literalsTrieBuilder .buildTokenTrie(literalsTrie );

        tokenizer.setNumbersTrie  (numbersTrie  );
        tokenizer.setBracketsTrie (bracketsTrie );
        tokenizer.setOperatorsTrie(operatorsTrie);
        tokenizer.setLiteralsTrie (literalsTrie );

        tokenizer.setMultiLineCommentEndString(multiLineCommentEndStr);

        tokenizer.addOwnershipForLiteralParsers(literalParsersStorage);

        return tokenizer;
    }

public:

    tokenizer_type makeTokenizer() const
    {
        auto copyOfThis = *this;
        // auto &copyOfThis = const_cast<TokenizerBuilder&>(*this); // !!! Вроде порешал проблему - был забыт return в одном месте
        return copyOfThis.clearStringLiteralFlagForOpChars().makeTokenizerImpl();
    }


}; // class TokenizerBuilder





} // namespace tokenizer
} // namespace umba

