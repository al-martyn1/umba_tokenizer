#pragma once

#include "filter_base.h"


//----------------------------------------------------------------------------
// umba::tokenizer::filters::
namespace umba {
namespace tokenizer {
namespace filters {

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

                    token_parsed_data_type token_parsed_data = typename TokenizerType::EmptyData();
                    if (!nextTokenHandler(tokenizer, lineStartFlag, UMBA_TOKENIZER_TOKEN_CTRL_CC_PP_START, e, e, token_parsed_data /* strValue */ , msg))
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

} // namespace filters
} // namespace tokenizer
} // namespace umba



