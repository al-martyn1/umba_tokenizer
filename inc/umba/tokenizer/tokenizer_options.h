#pragma once

#ifndef UMBA_TOKENIZER_H__DCAEEDE0_7624_4E47_9919_08460EF65A3B__
    #error "Do not include this file directly, include 'umba/tokenizer.h header instead'"
#endif


#include "umba/the.h"
#include "umba/rule_of_five.h"
#include "umba/warnings/pop.h"

//
#include "defs.h"

//
#include <variant>
#include <array>
#include <algorithm>
#include <memory>


//----------------------------------------------------------------------------
// umba::tokenizer::
namespace umba {
namespace tokenizer {



enum class FloatingPointSeparatorType
{
    dot,
    comma,
    both

};

//----------------------------------------------------------------------------
#include "umba/warnings/push_disable_padding_added.h"
template<typename StringType> //  = std::basic_string<CharType>
struct TokenizerOptions
{
    using string_type = StringType;
    using value_type  = typename StringType::value_type;
    using char_type   = value_type;
    using CharType    = char_type;


    bool                        singleLineCommentOnlyAtBeginning = false;
    bool                        processLineContinuation          = true ;  // '\' before line feed marks next line to be continuation of current line
    bool                        numbersAllowRankSeparator        = true ;
    char_type                   numbersRankSeparator             = (char_type)'\'' ;  // apos ' (39/0x27) or backtick are good for this
    int                         numberDefaultBase                = 10   ;  // Система счисления по умолчанию, применяется, когда не был указан префикс, явно задающий систему счисления.
    bool                        tabsAsSpaces                     = true ;  // 
    bool                        disableFloatingPointNumbers      = false;
    bool                        allowUnclassifiedChars           = true ;  //
    FloatingPointSeparatorType  floatingPointSeparatorType       = FloatingPointSeparatorType::dot;
    // bool                        rawMode                          = false;
    // string_type                 rawModeStopSequence;


    payload_type getTabToken() const
    {
        return tabsAsSpaces ? UMBA_TOKENIZER_TOKEN_SPACE : UMBA_TOKENIZER_TOKEN_TAB;
    }

    bool isNumberRankSeparator(CharType ch) const
    {
        if (!numbersAllowRankSeparator)
            return false;

        return ch==numbersRankSeparator;
    }

    bool isFloatingPointSeparator(CharType ch) const
    {
        if (disableFloatingPointNumbers)
            return false;

        if (floatingPointSeparatorType==FloatingPointSeparatorType::dot   && ch==(CharType)'.')
            return true;
        if (floatingPointSeparatorType==FloatingPointSeparatorType::comma && ch==(CharType)',')
            return true;
        if (floatingPointSeparatorType==FloatingPointSeparatorType::both  && (ch==(CharType)'.' || ch==(CharType)','))
            return true;

        return false;
    }


};
#include "umba/warnings/pop.h"




} // namespace tokenizer
} // namespace umba


