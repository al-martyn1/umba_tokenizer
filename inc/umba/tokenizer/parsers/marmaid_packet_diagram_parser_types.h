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

//
#include <string>
#include <stdexcept>
#include <initializer_list>
#include <unordered_map>
#include <climits>
#include <algorithm>

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
struct AddressRange
{
    std::uint64_t  start;
    std::uint64_t  end  ; // Тоже входит в диапазон
};

//----------------------------------------------------------------------------
struct CrcGeneratorConfig
{
    std::uint64_t   size    = 0; // 1/2/4
    std::uint64_t   seed    = 0;
    std::uint64_t   poly    = 0;
};

//----------------------------------------------------------------------------
// crc 0-20 seed 0 poly 0x1234
struct CrcOptions
{
    // std::size_t          cellIndex  = std::size_t(-1); // Индекс в массиве data, по которому расположена информация о ячейке
    CrcGeneratorConfig   crcConfig; // Размер crc (size) высчитывается от размера элемента, в который помещается, если размер не 1/2/4 - это ошибка
    AddressRange         addressRange; // Индексы начального и конечного байт данных, сам CRC не должен туда входить

}; // struct CrcOptions




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
    bool                            fillEntry = false;
    std::string                     reusedOrg;
    bool                            emptyOrg = false; // no data records after this
    bool                            textGenerated = false; // no data records after this

    Endianness                      endianness   = Endianness::undefined; // use project endianness or override endianness for this entry
    AddressRange                    addressRange; // Если указан тип - вычисляем при добавлении, 
    std::uint64_t                   orgAddress   = 0;
    std::uint64_t                   arraySize    = std::uint64_t(-1); // работает только для явно заданного типа

    std::string                     realTypeName; // For C++ output, full qualified

    std::size_t                     crcIndex = std::size_t(-1); // При разборе строки записи, если там нет опции CRC, то выходной параметр 
                                                                // CrcOptions не заполнен и не валиден, это поле содержит -1.
                                                                // Если опции CRC корректно заданы, то это поле устанавливается в 0,
                                                                // и парсер должен добавить опции CRC в отдельный массив, а сюда поместить
                                                                // реальный индекс.

    const TokenCollectionItemType   *pTokenInfo = 0; // стартовый токен, можно получить номер строки

    // Только размер типа
    std::uint64_t getTypeSize() const
    {
        UMBA_ASSERT(itemType==EPacketDiagramItemType::explicitType);
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
        if (itemType==EPacketDiagramItemType::explicitType)
        {
            if (arraySize==std::uint64_t(-1))
                return false;
            else
                return true;
        }

        if (itemType==EPacketDiagramItemType::singleValue)
            return false;

        return true;
    }

    std::uint64_t getArraySize() const
    {
        if (itemType==EPacketDiagramItemType::explicitType)
        {
            if (arraySize==std::uint64_t(-1))
                return 0;
            else
                return arraySize;
        }

        if (itemType==EPacketDiagramItemType::singleValue)
            return 0;

        return addressRange.end - addressRange.start+1;
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
    std::vector<CrcOptions>                crcList;
    
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

    void rebuildCrcList()
    {
        std::vector<CrcOptions> newCrcList;
        for(std::size_t i=0; i!=data.size(); ++i)
        {
            auto & curData = data[i];
            if (curData.crcIndex!=std::size_t(-1))
            {
                std::size_t newCrcIndex = newCrcList.size();
                newCrcList.emplace_back(crcList[curData.crcIndex]);
                curData.crcIndex = newCrcIndex;
            }
        }

        swap(crcList, newCrcList);
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
            rv.crcList = crcList; // Тупо копируем всё
            rv.rebuildCrcList();
        }

        return resVec;
    }


    void removeEmptyOrgs()
    {
        {
            std::vector<CrcOptions> newCrcList;
    
            auto simpleVec = splitToSimple();
            data.clear();
            for(const auto &smp : simpleVec)
            {
                auto newItemsIt = data.insert(data.end(), smp.data.begin(), smp.data.end());
                for(; newItemsIt!=data.end(); ++newItemsIt)
                {
                    auto & curData = *newItemsIt;
                    if (curData.crcIndex!=std::size_t(-1))
                    {
                        std::size_t newCrcIndex = newCrcList.size();
                        newCrcList.emplace_back(smp.crcList[curData.crcIndex]);
                        curData.crcIndex = newCrcIndex;
                    }
    
                }
            }
    
            swap(crcList, newCrcList);
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


inline
std::uint64_t getLoHalf(std::uint64_t val, std::uint64_t size)
{
    return val & makeByteSizeMask(size/2);
}

inline
std::uint64_t getHiHalf(std::uint64_t val, std::uint64_t size)
{
    return (val>>((size/2))*8) & makeByteSizeMask(size/2);
}

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

inline
byte_vector_t makeByteVector(std::uint64_t val, std::uint64_t size, Endianness endianness)
{
    byte_vector_t resVec;
    makeByteVector(val, size, endianness, resVec);
    return resVec;
}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
inline
char digitToChar(int d, bool bLower=false)
{
    d &= 0xF;
    // if (d<0)  return '-';
    if (d<10) return char('0'+d);
    return char((bLower?'a':'A')+d-10);
}


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

inline
std::string makeHexString(std::uint64_t val, std::uint64_t size)
{
    std::string resStr; resStr.reserve(16);
    if (size>8)
        size = 8;

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



//----------------------------------------------------------------------------
} // namespace utils
// umba::tokenizer::marmaid::utils::

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------

} // namespace marmaid
} // namespace tokenizer
} // namespace umba
// umba::tokenizer::marmaid::

