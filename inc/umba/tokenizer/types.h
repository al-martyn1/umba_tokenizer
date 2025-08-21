/*! \file
    \brief Типы данных для подсистемы токенизации
 */
#pragma once

//----------------------------------------------------------------------------
// umba::tokenizer::
namespace umba {
namespace tokenizer {

//----------------------------------------------------------------------------

// #if defined(UMBA_TARGET_BIT_SIZE) && UMBA_TARGET_BIT_SIZE>32
using small_size_t        = std::uint32_t;
using super_small_size_t  = std::uint16_t;

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------

} // namespace tokenizer
} // namespace umba

