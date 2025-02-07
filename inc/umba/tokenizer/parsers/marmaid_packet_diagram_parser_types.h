/*! \file
    \brief Парсер marmaid packet diagram, расширенная версия, с использованием типов и массивов
 */


#pragma once

#include "umba/string_plus.h"
//

#include "umba/tokenizer.h"
//
#include "umba/tokenizer/tokenizer_log.h"
#include "umba/tokenizer/tokenizer_log_console.h"
#include "umba/tokenizer/token_collection.h"
//
#include "parser_base.h"
#include "umba/tokenizer/lang/marmaid_packet_diagram.h"

//
#include "umba/enum_helpers.h"
#include "umba/flag_helpers.h"

//
#include <stdexcept>
#include <initializer_list>


//----------------------------------------------------------------------------
// umba::tokenizer::marmaid::
namespace umba {
namespace tokenizer {
namespace marmaid {

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
enum class EPacketDiagramType
{
    unknown,
    bitDiagram ,
    byteDiagram,
    memDiagram     // Memory diagram/layout
};

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
enum class EPacketDiagramRangeType
{
    singleValue,
    range      ,
    explicitType
};

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
enum class Endianness
{
    unknown,
    littleEndian,
    bigEndian,
    middleEndian,
    leMiddleEndian,
    beMiddleEndian
};

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
enum class DiagramParsingOptions : std::uint32_t
{
    none                          = 0x00,
    allowOverrideTitle            = 0x01,
    allowOverrideType             = 0x02, // diagramType
    allowOverrideEndianness       = 0x04,
    allowOverrideBitSize          = 0x08,
    allowOverrideDisplayWidth     = 0x10,
    //allowMultiHex   = 0x04

    all = allowOverrideTitle | allowOverrideType | allowOverrideEndianness | allowOverrideBitSize | allowOverrideDisplayWidth

}; // enum 

UMBA_ENUM_CLASS_IMPLEMENT_FLAG_OPERATORS(DiagramParsingOptions)

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
// crc 0-20 seed 0 poly 0x1234
struct CrcOptions
{
    std::size_t     cellIndex  = 0; // Индекс в массиве data, по которому расположена информация о ячейке
    std::uint64_t   crcSize    = 0; // 1/2/4 - высчитывается от размера элемента, в который помещается, если размер не 1/2/4 - это ошибка
    std::uint64_t   seed       = 0;
    std::uint64_t   poly       = 0;
    std::uint64_t   start      = 0; // Индексы начального и 
    std::uint64_t   end        = 0; // конечного байт данных, сам CRC не должен туда входить
};




//----------------------------------------------------------------------------
inline
std::string makeCppNameFromText(const std::string &text, bool bCpp)
{
    UMBA_USED(bCpp);

    std::string res = text;
    for(auto &ch : res)
    {
        if (ch>='A' && ch<='Z')
            continue;
        if (ch>='a' && ch<='z')
            continue;
        if (ch>='0' && ch<='9')
            continue;

        ch = '_';
    }

    return res;
}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
template<typename TokenCollectionItemType>
struct PacketDiagramItem
{
    EPacketDiagramRangeType         rangeType;
    umba::tokenizer::payload_type   explicitTypeTokenId;
    std::string                     text;

    std::uint64_t                   start; // Вычисляем при добавлении, если указан тип
    std::uint64_t                   end  ; // Вычисляем при добавлении, если указан тип; ходит в диапазон
    std::uint64_t                   arraySize = std::uint64_t(-1);

    std::string                     realTypeName; // For C++ output, full qualified

    const TokenCollectionItemType   *pTokenInfo = 0; // стартовый токен, можно получить номер строки

    // Только размер типа
    std::uint64_t getTypeSize() const
    {
        UMBA_ASSERT(rangeType==EPacketDiagramRangeType::explicitType);
        return std::size_t(explicitTypeTokenId&0x0F);
    }

    // Размер типа * кол-во элеметов
    std::uint64_t getTypeFieldSize() const
    {
        if (arraySize==std::uint64_t(-1))
            return getTypeSize();
        return arraySize*getTypeSize();
    }

    bool isArray() const
    {
        if (rangeType==EPacketDiagramRangeType::explicitType)
        {
            if (arraySize==std::uint64_t(-1))
                return false;
            else
                return true;
        }

        if (rangeType==EPacketDiagramRangeType::singleValue)
            return false;

        return true;
    }

    std::uint64_t getArraySize() const
    {
        if (rangeType==EPacketDiagramRangeType::explicitType)
        {
            if (arraySize==std::uint64_t(-1))
                return 0;
            else
                return arraySize;
        }

        if (rangeType==EPacketDiagramRangeType::singleValue)
            return 0;

        return end-start+1;
    }

    std::string getPlainTypeName() const // For plain C
    {
        if (!realTypeName.empty())
            return realTypeName;

        if (rangeType!=EPacketDiagramRangeType::explicitType)
            return "uint8_t"; // threat singles and ranges as bytes

        switch(explicitTypeTokenId)
        {
            case MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_CHAR  : return "char"   ;
            case MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_INT8  : return "int8_t" ;
            case MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_INT16 : return "int16_t";
            case MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_INT32 : return "int32_t";
            case MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_INT64 : return "int64_t";
            case MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_UINT8 : return "uint8_t" ;
            case MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_UINT16: return "uint16_t";
            case MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_UINT32: return "uint32_t";
            case MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_UINT64: return "uint64_t";

            default: return "unknown";
        }

    }

    std::string getCppTypeName() const // For C++
    {
        std::string name = getPlainTypeName();
        if (!realTypeName.empty())
            return name;

        if (rangeType!=EPacketDiagramRangeType::explicitType)
            return "std::" + name;

        if (explicitTypeTokenId!=MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_CHAR)
            return "std::" + name;

        return name;
    }

    std::string getCppOrCTypeName(bool bCpp) const
    {
        return bCpp ? getCppTypeName() : getPlainTypeName();
    }

    std::string getCppOrCFieldName(bool bCpp) const
    {
        return makeCppNameFromText(text, bCpp);
    }

    

}; // struct PacketDiagramItem

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
template<typename TokenCollectionItemType>
struct PacketDiagram
{
    using PacketDiagramItemType = PacketDiagramItem<TokenCollectionItemType>;

    EPacketDiagramType                     diagramType = EPacketDiagramType::unknown;
    std::string                            title  ;
    std::vector<PacketDiagramItemType>     data   ;
    std::vector<CrcOptions>                crcList;
    
    DiagramParsingOptions                  parsingOptions = DiagramParsingOptions::all;

    Endianness                             endianness   = Endianness::littleEndian;
    std::uint64_t                          bitSize      = 32;
    std::uint64_t                          displayWidth = 32;

    std::string getCppOrCTitle(bool bCpp) const
    {
        return title.empty() ? std::string("untitled") : makeCppNameFromText(title, bCpp);
    }

}; // struct PacketDiagram

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------

} // namespace marmaid
} // namespace tokenizer
} // namespace umba
// umba::tokenizer::marmaid::

