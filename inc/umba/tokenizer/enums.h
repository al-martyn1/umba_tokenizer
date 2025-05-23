/*! \file
    \brief Autogenerated enumerations (Umba Enum Gen)
 */

#pragma once


#include "marty_cpp/marty_enum.h"
#include "marty_cpp/marty_flag_ops.h"
#include "marty_cpp/marty_flags.h"

#include <exception>
#include <map>
#include <stdexcept>
#include <string>
#include <unordered_map>



namespace umba{
namespace tokenizer{

//#!ParserWaitForTokenFlags
enum class ParserWaitForTokenFlags : std::uint32_t
{
    stopOnSingleLineComment   = 0x0001 /*!< Остановится на однострочном коменте */,
    stopOnMultiLineComment    = 0x0002 /*!< Остановится на многострочном коменте */,
    stopOnComment             = 0x0003 /*!< Остановится на любом коменте */,
    stopOnSpace               = 0x0004 /*!< Остановится на пробельном символе (space/tab) */,
    stopOnLinefeed            = 0x0008 /*!< Остановится на переводе строки (или FormFeed) */,
    stopOnAnySpace            = 0x000C /*!< Остановится на любом пробельном символе (space|lf) */,
    stopOnAny                 = 0x000F /*!<  */

}; // enum 
//#!

MARTY_CPP_MAKE_ENUM_FLAGS(ParserWaitForTokenFlags)

MARTY_CPP_ENUM_FLAGS_SERIALIZE_BEGIN( ParserWaitForTokenFlags, std::map, 1 )
    MARTY_CPP_ENUM_FLAGS_SERIALIZE_ITEM( ParserWaitForTokenFlags::stopOnSingleLineComment , "StopOnSingleLineComment" );
    MARTY_CPP_ENUM_FLAGS_SERIALIZE_ITEM( ParserWaitForTokenFlags::stopOnMultiLineComment  , "StopOnMultiLineComment"  );
    MARTY_CPP_ENUM_FLAGS_SERIALIZE_ITEM( ParserWaitForTokenFlags::stopOnComment           , "StopOnComment"           );
    MARTY_CPP_ENUM_FLAGS_SERIALIZE_ITEM( ParserWaitForTokenFlags::stopOnSpace             , "StopOnSpace"             );
    MARTY_CPP_ENUM_FLAGS_SERIALIZE_ITEM( ParserWaitForTokenFlags::stopOnAny               , "StopOnAny"               );
    MARTY_CPP_ENUM_FLAGS_SERIALIZE_ITEM( ParserWaitForTokenFlags::stopOnLinefeed          , "StopOnLinefeed"          );
    MARTY_CPP_ENUM_FLAGS_SERIALIZE_ITEM( ParserWaitForTokenFlags::stopOnAnySpace          , "StopOnAnySpace"          );
MARTY_CPP_ENUM_FLAGS_SERIALIZE_END( ParserWaitForTokenFlags, std::map, 1 )

MARTY_CPP_ENUM_FLAGS_DESERIALIZE_BEGIN( ParserWaitForTokenFlags, std::map, 1 )
    MARTY_CPP_ENUM_FLAGS_DESERIALIZE_ITEM( ParserWaitForTokenFlags::stopOnSingleLineComment , "stop-on-single-line-comment" );
    MARTY_CPP_ENUM_FLAGS_DESERIALIZE_ITEM( ParserWaitForTokenFlags::stopOnSingleLineComment , "stop_on_single_line_comment" );
    MARTY_CPP_ENUM_FLAGS_DESERIALIZE_ITEM( ParserWaitForTokenFlags::stopOnSingleLineComment , "stoponsinglelinecomment"     );
    MARTY_CPP_ENUM_FLAGS_DESERIALIZE_ITEM( ParserWaitForTokenFlags::stopOnMultiLineComment  , "stop-on-multi-line-comment"  );
    MARTY_CPP_ENUM_FLAGS_DESERIALIZE_ITEM( ParserWaitForTokenFlags::stopOnMultiLineComment  , "stop_on_multi_line_comment"  );
    MARTY_CPP_ENUM_FLAGS_DESERIALIZE_ITEM( ParserWaitForTokenFlags::stopOnMultiLineComment  , "stoponmultilinecomment"      );
    MARTY_CPP_ENUM_FLAGS_DESERIALIZE_ITEM( ParserWaitForTokenFlags::stopOnComment           , "stop-on-comment"             );
    MARTY_CPP_ENUM_FLAGS_DESERIALIZE_ITEM( ParserWaitForTokenFlags::stopOnComment           , "stop_on_comment"             );
    MARTY_CPP_ENUM_FLAGS_DESERIALIZE_ITEM( ParserWaitForTokenFlags::stopOnComment           , "stoponcomment"               );
    MARTY_CPP_ENUM_FLAGS_DESERIALIZE_ITEM( ParserWaitForTokenFlags::stopOnSpace             , "stop-on-space"               );
    MARTY_CPP_ENUM_FLAGS_DESERIALIZE_ITEM( ParserWaitForTokenFlags::stopOnSpace             , "stop_on_space"               );
    MARTY_CPP_ENUM_FLAGS_DESERIALIZE_ITEM( ParserWaitForTokenFlags::stopOnSpace             , "stoponspace"                 );
    MARTY_CPP_ENUM_FLAGS_DESERIALIZE_ITEM( ParserWaitForTokenFlags::stopOnAny               , "stop-on-any"                 );
    MARTY_CPP_ENUM_FLAGS_DESERIALIZE_ITEM( ParserWaitForTokenFlags::stopOnAny               , "stop_on_any"                 );
    MARTY_CPP_ENUM_FLAGS_DESERIALIZE_ITEM( ParserWaitForTokenFlags::stopOnAny               , "stoponany"                   );
    MARTY_CPP_ENUM_FLAGS_DESERIALIZE_ITEM( ParserWaitForTokenFlags::stopOnLinefeed          , "stop-on-linefeed"            );
    MARTY_CPP_ENUM_FLAGS_DESERIALIZE_ITEM( ParserWaitForTokenFlags::stopOnLinefeed          , "stop_on_linefeed"            );
    MARTY_CPP_ENUM_FLAGS_DESERIALIZE_ITEM( ParserWaitForTokenFlags::stopOnLinefeed          , "stoponlinefeed"              );
    MARTY_CPP_ENUM_FLAGS_DESERIALIZE_ITEM( ParserWaitForTokenFlags::stopOnAnySpace          , "stop-on-any-space"           );
    MARTY_CPP_ENUM_FLAGS_DESERIALIZE_ITEM( ParserWaitForTokenFlags::stopOnAnySpace          , "stop_on_any_space"           );
    MARTY_CPP_ENUM_FLAGS_DESERIALIZE_ITEM( ParserWaitForTokenFlags::stopOnAnySpace          , "stoponanyspace"              );
MARTY_CPP_ENUM_FLAGS_DESERIALIZE_END( ParserWaitForTokenFlags, std::map, 1 )

} // namespace tokenizer
} // namespace umba

