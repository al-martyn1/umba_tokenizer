#pragma once

#ifndef UMBA_TOKENIZER_H__DCAEEDE0_7624_4E47_9919_08460EF65A3B__
    #error "Do not include this file directly, include 'umba/tokenizer.h header instead'"
#endif


#ifndef MARTY_ARG_USED

    //! Подавление варнинга о неиспользованном аргументе
    #define MARTY_ARG_USED(x)                   (void)(x)

#endif

#if !defined(USE_MARTY_BIGINT)
    #define USE_MARTY_BIGINT 1
#endif

#if !defined(USE_MARTY_DECIMAL)
    #define USE_MARTY_DECIMAL 1
#endif



#if defined(USE_MARTY_BIGINT) && USE_MARTY_BIGINT!=0
    #include "marty_bigint/marty_bigint.h"
    #if !defined(UMBA_TOKENOZER_MARTY_BIGINT_USED)
        #define UMBA_TOKENOZER_MARTY_BIGINT_USED
    #endif
#else
    #if defined(UMBA_TOKENOZER_MARTY_BIGINT_USED)
        #undef UMBA_TOKENOZER_MARTY_BIGINT_USED
    #endif
#endif

#if defined(USE_MARTY_DECIMAL) && USE_MARTY_DECIMAL!=0
    #include "marty_decimal/marty_decimal.h"
    #if !defined(UMBA_TOKENOZER_MARTY_DECIMAL_USED)
        #define UMBA_TOKENOZER_MARTY_DECIMAL_USED
    #endif
#else
    #if defined(UMBA_TOKENOZER_MARTY_DECIMAL_USED)
        #undef UMBA_TOKENOZER_MARTY_DECIMAL_USED
    #endif
#endif




// umba::tokenizer::
namespace umba {
namespace tokenizer {



} // namespace tokenizer
} // namespace umba

