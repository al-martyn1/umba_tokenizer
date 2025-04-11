#pragma once

#include "filter_base.h"


//----------------------------------------------------------------------------
// umba::tokenizer::filters::
namespace umba {
namespace tokenizer {
namespace filters {

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
//! Фильтр, который буферизирует токены UMBA_TOKENIZER_TOKEN_RAW_CHAR
template<typename TokenizerType>
using UnclassifiedCharsCollectingFilter = TokenCollectingFilter<TokenizerType, UMBA_TOKENIZER_TOKEN_UNCLASSIFIED_CHAR>;

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------

} // namespace filters
} // namespace tokenizer
} // namespace umba



