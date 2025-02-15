/*! \file
    \brief Парсер marmaid packet diagram, расширенная версия, с использованием типов и массивов
 */


#pragma once

#include "umba/string.h"
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
#include "umba/rule_of_five.h"

//
#include <string>
#include <stdexcept>
#include <initializer_list>
#include <unordered_map>
#include <climits>
#include <algorithm>
#include <utility>

//----------------------------------------------------------------------------
// umba::tokenizer::marmaid::
namespace umba {
namespace tokenizer {
namespace marmaid {

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

}; // enum class DiagramParsingOptions : std::uint32_t

UMBA_ENUM_CLASS_IMPLEMENT_FLAG_OPERATORS(DiagramParsingOptions)

//----------------------------------------------------------------------------
enum class EPacketDiagramType
{
    unknown, invalid = unknown, undefined = unknown,
    bitDiagram, byteDiagram, memDiagram     // Memory diagram/layout
};

//----------------------------------------------------------------------------
enum class EPacketDiagramItemType
{
    unknown, invalid = unknown, undefined = unknown,
    singleValue, range, explicitType, org
};

//----------------------------------------------------------------------------
enum class EOrgType
{
    unknown, invalid = unknown, undefined = unknown,
    orgAuto, orgAbs, orgRel
};

//----------------------------------------------------------------------------
enum class Endianness
{
    unknown, invalid = unknown, undefined = unknown,
    littleEndian, bigEndian, middleEndian, leMiddleEndian, beMiddleEndian
};

//----------------------------------------------------------------------------
enum class AddressRangeKind
{
    unknown, invalid = unknown, undefined = unknown,
    localIndexes, // индексы внутри ORG
    globalLabels  // по меткам/именам глобально

};

//----------------------------------------------------------------------------
struct AddressRange
{
    std::uint64_t  start; // Это индексы внутри одного org
    std::uint64_t  end  ; // Тоже входит в диапазон
};

//----------------------------------------------------------------------------
struct AddressRangeLabels
{
    std::string  start;
    std::string  end  ; // Тоже входит в диапазон
};

//----------------------------------------------------------------------------
enum class ChecksumKind
{
    unknown, invalid = unknown, undefined = unknown,
    crc, simpleSum, simpleSumComplement, simpleSumInvert, simpleXor, simpleXorComplement, simpleXorInvert
};

//----------------------------------------------------------------------------
struct CrcConfig
{
    std::uint64_t   size    = 0; // 1/2/4
    std::uint64_t   seed    = 0;
    std::uint64_t   poly    = 0;
};

//----------------------------------------------------------------------------

// crc 0-20 seed 0 poly 0x1234
struct ChecksumOptions
{
    ChecksumKind         kind = ChecksumKind::undefined;
    // std::size_t          cellIndex  = std::size_t(-1); // Индекс в массиве data, по которому расположена информация о ячейке
    CrcConfig            crcConfig    ; // Размер checksum (size) высчитывается от размера элемента, в который помещается, если размер не 1/2/4 - это ошибка

    AddressRangeKind     addressRangeKind = AddressRangeKind::undefined;
    AddressRange         addressRange ; // Индексы начального и конечного байт данных, сам CRC не должен туда входить. Индексы внутри одного org
    AddressRangeLabels   addressRangeLabels; // Названия полей. Могут относится к разным org, при условии, что между ними нет gaps

}; // struct ChecksumOptions




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

inline
std::string makeIdFromText(const std::string &text)
{
    auto res = umba::string::tolower_copy(makeCppNameFromText(text, false));
    if (res.empty())
        return res;
    
    auto ch1 = res.front();
    for(auto ch: res)
    {
        if (ch!=ch1)
            return res;
    }
    
    return std::string(1, ch1);
}



//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
template<typename TokenCollectionItemType>
struct PacketDiagramItem
{
    EPacketDiagramItemType          itemType = EPacketDiagramItemType::undefined;
    EOrgType                        orgType  = EOrgType::undefined;
    umba::tokenizer::payload_type   explicitTypeTokenId;
    std::string                     text;
    bool                            fillEntry     = false;
    std::string                     reusedOrg;
    bool                            emptyOrg      = false; // no data records after this
    bool                            textGenerated = false; // text is auto-generated
    bool                            asciiZet      = false; // can be used only with ranges or byte/char arrays

    Endianness                      endianness   = Endianness::undefined; // use project endianness or override endianness for this entry
    AddressRange                    addressRange; // Если указан тип - вычисляем при добавлении, 
    std::uint64_t                   orgAddress   = 0;
    std::uint64_t                   arraySize    = std::uint64_t(-1); // работает только для явно заданного типа

    std::string                     realTypeName; // For C++ output, full qualified

    std::size_t                     checksumIndex = std::size_t(-1); // При разборе строки записи, если там нет опции CRC, то выходной параметр 
                                                                // ChecksumOptions не заполнен и не валиден, это поле содержит -1.
                                                                // Если опции CRC корректно заданы, то это поле устанавливается в 0,
                                                                // и парсер должен добавить опции CRC в отдельный массив, а сюда поместить
                                                                // реальный индекс.

    const TokenCollectionItemType   *pTokenInfo = 0; // стартовый токен, можно получить номер строки


    bool isDataEntry() const
    {
        return itemType==EPacketDiagramItemType::explicitType
            || itemType==EPacketDiagramItemType::singleValue
            || itemType==EPacketDiagramItemType::range
             ;
    }

    // Только размер типа
    std::uint64_t getTypeSize() const
    {
        if (itemType==EPacketDiagramItemType::explicitType)
            return std::size_t(explicitTypeTokenId&0x0F);
        else if (itemType==EPacketDiagramItemType::singleValue || itemType==EPacketDiagramItemType::range)
            return 1;
        UMBA_ASSERT_FAIL_MSG("Not a data entry");
        return 0;
    }

    bool isArray() const
    {
        if (itemType==EPacketDiagramItemType::explicitType)
            return (arraySize==std::uint64_t(-1)) ? false : true;

        else if (itemType==EPacketDiagramItemType::singleValue)
            return false;

        else if (itemType==EPacketDiagramItemType::range)
            return true;

        UMBA_ASSERT_FAIL_MSG("Not a data entry");
        return false;
    }

    bool isAsciiZet() const
    {
        return asciiZet;
    }

    std::uint64_t getArraySize() const
    {
        if (itemType==EPacketDiagramItemType::explicitType)
            return (arraySize==std::uint64_t(-1)) ? std::uint64_t(1) : arraySize;

        else if (itemType==EPacketDiagramItemType::singleValue)
            return 1;

        else if (itemType==EPacketDiagramItemType::range)
            return addressRange.end - addressRange.start+1;

        UMBA_ASSERT_FAIL_MSG("Not a data entry");
        return 0;
    }

    // Размер типа * кол-во элеметов
    std::uint64_t getTypeFieldSize() const
    {
        return getArraySize()*getTypeSize();
    }


    std::string getPlainTypeName() const // For plain C
    {
        if (!realTypeName.empty())
            return realTypeName;

        if (itemType!=EPacketDiagramItemType::explicitType)
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

        if (itemType!=EPacketDiagramItemType::explicitType)
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

    static std::string makeFieldId(const std::string &t)
    {
        return makeIdFromText(t);
    }

    std::string getFieldId() const
    {
        return makeFieldId(text);
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
    std::vector<ChecksumOptions>           checksumList;
    
    DiagramParsingOptions                  parsingOptions = DiagramParsingOptions::all;

    Endianness                             endianness   = Endianness::unknown;
    std::uint64_t                          bitSize      = 32;
    std::uint64_t                          displayWidth = 32;
    std::uint64_t                          lastOrg      = 0 ;
    std::uint64_t                          orgAddress   = std::uint64_t(-1);


    std::unordered_map<std::string, std::size_t>     entryNames;  // храним индекс в векторе data
    std::unordered_map<std::string, std::size_t>     orgNames  ;  // храним индекс в векторе data
    std::size_t                                      fillEntryCounter = 0;


    static std::string makeFieldId(const std::string &t)
    {
        return makeIdFromText(t);
    }

    std::string getCppOrCTitle(bool bCpp) const
    {
        return title.empty() ? std::string("untitled") : makeCppNameFromText(title, bCpp);
    }

    bool isDataEmpty() const
    {
        if (data.empty())
            return true;

        if (data.size()==1u && data.back().itemType==EPacketDiagramItemType::org)
            return true; // Одна запись, и она содержит задание адреса

        return false;
    }

    // Вычисляет базовый адрес для записи с индексом entryIdx
    std::uint64_t calcBaseAddress(std::size_t entryIdx=std::size_t(-1)) const
    {
        if (entryIdx>data.size())
            entryIdx = data.size();

        std::uint64_t addr = 0;

        for(std::size_t i=0; i!=entryIdx; ++i)
        {
            if (data[i].itemType!=EPacketDiagramItemType::org)
                continue;

            addr = data[i].orgAddress; // item.addressRange.start; // адрес в записи типа org всегда абсолютный, auto и rel обсчитываем при добавлении
        }

        return addr;
    }

    std::string generateOrgName(std::size_t &orgCounter) const
    {
        return getCppOrCTitle(false) + "_Org" + std::to_string(orgCounter++);
    }

    void rebuildNameMaps()
    {
        entryNames.clear();
        orgNames  .clear();

        for(std::size_t i=0; i!=data.size(); ++i)
        {
            const auto & curData = data[i];

            std::unordered_map<std::string, std::size_t> &m = (curData.itemType==EPacketDiagramItemType::org) ? orgNames : entryNames;
            m[curData.getFieldId()] = i;
        }
    }

    void rebuildChecksumList()
    {
        std::vector<ChecksumOptions> newChecksumList;
        for(std::size_t i=0; i!=data.size(); ++i)
        {
            auto & curData = data[i];
            if (curData.checksumIndex!=std::size_t(-1))
            {
                std::size_t newChecksumIndex = newChecksumList.size();
                newChecksumList.emplace_back(checksumList[curData.checksumIndex]);
                curData.checksumIndex = newChecksumIndex;
            }
        }

        swap(checksumList, newChecksumList);
    }

    std::vector<PacketDiagram> splitToSimple() const
    {
        std::vector<PacketDiagram> resVec;

        PacketDiagram cur = *this;
        cur.data.clear();

        for(std::size_t i=0; i!=data.size(); ++i)
        {
            const auto & curData = data[i];

            if (curData.itemType!=EPacketDiagramItemType::org)
            {
                // Обычная запись, просто добавляем
                // if (!curData.fillEntry)
                    cur.data.emplace_back(curData);
                continue;
            }

            if (cur.data.size()>1) // Есть записи помимо org
            {
                resVec.emplace_back(cur);
                cur.data.clear();
                cur.orgAddress = curData.orgAddress;
                cur.title      = curData.text;
                cur.data.emplace_back(curData);
                continue;
            }

            if ( cur.data.size()==1)
            {
                if (cur.data.back().itemType==EPacketDiagramItemType::org) // предыдущая и единственная запись - тоже org
                {
                    if (cur.data.back().orgAddress==curData.orgAddress) // Адреса одинаковые?
                    {
                        if (!curData.textGenerated) // текущий элемент - не сгенерённый, заменяем на него, не важно, что там лежало - сгенеренное или нет
                        {
                             cur.data.back() = curData;
                        }
                        // Иначе ничего не делаем
                    }
                    else
                    {
                        // Адреса разные, перетираем безусловно
                        cur.data.back() = curData;
                    }

                    continue;
                }
            }

            if (!cur.data.empty()) // Есть записи помимо org
            {
                resVec.emplace_back(cur);
            }
            cur.data.clear();
            cur.orgAddress = curData.orgAddress;
            cur.title      = curData.text;
            cur.data.emplace_back(curData);
        }

        if (cur.data.size()>1) // Есть записи помимо org
        {
            resVec.emplace_back(cur);
        }

        for(auto &rv : resVec)
        {
            rv.rebuildNameMaps();
            rv.checksumList = checksumList; // Тупо копируем всё
            rv.rebuildChecksumList();
        }

        return resVec;
    }


    void removeEmptyOrgs()
    {
        {
            std::vector<ChecksumOptions> newChecksumList;
    
            auto simpleVec = splitToSimple();
            data.clear();
            for(const auto &smp : simpleVec)
            {
                auto newItemsIt = data.insert(data.end(), smp.data.begin(), smp.data.end());
                for(; newItemsIt!=data.end(); ++newItemsIt)
                {
                    auto & curData = *newItemsIt;
                    if (curData.checksumIndex!=std::size_t(-1))
                    {
                        std::size_t newChecksumIndex = newChecksumList.size();
                        newChecksumList.emplace_back(smp.checksumList[curData.checksumIndex]);
                        curData.checksumIndex = newChecksumIndex;
                    }
    
                }
            }
    
            swap(checksumList, newChecksumList);
        }

        rebuildNameMaps();

        for(std::size_t i=0; i!=data.size(); ++i)
        {
            if (data[i].itemType==EPacketDiagramItemType::org && i!=0)
            {
                if (data[i-1].itemType==EPacketDiagramItemType::org)
                    data[i-1].emptyOrg = true;
            }
        }

        if (!data.empty() && data.back().itemType==EPacketDiagramItemType::org)
            data.back().emptyOrg = true;
    }


}; // struct PacketDiagram

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
// umba::tokenizer::marmaid::utils::
namespace utils {

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
using unordered_memory_t = std::unordered_map<std::uint64_t, std::uint8_t>;


#if defined(DEBUG) || defined(_DEBUG)

    using byte_vector_t = std::vector<std::uint8_t>;

#else

    using byte_vector_t = std::basic_string<std::uint8_t>; // std::basic_string uses small string optimization and faster on short strings

#endif

//----------------------------------------------------------------------------
inline
std::uint64_t makeByteSizeMask(std::size_t n)
{
    switch(n)
    {
        case 1 : return std::uint64_t(0x00000000000000FFull);
        case 2 : return std::uint64_t(0x000000000000FFFFull);
        case 3 : return std::uint64_t(0x0000000000FFFFFFull);
        case 4 : return std::uint64_t(0x00000000FFFFFFFFull);
        case 5 : return std::uint64_t(0x000000FFFFFFFFFFull);
        case 6 : return std::uint64_t(0x0000FFFFFFFFFFFFull);
        case 7 : return std::uint64_t(0x00FFFFFFFFFFFFFFull);
        default: return std::uint64_t(0xFFFFFFFFFFFFFFFFull);
    }
}

/*
CHAR_BIT       = 8 (0x0000000000000008)
MB_LEN_MAX     = 5 (0x0000000000000005)

CHAR_MIN       = -128 (0xFFFFFFFFFFFFFF80)
CHAR_MAX       = 127 (0x000000000000007F)

SCHAR_MIN      = -128 (0xFFFFFFFFFFFFFF80)
SHRT_MIN       = -32768 (0xFFFFFFFFFFFF8000)
INT_MIN        = -2147483648 (0xFFFFFFFF80000000)
LONG_MIN       = -2147483648 (0xFFFFFFFF80000000)
LLONG_MIN      = -9223372036854775808 (0x8000000000000000)
SCHAR_MAX      = 127 (0x000000000000007F)
SHRT_MAX       = 32767 (0x0000000000007FFF)
INT_MAX        = 2147483647 (0x000000007FFFFFFF)
LONG_MAX       = 2147483647 (0x000000007FFFFFFF)
LLONG_MAX      = 9223372036854775807 (0x7FFFFFFFFFFFFFFF)

UCHAR_MAX      = 255 (0x00000000000000FF)
USHRT_MAX      = 65535 (0x000000000000FFFF)
UINT_MAX       = 4294967295 (0x00000000FFFFFFFF)
ULONG_MAX      = 4294967295 (0x00000000FFFFFFFF)
ULLONG_MAX     = 18446744073709551615 (0xFFFFFFFFFFFFFFFF)
PTRDIFF_MIN    = -9223372036854775808 (0x8000000000000000)
PTRDIFF_MAX    = 9223372036854775807 (0x7FFFFFFFFFFFFFFF)
SIZE_MAX       = 18446744073709551615 (0xFFFFFFFFFFFFFFFF)
SIG_ATOMIC_MIN = -2147483648 (0xFFFFFFFF80000000)
SIG_ATOMIC_MAX = 2147483647 (0x000000007FFFFFFF)
WCHAR_MIN      = 0 (0x0000000000000000)
WCHAR_MAX      = 65535 (0x000000000000FFFF)
WINT_MIN       = 0 (0x0000000000000000)
WINT_MAX       = 65535 (0x000000000000FFFF)
*/    


//----------------------------------------------------------------------------
inline
std::pair<std::int64_t, std::int64_t> getMinMaxValuesSigned(std::uint64_t size)
{
    using namespace std;

    switch(size)
    {
        case 1 : return make_pair(int64_t(numeric_limits<int8_t >::min()), int64_t(numeric_limits<int8_t >::max()));

        case 2 : return make_pair(int64_t(numeric_limits<int16_t>::min()), int64_t(numeric_limits<int16_t>::max()));

        case 3 : 
               {
                   auto mmp = getMinMaxValuesSigned(2);
                   return make_pair(int64_t(mmp.first<<8), int64_t((mmp.second<<8)|0xFF));
               }
                 
        case 4 : return make_pair(int64_t(numeric_limits<int32_t>::min()), int64_t(numeric_limits<int32_t>::max()));

        case 5 :
               {
                   auto mmp = getMinMaxValuesSigned(4);
                   return make_pair(int64_t(mmp.first<<8), int64_t((mmp.second<<8)|0xFF));
               }
                 
        case 6 :
               {
                   auto mmp = getMinMaxValuesSigned(4);
                   return make_pair(int64_t(mmp.first<<16), int64_t((mmp.second<<16)|0xFFFF));
               }

        case 7 :
               {
                   auto mmp = getMinMaxValuesSigned(4);
                   return make_pair(int64_t(mmp.first<<24), int64_t((mmp.second<<24)|0xFFFFFF));
               }

        default: return make_pair(int64_t(numeric_limits<int64_t>::min()), int64_t(numeric_limits<int64_t>::max()));
    }
}

//----------------------------------------------------------------------------
inline
std::pair<std::uint64_t, std::uint64_t> getMinMaxValuesUnsigned(std::uint64_t size)
{
    using namespace std;

    switch(size)
    {
        case 1 : return make_pair(uint64_t(numeric_limits<uint8_t >::min()), uint64_t(numeric_limits<uint8_t >::max()));

        case 2 : return make_pair(uint64_t(numeric_limits<uint16_t>::min()), uint64_t(numeric_limits<uint16_t>::max()));

        case 3 : 
               {
                   auto mmp = getMinMaxValuesUnsigned(2);
                   return make_pair(uint64_t(mmp.first<<8), uint64_t((mmp.second<<8)|0xFF));
               }
                 
        case 4 : return make_pair(uint64_t(numeric_limits<uint32_t>::min()), uint64_t(numeric_limits<uint32_t>::max()));

        case 5 :
               {
                   auto mmp = getMinMaxValuesUnsigned(4);
                   return make_pair(uint64_t(mmp.first<<8), uint64_t((mmp.second<<8)|0xFF));
               }
                 
        case 6 :
               {
                   auto mmp = getMinMaxValuesUnsigned(4);
                   return make_pair(uint64_t(mmp.first<<16), uint64_t((mmp.second<<16)|0xFFFF));
               }

        case 7 :
               {
                   auto mmp = getMinMaxValuesUnsigned(4);
                   return make_pair(uint64_t(mmp.first<<24), uint64_t((mmp.second<<24)|0xFFFFFF));
               }

        default: return make_pair(uint64_t(numeric_limits<uint64_t>::min()), uint64_t(numeric_limits<uint64_t>::max()));
    }
}

//----------------------------------------------------------------------------
inline
std::uint64_t getLoHalf(std::uint64_t val, std::uint64_t size)
{
    return val & makeByteSizeMask(size/2);
}

//----------------------------------------------------------------------------
inline
std::uint64_t getHiHalf(std::uint64_t val, std::uint64_t size)
{
    return (val>>((size/2))*8) & makeByteSizeMask(size/2);
}

//----------------------------------------------------------------------------
inline
void makeByteVector(std::uint64_t val, std::uint64_t size, Endianness endianness, byte_vector_t &resVec)
{
    if (endianness==Endianness::undefined)
        endianness = Endianness::littleEndian;

    std::size_t resVecOrgSize = resVec.size();

    if (size>8)
        throw std::invalid_argument("size too much (greater than 8)");

    std::uint64_t orgVal = val;

    // Допустимы ли только степени двойки (1/2/4/8) или можно использовать и 3/5/7?
    // в режиме leMiddleEndian и beMiddleEndian - точно нельзя, только 4 или 8

    switch(endianness)
    {
        case Endianness::undefined     : throw std::runtime_error("invalid endianness (undefined)");
        case Endianness::middleEndian  : throw std::runtime_error("invalid endianness (middleEndian)");

        case Endianness::littleEndian  : [[fallthrough]];
        case Endianness::bigEndian     :
             {
                 for(std::size_t i=0; i!=size; ++i, val>>=8)
                     resVec.push_back(std::uint8_t(val));

                 if (val)
                     throw std::out_of_range("value can't fit into " + std::to_string(size) + " bytes: " + std::to_string(orgVal));
             }
             break;

        case Endianness::leMiddleEndian:
             if (size!=4 && size!=8)
                 throw std::invalid_argument("middle-endian values can be only 4 or 8 bytes len");
             // половинки идут в littleEndian формате, но старшая половинка - первая
             makeByteVector(getHiHalf(val, size), size/2, Endianness::littleEndian, resVec);
             makeByteVector(getLoHalf(val, size), size/2, Endianness::littleEndian, resVec);
             return;

        case Endianness::beMiddleEndian:
             if (size!=4 && size!=8)
                 throw std::invalid_argument("middle-endian values can be only 4 or 8 bytes len");
             // половинки идут в bigEndian формате, но младшая половинка - первая
             makeByteVector(getLoHalf(val, size), size/2, Endianness::bigEndian, resVec);
             makeByteVector(getHiHalf(val, size), size/2, Endianness::bigEndian, resVec);
             return;
    }

    if (endianness==Endianness::bigEndian)
    {
        std::reverse(resVec.begin()+std::ptrdiff_t(resVecOrgSize), resVec.end()); // меняем только то, что добавили сами
    }

}

//----------------------------------------------------------------------------
inline
byte_vector_t makeByteVector(std::uint64_t val, std::uint64_t size, Endianness endianness)
{
    byte_vector_t resVec;
    makeByteVector(val, size, endianness, resVec);
    return resVec;
}

//----------------------------------------------------------------------------
inline
char digitToChar(int d, bool bLower=false)
{
    d &= 0xF;
    // if (d<0)  return '-';
    if (d<10) return char('0'+d);
    return char((bLower?'a':'A')+d-10);
}

//----------------------------------------------------------------------------
inline
int charToDigit(char ch)
{
    if (ch>='0' && ch<='9')
        return ch-'0';

    if (ch>='A' && ch<='F')
        return ch-'A'+10;

    if (ch>='a' && ch<='f')
        return ch-'a'+10;

    return -1;
}

//----------------------------------------------------------------------------
inline
std::string makeByteVectorDump(const byte_vector_t &v)
{
    std::string resStr; resStr.reserve(v.size()*3);

    for(auto b : v)
    {
        resStr.append(1, digitToChar((int)unsigned(b>>4 )));
        resStr.append(1, digitToChar((int)unsigned(b&0xF)));
        resStr.append(1, ' ');
    }

    resStr.pop_back(); // remove last space
    return resStr;
}

//----------------------------------------------------------------------------
inline
std::string makeHexString(std::uint64_t val, std::uint64_t size)
{
    std::string resStr; resStr.reserve(16);
    if (size>8)
        size = 8;
    if (size<1)
        size = 1;

    for(auto i=0; i!=16; ++i, val>>=4)
    {
        resStr.append(1, digitToChar((int)unsigned(val&0xF)));
    }

    // Старшие - у нас в конце
    // auto nCut = 16u-std::size_t(size*2);

    resStr.erase(size*2);
    std::reverse(resStr.begin(), resStr.end());

    return resStr;

}

//----------------------------------------------------------------------------
//! returns 0 if ok, -1 if wrong char detected and 1 if number of hex digits is wrong
inline
int isStringDumpString(const std::string &str, byte_vector_t *pResVec=0)
{
    std::uint8_t curByte = 0;
    std::size_t nDigitsProcessed = 0;

    for(auto ch: str)
    {
        if (ch==' ')
            continue; // Пробелы игнорируем

        int d = charToDigit(ch);
        if (d<0)
            return -1;

        curByte <<= 4;
        curByte |= std::uint8_t(d);

        ++nDigitsProcessed;
        if ((nDigitsProcessed&1)==0)
        {
            if (pResVec)
                pResVec->push_back(curByte);
            curByte = 0;
        }
    }

    return ((nDigitsProcessed&1)==0) ? 0 : 1; // если нечетное количество цифр - дамп некорректный
}

//----------------------------------------------------------------------------
inline
void checkThrowValueFit(std::int64_t val, std::uint64_t size)
{
    auto minMax = getMinMaxValuesSigned(size);
    if (!(val>=minMax.first && val<=minMax.second))
        throw std::out_of_range("value can't fit into " + std::to_string(size) + " bytes: " + std::to_string(val) + " (0x" + makeHexString(std::uint64_t(val), 8) + ")");
}

inline
void checkThrowValueFit(std::uint64_t val, std::uint64_t size)
{
    auto minMax = getMinMaxValuesUnsigned(size);
    if (!(val>=minMax.first && val<=minMax.second))
        throw std::out_of_range("value can't fit into " + std::to_string(size) + " bytes: " + std::to_string(val) + " (0x" + makeHexString(std::uint64_t(val), 8) + ")");
}

//----------------------------------------------------------------------------
//! Строка val - число, разбирается как знаковое или беззнаковое, потом проверяется, влезает ли число в диапазон, задаваемый size
inline
byte_vector_t makeByteVectorFromIntStr(const std::string &valStr, std::uint64_t size, Endianness endianness, bool bSigned, std::uint64_t *pParsedVal=0)
{
    std::uint64_t uval = 0;
 
    if (bSigned)
    {
        std::int64_t ival = stoll(valStr); // кидает std::invalid_argument или std::out_of_range
        checkThrowValueFit(ival, size);
        // auto minMax = getMinMaxValuesSigned(size);
        // if (!(ival>=minMax.first && ival<=minMax.second))
        //     throw std::out_of_range("value can't fit into " + std::to_string(size) + " bytes: " + std::to_string(ival) + " (0x" + makeHexString(std::uint64_t(ival), 8) + ")");
        uval = std::uint64_t(ival);
    }
    else
    {
        uval = stoull(valStr); // кидает std::invalid_argument или std::out_of_range
        checkThrowValueFit(uval, size);
        // auto minMax = getMinMaxValuesUnsigned(size);
        // if (!(uval>=minMax.first && uval<=minMax.second))
        //     throw std::out_of_range("value can't fit into " + std::to_string(size) + " bytes: " + std::to_string(uval) + " (0x" + makeHexString(std::uint64_t(uval), 8) + ")");
    }

    if (pParsedVal)
        *pParsedVal = uval;

    return makeByteVector(uval, size, endianness);
}

//----------------------------------------------------------------------------
//! Строка val - последовательность символов ASCII, первые символы попадают в старшие разряды, потом проверяется, влезает ли число в диапазон, задаваемый size
inline
byte_vector_t makeByteVectorFromCharLiteral(const std::string &valStr, std::uint64_t size, Endianness endianness, std::uint64_t *pParsedVal=0)
{
    std::uint64_t val = 0;
    for(auto ch: valStr)
    {
        val <<= 8;
        val |= std::uint64_t(std::uint8_t(ch));
    }
 
    checkThrowValueFit(val, size);
    // auto minMax = getMinMaxValuesUnsigned(size);
    // if (!(val>=minMax.first && val<=minMax.second))
    //     throw std::out_of_range("value can't fit into " + std::to_string(size) + " bytes: " + std::to_string(val) + " (0x" + makeHexString(std::uint64_t(val), 8) + ")");

    if (pParsedVal)
        *pParsedVal = val;

    return makeByteVector(val, size, endianness);
}

//----------------------------------------------------------------------------
//! Строка val - последовательность символов ASCII как строка, переводится в байты один в один, дополняется нулями до нужной длины, кидает std::out_of_range, если строка длиннее заданного количества байт
//! Может использоваться только для байтовых массивов или диапазонов
inline
byte_vector_t makeByteVectorFromStringLiteral(const std::string &valStr, std::uint64_t size, bool asciiZ=false)
{
    auto requiredSize = valStr.size(); // Сколько места требуется под строку
    if (asciiZ)
        ++requiredSize;

    if (requiredSize>size)
        throw std::out_of_range("string can't fit into " + std::to_string(size) + " bytes: required length is " + std::to_string(requiredSize));

    byte_vector_t resVec; resVec.reserve(size);
    for(auto ch : valStr)
        resVec.push_back(std::uint8_t(ch));

    while(resVec.size()<size)
        resVec.push_back(0);

    return resVec;
}

//----------------------------------------------------------------------------
//! Строка val - строка дампа, каждый байт представлен парой HEX-цифр, пробелы допустимы произвольно, не обязательно попарно
//! Может использоваться только для байтовых массивов или диапазонов
inline
byte_vector_t makeByteVectorFromDumpString(const std::string &strDump, std::uint64_t size, bool asciiZ=false)
{
    byte_vector_t bytes;
    int res = isStringDumpString(strDump, &bytes);

    if (res<0)
        throw std::invalid_argument("invalid char found in dump");
    else if (res<0)
        throw std::invalid_argument("wrong number of digits in dump");

    while(bytes.size()<size)
        bytes.push_back(0);

    // should end in zero.
    // must end in zero.
    // should ends in zero.
    // must ends in zero.
    // should end with zero.
    // must end with zero.
    // should ends with zero.
    // must ends with zero.


    if (asciiZ && bytes.back()!=0)
        throw std::invalid_argument("dump for Ascii-Z fields should ends with zero");

    if (bytes.size()>size)
        throw std::invalid_argument("dump too long: field size is " + std::to_string(size) + ", but taken dump size is " + std::to_string(bytes.size()));

    return bytes;
}

//----------------------------------------------------------------------------
inline
std::string endiannessToString(Endianness endianness)
{
    switch(endianness)
    {
        case Endianness::middleEndian  : return "middle-endian";
        case Endianness::littleEndian  : return "little-endian";
        case Endianness::bigEndian     : return "big-endian";
        case Endianness::leMiddleEndian: return "le-middle-endian";
        case Endianness::beMiddleEndian: return "be-middle-endian";
        case Endianness::invalid       : [[fallthrough]];
        default: return "undefined";
    }
}

//----------------------------------------------------------------------------
//! Отделяет индекс от имени поля. Простая реализация, без использования парсера. 0 - ошибка, 1 - есть индекс, -1 - индекса нет
inline
int simpleSplitNameAndIndex(const std::string &fullName, std::string *pName, std::uint64_t *pIndex)
{
    auto startPos = fullName.find('[');
    if (startPos==fullName.npos)
    {
        auto endPos = fullName.find(']');
        if (endPos!=fullName.npos)
            return 0; // ошибка

        if (pName)
           *pName = umba::string::trim_copy(fullName);

        return -1; // нет индекса
    }

    if (startPos==0)
        return 0; // ошибка

    auto endPos = fullName.find(']', startPos+1);
    if (endPos==fullName.npos)
        return 0; // ошибка

    std::uint64_t idx = 0;
    std::string strIdx = umba::string::trim_copy(std::string(fullName, startPos+1, endPos-startPos-1));
    if (strIdx.empty() || strIdx.front()=='-') // пустая строка и отрицательные числа недопустимы
        return 0; // ошибка

    try
    {
        idx = stoull(strIdx);
    }
    catch(...)
    {
        return 0; // ошибка
    }

    if (pName)
       *pName = umba::string::trim_copy(std::string(fullName, 0, startPos));

    if (pIndex)
       *pIndex = idx;

    return 1;
}



//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
//! Итератор по несортированной памяти. Кидает исключение при попытке чтения неинициализированной ранее ячейки.
class UnorderedMemoryIterator
{
    unordered_memory_t    *m_pMemory = 0;
    std::uint64_t          m_address = 0;

    struct Proxy
    {
        unordered_memory_t    *pMemory = 0;
        std::uint64_t          address = 0;

        UMBA_RULE_OF_FIVE_DEFAULT(Proxy);

        explicit Proxy(unordered_memory_t *pm, std::uint64_t addr) : pMemory(pm), address(addr) {}

        Proxy& operator=(std::uint8_t b)
        {
            (*pMemory)[address] = b;
            return *this;
        }

        operator std::uint8_t() const
        {
            unordered_memory_t::const_iterator it = pMemory->find(address);
            if (it!=pMemory->end())
                return it->second;
            throw std::out_of_range("memory not assigned at address: 0x" + makeHexString(address, 8)); // range_error
        }

    }; // struct Proxy


public:

    UMBA_RULE_OF_FIVE_DEFAULT(UnorderedMemoryIterator);

    explicit UnorderedMemoryIterator(unordered_memory_t &m , std::uint64_t addr) : m_pMemory(&m), m_address(addr) {}
    explicit UnorderedMemoryIterator(unordered_memory_t *pm, std::uint64_t addr) : m_pMemory(pm), m_address(addr) {}

    bool operator==(const UnorderedMemoryIterator &other) const
    {
        return m_pMemory==other.m_pMemory && m_address==other.m_address;
    }

    bool operator!=(const UnorderedMemoryIterator &other) const
    {
        return m_pMemory!=other.m_pMemory || m_address!=other.m_address;
    }

    operator std::uint64_t() const
    {
        return m_address;
    }

    UnorderedMemoryIterator& operator++() // pre
    {
        ++m_address;
        return *this;
    }

    UnorderedMemoryIterator operator++(int) // post
    {
        auto cp = *this;
        ++m_address;
        return cp;
    }

    Proxy operator*()
    {
        return Proxy(m_pMemory, m_address);
    }

    // UartOutputIterator& operator=(std::uint8_t b)
    // {
    //     // ignoring result
    //     pUart->Write((const uint8_t*)&b, 1); // std::size_t *pBytesWritten=0
    //     return *this;
    // }
    //  
    // UartOutputIterator& operator=(char ch)
    // {
    //     // ignoring result
    //     // Result_t Write(const uint8_t* buffer, std::size_t dataSize, std::size_t *pBytesWritten=0)
    //     pUart->Write((const uint8_t*)&ch, 1); // std::size_t *pBytesWritten=0
    //     return *this;
    // }

}; // class UnorderedMemoryIterator





// using unordered_memory_t = std::unordered_map<std::uint64_t, std::uint8_t>;
// using byte_vector_t = std::vector<std::uint8_t>;




//----------------------------------------------------------------------------
} // namespace utils
// umba::tokenizer::marmaid::utils::

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------

} // namespace marmaid
} // namespace tokenizer
} // namespace umba
// umba::tokenizer::marmaid::

