/*! \file
    \brief Типы для парсера marmaid packet diagram, расширенная версия, с использованием типов и массивов
 */


#pragma once

#include "marmaid_packet_diagram_enums.h"
//
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
#include "umba/string.h"


//
#include "umba/enum_helpers.h"
#include "umba/flag_helpers.h"
#include "umba/rule_of_five.h"
//
#include "marty_mem/marty_mem.h"
#include "marty_mem/virtual_address_memory_iterator.h"
#include "marty_mem/exceptions.h"

//
#include <string>
#include <stdexcept>
#include <initializer_list>
#include <unordered_map>
#include <climits>
#include <algorithm>
#include <utility>
#include <string_view>
#include <vector>

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
// umba::tokenizer::marmaid::
namespace umba {
namespace tokenizer {
namespace marmaid {

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
// using DiagramDisplayOptionFlags     = umba::tokenizer::MarmaidPacketDiagramDisplayOptionFlags;
// using DiagramDisplayOptions         = umba::tokenizer::EMarmaidPacketDiagramDisplayOptions   ;
// using DiagramParsingOptions         = umba::tokenizer::MarmaidPacketDiagramParsingOptions    ;
// using EPacketDiagramType            = umba::tokenizer::EMarmaidPacketDiagramType             ;
// using EMemoryModel                  = umba::tokenizer::EMarmaidPacketDiagramMemoryModel      ;

using EPacketDiagramType            = PacketDiagramType;
using EMemoryModel                  = MemoryModel      ;

//----------------------------------------------------------------------------
enum class EPacketDiagramItemType
{
    unknown=-1, invalid = unknown, undefined = unknown,
    singleValue, range, explicitType, org
};

//----------------------------------------------------------------------------
enum class EOrgType
{
    unknown=-1, invalid = unknown, undefined = unknown,
    orgAuto, orgAbs, orgRel
};

//----------------------------------------------------------------------------
enum class Endianness
{
    unknown=-1, invalid = unknown, undefined = unknown,
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
    // bool                            charsRange    = false; // used only with ranges or byte/char arrays, 

    Endianness                      endianness   = Endianness::undefined; // use project endianness or override endianness for this entry
    AddressRange                    addressRange; // Если указан тип - вычисляем при добавлении, 
    std::uint64_t                   orgAddress   = 0; // В сегментном режиме - сегмент
    std::uint64_t                   orgOffset    = 0; // Только для сегментного режима
    std::uint64_t                   arraySize    = std::uint64_t(-1); // работает только для явно заданного типа

    std::string                     realTypeName; // For C++ output, full qualified

    std::size_t                     checksumIndex = std::size_t(-1); // При разборе строки записи, если там нет опции CRC, то выходной параметр 
                                                                // ChecksumOptions не заполнен и не валиден, это поле содержит -1.
                                                                // Если опции CRC корректно заданы, то это поле устанавливается в 0,
                                                                // и парсер должен добавить опции CRC в отдельный массив, а сюда поместить
                                                                // реальный индекс.

    const TokenCollectionItemType   *pTokenInfo = 0; // стартовый токен, можно получить номер строки


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

    bool isSigned() const
    {
        if (itemType!=EPacketDiagramItemType::explicitType)
            return false; // bytes are unsigned

        switch(explicitTypeTokenId)
        {
            case MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_CHAR  : return true ;
            case MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_INT8  : return true ;
            case MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_INT16 : return true ;
            case MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_INT32 : return true ;
            case MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_INT64 : return true ;
            case MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_UINT8 : return false;
            case MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_UINT16: return false;
            case MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_UINT32: return false;
            case MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_UINT64: return false;

            default: return true;
        }
    }

    bool isOrgEntry() const
    {
        return itemType==EPacketDiagramItemType::org;
    }

    bool isFillEntry() const
    {
        return fillEntry;
    }

    bool isDataEntry() const
    {
        return itemType==EPacketDiagramItemType::explicitType
            || itemType==EPacketDiagramItemType::singleValue
            || itemType==EPacketDiagramItemType::range
             ;
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

    bool isByteRange() const
    {
        if (itemType==EPacketDiagramItemType::range)
            return true;
        return false;
    }

    bool isAsciiZet() const
    {
        return asciiZet;
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

    using MemoryIteratorType      = marty::mem::VirtualAddressMemoryIterator<std::uint8_t>;
    using ConstMemoryIteratorType = marty::mem::ConstVirtualAddressMemoryIterator<std::uint8_t>;

    MemoryIteratorType createMemoryIterator(const marty::mem::SegmentedAddressTraits &traits, marty::mem::Memory *pMem=0, bool errorOnWrappedAccess=false) const
    {
        using namespace marty::mem;
        return makeSegmentedVirtualAddressMemoryIterator<std::uint8_t>(pMem, orgAddress, orgOffset, errorOnWrappedAccess ? MemoryOptionFlags::errorOnWrapedAddressAccess : MemoryOptionFlags::errorOnAddressWrap, traits);
    }

    ConstMemoryIteratorType createConstMemoryIterator(const marty::mem::SegmentedAddressTraits &traits, const marty::mem::Memory *pMem=0, bool errorOnWrappedAccess=false) const
    {
        using namespace marty::mem;
        return makeSegmentedConstVirtualAddressMemoryIterator<std::uint8_t>(pMem, orgAddress, orgOffset, errorOnWrappedAccess ? MemoryOptionFlags::errorOnWrapedAddressAccess : MemoryOptionFlags::errorOnAddressWrap, traits);
    }

    MemoryIteratorType createMemoryIterator(const marty::mem::LinearAddressTraits &traits, marty::mem::Memory *pMem=0, bool errorOnWrappedAccess=false) const
    {
        using namespace marty::mem;
        return makeLinearVirtualAddressMemoryIterator<std::uint8_t>(pMem, orgAddress, errorOnWrappedAccess ? MemoryOptionFlags::errorOnWrapedAddressAccess : MemoryOptionFlags::errorOnAddressWrap, traits);
    }

    ConstMemoryIteratorType createConstMemoryIterator(const marty::mem::LinearAddressTraits &traits, const marty::mem::Memory *pMem=0, bool errorOnWrappedAccess=false) const
    {
        using namespace marty::mem;
        return makeLinearConstVirtualAddressMemoryIterator<std::uint8_t>(pMem, orgAddress, errorOnWrappedAccess ? MemoryOptionFlags::errorOnWrapedAddressAccess : MemoryOptionFlags::errorOnAddressWrap, traits);
    }



}; // struct PacketDiagramItem

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
template<typename TokenCollectionItemType>
struct PacketDiagram
{
    using PacketDiagramItemType   = PacketDiagramItem<TokenCollectionItemType>;
    using MemoryIteratorType      = typename PacketDiagramItemType::MemoryIteratorType     ;
    using ConstMemoryIteratorType = typename PacketDiagramItemType::ConstMemoryIteratorType;


    EPacketDiagramType                     diagramType = EPacketDiagramType::unknown;
    std::string                            title  ;
    std::vector<PacketDiagramItemType>     data   ;
    std::vector<ChecksumOptions>           checksumList;
    
    PacketDiagramParsingOptions            parsingOptions      = PacketDiagramParsingOptions::all;

    Endianness                             endianness          = Endianness::unknown;
    EMemoryModel                           memoryModel         = EMemoryModel::flat;
    PacketDiagramDisplayOptionFlags        displayOptionFlags  = PacketDiagramDisplayOptionFlags::byteNumbers 
                                                               | PacketDiagramDisplayOptionFlags::splitWordsToBytes
                                                               // | PacketDiagramDisplayOptionFlags::singleByteNumbers
                                                               ;

    std::uint64_t                          dataBitSize         = 32; // 
    std::uint64_t                          segmentBitSize      = 16; // 
    std::uint64_t                          segmentShift        =  4; // 
    std::uint64_t                          offsetBitSize       = 16; // 

    std::uint64_t                          displayWidth        = 32;
    std::uint64_t                          lastOrg             = 0 ;
    std::uint64_t                          lastOrgOffset       = 0 ;
    std::uint64_t                          orgAddress          = std::uint64_t(-1);
    std::uint64_t                          orgOffset           = std::uint64_t(-1);


    std::unordered_map<std::string, std::size_t>     entryNames;  // храним индекс в векторе data
    std::unordered_map<std::string, std::size_t>     orgNames  ;  // храним индекс в векторе data
    std::size_t                                      fillEntryCounter = 0;


    void setDisplayOptionFlags(PacketDiagramDisplayOptionFlags flags)
    {
        displayOptionFlags = flags;
    }

    //void setResetOptionFlag(DiagramDisplayOptionFlags flag, bool bSet /* else - clear*/)
    void setResetOptionFlags(PacketDiagramDisplayOptionFlags flagsSet, PacketDiagramDisplayOptionFlags flagsReset)
    {
        displayOptionFlags &= ~flagsReset;
        displayOptionFlags |=  flagsSet  ;
    }

    void setOptionFlag(PacketDiagramDisplayOptions opt)
    {
        if ((parsingOptions&PacketDiagramParsingOptions::allowOverrideDisplayOptionFlags)==0) // Если переопределение не разрешено, выходим
            return;

        switch(opt)
        {
            case PacketDiagramDisplayOptions::singleByteNumbers       : setResetOptionFlags(PacketDiagramDisplayOptionFlags::singleByteNumbers, PacketDiagramDisplayOptionFlags::none); break;
            case PacketDiagramDisplayOptions::noSingleByteNumbers     : setResetOptionFlags(PacketDiagramDisplayOptionFlags::none, PacketDiagramDisplayOptionFlags::singleByteNumbers); break;

            case PacketDiagramDisplayOptions::singleByteBlockNumbers  : setResetOptionFlags(PacketDiagramDisplayOptionFlags::singleByteBlockNumbers, PacketDiagramDisplayOptionFlags::none); break;
            case PacketDiagramDisplayOptions::noSingleByteBlockNumbers: setResetOptionFlags(PacketDiagramDisplayOptionFlags::none, PacketDiagramDisplayOptionFlags::singleByteBlockNumbers); break;

            case PacketDiagramDisplayOptions::byteNumbers             : setResetOptionFlags(PacketDiagramDisplayOptionFlags::byteNumbers, PacketDiagramDisplayOptionFlags::none); break;
            case PacketDiagramDisplayOptions::noByteNumbers           : setResetOptionFlags(PacketDiagramDisplayOptionFlags::none, PacketDiagramDisplayOptionFlags::byteNumbers); break;

            case PacketDiagramDisplayOptions::splitWordsToBytes       : setResetOptionFlags(PacketDiagramDisplayOptionFlags::splitWordsToBytes, PacketDiagramDisplayOptionFlags::none); break;
            case PacketDiagramDisplayOptions::noSplitWordsToBytes     : setResetOptionFlags(PacketDiagramDisplayOptionFlags::none, PacketDiagramDisplayOptionFlags::splitWordsToBytes); break;

            case PacketDiagramDisplayOptions::rangeAsChars            : setResetOptionFlags(PacketDiagramDisplayOptionFlags::rangeAsChars, PacketDiagramDisplayOptionFlags::none); break;
            case PacketDiagramDisplayOptions::rangeAsBytes            : setResetOptionFlags(PacketDiagramDisplayOptionFlags::none, PacketDiagramDisplayOptionFlags::rangeAsChars); break;

            case PacketDiagramDisplayOptions::none   : break;
            case PacketDiagramDisplayOptions::invalid: break;
        }
    }

    bool testDisplayOption(PacketDiagramDisplayOptionFlags flags) const
    {
        return (displayOptionFlags&flags) == flags;
    }

    bool testDisplayOption(PacketDiagramDisplayOptions opt) const
    {
        switch(opt)
        {
            case PacketDiagramDisplayOptions::singleByteNumbers       : return  testDisplayOption(PacketDiagramDisplayOptionFlags::singleByteNumbers);
            case PacketDiagramDisplayOptions::noSingleByteNumbers     : return !testDisplayOption(PacketDiagramDisplayOptionFlags::singleByteNumbers);

            case PacketDiagramDisplayOptions::singleByteBlockNumbers  : return  testDisplayOption(PacketDiagramDisplayOptionFlags::singleByteBlockNumbers);
            case PacketDiagramDisplayOptions::noSingleByteBlockNumbers: return !testDisplayOption(PacketDiagramDisplayOptionFlags::singleByteBlockNumbers);

            case PacketDiagramDisplayOptions::byteNumbers             : return  testDisplayOption(PacketDiagramDisplayOptionFlags::byteNumbers);
            case PacketDiagramDisplayOptions::noByteNumbers           : return !testDisplayOption(PacketDiagramDisplayOptionFlags::byteNumbers);

            case PacketDiagramDisplayOptions::splitWordsToBytes       : return  testDisplayOption(PacketDiagramDisplayOptionFlags::splitWordsToBytes);
            case PacketDiagramDisplayOptions::noSplitWordsToBytes     : return !testDisplayOption(PacketDiagramDisplayOptionFlags::splitWordsToBytes);

            case PacketDiagramDisplayOptions::rangeAsChars            : return  testDisplayOption(PacketDiagramDisplayOptionFlags::rangeAsChars);
            case PacketDiagramDisplayOptions::rangeAsBytes            : return !testDisplayOption(PacketDiagramDisplayOptionFlags::rangeAsChars);

            case PacketDiagramDisplayOptions::none   : break;
            case PacketDiagramDisplayOptions::invalid: break;
        }
        
        UMBA_ASSERT_FAIL_MSG("unknown PacketDiagramDisplayOptions option");
        throw std::runtime_error("unknown PacketDiagramDisplayOptions option");
    }


    int getDisplayWidth() const
    {
        if (displayWidth<=12u)
            return 8;
        if (displayWidth<=20u)
            return 16;
        if (displayWidth<=28u)
            return 24;
        // if (displayWidth<=40u)
        //     return 32;
        // if (displayWidth<=56u)
        //     return 48;
        //  
        // return 64;

        return 32;
    }
    
    Endianness getItemEndianness(const PacketDiagramItemType &item) const
    {
        auto e = item.endianness;
        if (e==Endianness::unknown)
            e = endianness;
        if (e==Endianness::unknown)
            e = Endianness::littleEndian;

        return e;
    }

    MemoryIteratorType createMemoryIterator(const PacketDiagramItemType &item, marty::mem::Memory *pMem=0, bool errorOnWrappedAccess=false) const
    {
        if (memoryModel==EMemoryModel::flat)
            return item.createMemoryIterator(marty::mem::LinearAddressTraits(), pMem, errorOnWrappedAccess);

        marty::mem::SegmentedAddressTraits traits;
        traits.segmentBitSize = segmentBitSize;
        traits.offsetBitSize  = offsetBitSize ;
        traits.paragraphSize  = 1ull<<segmentShift  ;

        return item.createMemoryIterator(traits, pMem, errorOnWrappedAccess);
    }

    ConstMemoryIteratorType createConstMemoryIterator(const PacketDiagramItemType &item, marty::mem::Memory *pMem=0, bool errorOnWrappedAccess=false) const
    {
        if (memoryModel==EMemoryModel::flat)
            return item.createConstMemoryIterator(marty::mem::LinearAddressTraits(), pMem, errorOnWrappedAccess);

        marty::mem::SegmentedAddressTraits traits;
        traits.segmentBitSize = segmentBitSize;
        traits.offsetBitSize  = offsetBitSize ;
        traits.paragraphSize  = 1ull<<segmentShift  ;

        return item.createConstMemoryIterator(traits, pMem, errorOnWrappedAccess);
    }

    MemoryIteratorType createZeroBaseMemoryIterator(marty::mem::Memory *pMem=0, bool errorOnWrappedAccess=false) const
    {
        using namespace marty::mem;

        if (memoryModel==EMemoryModel::flat)
            return makeLinearVirtualAddressMemoryIterator<std::uint8_t>(pMem, 0 /* orgAddress */ , errorOnWrappedAccess ? MemoryOptionFlags::errorOnWrapedAddressAccess : MemoryOptionFlags::errorOnAddressWrap, LinearAddressTraits());

        marty::mem::SegmentedAddressTraits traits;
        traits.segmentBitSize = segmentBitSize;
        traits.offsetBitSize  = offsetBitSize ;
        traits.paragraphSize  = 1<<segmentShift  ;

        return makeSegmentedVirtualAddressMemoryIterator<std::uint8_t>(pMem, 0 /* orgAddress */ , 0 /* orgOffset */ , errorOnWrappedAccess ? MemoryOptionFlags::errorOnWrapedAddressAccess : MemoryOptionFlags::errorOnAddressWrap, traits);
    }

    ConstMemoryIteratorType createZeroBaseConstMemoryIterator(marty::mem::Memory *pMem=0, bool errorOnWrappedAccess=false) const
    {
        using namespace marty::mem;

        if (memoryModel==EMemoryModel::flat)
            return makeLinearConstVirtualAddressMemoryIterator<std::uint8_t>(pMem, 0 /* orgAddress */ , errorOnWrappedAccess ? MemoryOptionFlags::errorOnWrapedAddressAccess : MemoryOptionFlags::errorOnAddressWrap, LinearAddressTraits());

        marty::mem::SegmentedAddressTraits traits;
        traits.segmentBitSize = segmentBitSize;
        traits.offsetBitSize  = offsetBitSize ;
        traits.paragraphSize  = 1ull<<segmentShift  ;

        return makeSegmentedConstVirtualAddressMemoryIterator<std::uint8_t>(pMem, 0 /* orgAddress */ , 0 /* orgOffset */ , errorOnWrappedAccess ? MemoryOptionFlags::errorOnWrapedAddressAccess : MemoryOptionFlags::errorOnAddressWrap, traits);
    }

    MemoryIteratorType createMemoryIterator(std::size_t idx, marty::mem::Memory *pMem=0, bool errorOnWrappedAccess=false) const
    {
        if (data.empty())
            return createZeroBaseMemoryIterator(pMem, errorOnWrappedAccess);

        if (idx<data.size())
            return createMemoryIterator(data[idx], pMem, errorOnWrappedAccess);

        auto it = createMemoryIterator(data.back(), pMem, errorOnWrappedAccess);
        it += data.back().getTypeFieldSize();

        return it;
    }

    ConstMemoryIteratorType createConstMemoryIterator(std::size_t idx, marty::mem::Memory *pMem=0, bool errorOnWrappedAccess=false) const
    {
        if (data.empty())
            return createZeroBaseConstMemoryIterator(pMem, errorOnWrappedAccess);

        if (idx<data.size())
            return createConstMemoryIterator(data[idx], pMem, errorOnWrappedAccess);

        auto it = createConstMemoryIterator(data.back(), pMem, errorOnWrappedAccess);
        it += marty::mem::ptrdiff_t(data.back().getTypeFieldSize());

        return it;
    }


    static std::string makeFieldId(const std::string &t)
    {
        return makeIdFromText(t);
    }

    std::size_t findEntryByName(const std::string &name) const
    {
        auto id = makeFieldId(name);
        std::unordered_map<std::string, std::size_t>::const_iterator it = entryNames.find(id);
        if (it==entryNames.end())
            return std::size_t(-1);
        return it->second;
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

    std::size_t findLastOrgIdxBefore(std::size_t entryIdx=std::size_t(-1)) const
    {
        if (entryIdx>data.size())
            entryIdx = data.size();

        std::size_t lastOrgIdx = std::size_t(-1);

        for(std::size_t i=0; i!=entryIdx; ++i)
        {
            if (data[i].itemType!=EPacketDiagramItemType::org)
                continue;

            lastOrgIdx = i;
        }

        return lastOrgIdx;
    }

    std::size_t findOrgFrom(std::size_t entryIdx=0) const
    {
        // if (entryIdx>data.size())
        //     entryIdx = data.size();

        while(entryIdx<data.size())
        {
            if (data[entryIdx].isOrgEntry())
                return entryIdx;
            ++entryIdx;
        }

        return entryIdx;
    }

    std::size_t findNextOrgOrFillEntry(std::size_t curOrgIdx=0, bool stopOnFill=false) const
    {
        std::size_t idx = curOrgIdx;

        // if (idx>data.size())
        //     idx = 0;

        ++idx; // переходим на следующий индекс

        while(idx<data.size())
        {
            if (data[idx].isOrgEntry())
                return idx;
            if (data[idx].isFillEntry() && stopOnFill)
                return idx;
            ++idx;
        }

        return idx;
    }

    // fullSections - including filling
    std::vector< std::pair<std::size_t, std::size_t> > findAllSections(bool fullSections=true) const
    {
        std::size_t idxStart = findOrgFrom(0);
        if (idxStart>=data.size())
            return std::vector< std::pair<std::size_t, std::size_t> >(1, std::make_pair(std::size_t(0), findNextOrgOrFillEntry(0, !fullSections)));

        std::vector< std::pair<std::size_t, std::size_t> > resVec;

        std::size_t nextOrgIdx = idxStart;
        std::size_t idxEnd     = 0;

        do
        {
            idxStart   = nextOrgIdx;
            nextOrgIdx = findNextOrgOrFillEntry(idxStart);
            idxEnd     = nextOrgIdx;
            if (!fullSections)
                 idxEnd = findNextOrgOrFillEntry(idxStart, true); // либо тот же ORG найдёт, либо остановится на fill элементе

            resVec.emplace_back(std::make_pair(idxStart, idxEnd));

        } while(idxEnd<data.size());

        return resVec;
    }

    // Вычисляет базовый адрес для записи с индексом entryIdx. Нужно научить выдавать итератор для entry, которая могла бы следовать за последней
    ConstMemoryIteratorType getBaseAddressIterator(std::size_t entryIdx=std::size_t(-1), marty::mem::Memory *pMem=0) const
    {
        std::size_t lastOrgIdx = findLastOrgIdxBefore(entryIdx);

        if (lastOrgIdx==std::size_t(-1))
            return createZeroBaseConstMemoryIterator(pMem);

        return createConstMemoryIterator(data[lastOrgIdx], pMem);
        // return addr;
        // ConstMemoryIteratorType createConstMemoryIterator(const PacketDiagramItemType &item, marty::mem::Memory *pMem = 0) const
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
byte_vector_t makeByteVectorStringFromStringLiteral(const std::string &valStr, std::uint64_t size, bool asciiZ=false)
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
inline
byte_vector_t makeByteVectorFromDumpString(const std::string &strDump)
{
    byte_vector_t bytes;
    int res = isStringDumpString(strDump, &bytes);

    if (res<0)
        throw std::invalid_argument("invalid char found in dump");
    else if (res>0)
        throw std::invalid_argument("wrong number of digits in dump");

    return bytes;
}

//----------------------------------------------------------------------------
inline
byte_vector_t makeByteVectorFromDumpString(const std::string &strDump, std::uint64_t size, bool exactFit=false)
{
    byte_vector_t bytes = makeByteVectorFromDumpString(strDump);

    if (bytes.size()>size)
        throw std::out_of_range("hex dump can't fit into " + std::to_string(size) + " bytes: required length is " + std::to_string(bytes.size()));

    if (exactFit && bytes.size()!=size)
        throw std::out_of_range("hex dump can't fit into " + std::to_string(size) + " bytes: required length is " + std::to_string(bytes.size()));

    return bytes;
}

//----------------------------------------------------------------------------

//! Строка val - строка дампа, каждый байт представлен парой HEX-цифр, пробелы допустимы произвольно, не обязательно попарно
//! Может использоваться только для байтовых массивов или диапазонов
inline
byte_vector_t makeByteVectorStringFromDumpString(const std::string &strDump, std::uint64_t size, bool asciiZ=false)
{
    byte_vector_t bytes = makeByteVectorFromDumpString(strDump);

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
template<typename TokenCollectionItemType, typename LogMessageHandler>
void memorySetVariable(const PacketDiagram<TokenCollectionItemType> &diagram, marty::mem::Memory &mem, std::string varFullName, std::string value, LogMessageHandler logMsgHandler)
{
    // using DiagramType = decltype(diagram);
    using MemoryIteratorType = typename PacketDiagram<TokenCollectionItemType> :: MemoryIteratorType;
    

    varFullName = umba::string::trim_copy(varFullName);
    value = umba::string::trim_copy(value);

    std::string varName;
    std::uint64_t varArrayIndex = 0;
    bool doSetArrayItem = false;

    umba::string::unquote(varFullName);

    int parseVarNameRes = umba::tokenizer::marmaid::utils::simpleSplitNameAndIndex(varFullName , &varName, &varArrayIndex);
    if (parseVarNameRes==0)
    {
        marty::mem::throwMemoryAccessError(marty::mem::MemoryAccessResultCode::memoryFillError, "error in entry name: '" + varFullName + "'");
    }

    if (parseVarNameRes>0)
        doSetArrayItem = true;

    std::size_t entryIdx = diagram.findEntryByName(varName);
    if (entryIdx==std::size_t(-1))
    {
        marty::mem::throwMemoryAccessError(marty::mem::MemoryAccessResultCode::memoryFillError, "entry not found: '" + varFullName + "'");
    }

    const auto &entryItem = diagram.data[entryIdx];
    if (!entryItem.isDataEntry())
    {
        marty::mem::throwMemoryAccessError(marty::mem::MemoryAccessResultCode::memoryFillError, "entry not a data entry: '" + varFullName + "'");
    }

    // Endianness
    auto entryEndianness = diagram.getItemEndianness(entryItem);
    std::uint64_t entryTypeSize = entryItem.getTypeSize();

    auto valueQuotType = umba::string::unquote(value);

    byte_vector_t bv;
    MemoryIteratorType fieldMemoryIt;

    if (doSetArrayItem)
    {
        if (!entryItem.isArray())
        {
            marty::mem::throwMemoryAccessError(marty::mem::MemoryAccessResultCode::memoryFillError, "try to set indexed variable, but data entry is not an array: '" + varFullName + "'");
        }

        if (varArrayIndex>=entryItem.getArraySize())
        {
            marty::mem::throwMemoryAccessError(marty::mem::MemoryAccessResultCode::memoryFillError, "taken index is out of range: '" + varFullName + "'");
        }

        if (valueQuotType==umba::string::SimpleQuotesType::notQuoted)
        {
            bv = makeByteVectorFromIntStr(value, entryTypeSize, entryEndianness, entryItem.isSigned() /* , std::uint64_t *pParsedVal=0 */ );
        }
        else if (valueQuotType==umba::string::SimpleQuotesType::aposQuoted)
        {
            bv = makeByteVectorFromCharLiteral(value, entryTypeSize, entryEndianness /* , std::uint64_t *pParsedVal=0 */ );
        }
        else // valueQuotType==umba::string::SimpleQuotesType::dblQuoted
        {
            marty::mem::throwMemoryAccessError(marty::mem::MemoryAccessResultCode::memoryFillError, "can't assign string to an array item: '" + varFullName + "'");
        }

        // MemoryIteratorType createMemoryIterator(const PacketDiagramItemType &item, marty::mem::Memory *pMem = 0) const
        fieldMemoryIt  = diagram.createMemoryIterator(entryItem, &mem, true /* errorOnWrappedAccess */ );
        fieldMemoryIt += std::int64_t(entryTypeSize*varArrayIndex);
        
    }
    else // задаём поле, индекс массива не указан. Если размер типа - 1, и у нас задана строка или дамп - то нормас
    {
        if (entryItem.isArray()) // поле объявлено как массив
        {
            if (entryTypeSize!=1)
            {
                marty::mem::throwMemoryAccessError(marty::mem::MemoryAccessResultCode::memoryFillError, "Can't assign arrays of elements with size greater than 1: '" + varFullName + "'");
            }
           
            if (valueQuotType==umba::string::SimpleQuotesType::dblQuoted)
            {
                bv = makeByteVectorStringFromStringLiteral(value, entryItem.getArraySize(), entryItem.isAsciiZet());
            }
            else if (valueQuotType==umba::string::SimpleQuotesType::notQuoted)
            {
                bv = makeByteVectorStringFromDumpString(value, entryItem.getArraySize(), entryItem.isAsciiZet());
            }
            else
            {
                marty::mem::throwMemoryAccessError(marty::mem::MemoryAccessResultCode::memoryFillError, "can't assign char literal to an string/array: '" + varFullName + "'");
            }

        }
        else // обычное поле
        {
            if (valueQuotType==umba::string::SimpleQuotesType::notQuoted)
            {
                bv = makeByteVectorFromIntStr(value, entryTypeSize, entryEndianness, entryItem.isSigned() /* , std::uint64_t *pParsedVal=0 */ );
            }
            else if (valueQuotType==umba::string::SimpleQuotesType::aposQuoted)
            {
                bv = makeByteVectorFromCharLiteral(value, entryTypeSize, entryEndianness /* , std::uint64_t *pParsedVal=0 */ );
            }
            else // valueQuotType==umba::string::SimpleQuotesType::dblQuoted
            {
                marty::mem::throwMemoryAccessError(marty::mem::MemoryAccessResultCode::memoryFillError, "Can't assign string to simple field: '" + varFullName + "'");
            }
        }

        fieldMemoryIt  = diagram.createMemoryIterator(entryItem, &mem, true /* errorOnWrappedAccess */ );
    }

    // assign bv to memory here

    //LOG_MSG << "---\n";
    logMsgHandler("Set '" + varFullName + "' to " + value);
    //LOG_MSG << "Set '" << varFullName << "' to " << value << "\n";

    for(auto b : bv)
    {
        auto byte = b; // std::uint8_t(*fieldMemoryIt); // b; // std::uint8_t(*it);
        auto byteStr = marty::mem::utils::makeHexString(byte, 1);

        //LOG_MSG << std::string(fieldMemoryIt) << ": " << byteStr << "\n";
        logMsgHandler(std::string(fieldMemoryIt) + ": " + byteStr);

        *fieldMemoryIt = b;
        ++fieldMemoryIt;
    }

}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
// Делаем вектор string_view из строки текста. Строка разделяется только символами LF ('\n')
// Символ перевода строки также входит в string_view
inline
std::vector<std::string_view> makeTextStringViewsHelper(const std::string &text)
{
    std::vector<std::string_view> resVec; resVec.reserve(text.size()/64);

    std::size_t idxStart = 0;
    std::size_t idx      = 0;

    for(; idx!=text.size(); ++idx)
    {
        if (text[idx]=='\n')
        {
            std::size_t idxNext = idx+1;
            std::size_t size    = idxNext - idxStart;
            resVec.emplace_back(text.data()+idxStart, size);
            idxStart = idxNext;
        }
    }

    if (idxStart!=idx)
    {
        resVec.emplace_back(text.data()+idxStart, idx-idxStart);
    }

    return resVec;
}

//----------------------------------------------------------------------------
// Обрезаем переводы строки из вектора string_view
inline
std::vector<std::string_view> stripLinefeedsFromStringViewsVector(const std::vector<std::string_view> &svVec)
{
    std::vector<std::string_view> svRes = svVec;
    for(auto &sv : svRes)
    {
        while(!sv.empty())
        {
            if (sv.back()=='\r' || sv.back()=='\n')
                sv.remove_suffix(1);
            else
                break;
        }
    }

    return svRes;
}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
inline std::string_view removePrefix(std::string_view &sv, std::size_t n) { sv.remove_prefix(n); return sv; }
inline std::string_view removeSuffix(std::string_view &sv, std::size_t n) { sv.remove_suffix(n); return sv; }

inline std::string removePrefix(std::string &str, std::size_t n)
{
    // basic_string& erase( size_type index = 0, size_type count = npos );
    // https://en.cppreference.com/w/cpp/string/basic_string/erase

    if (n>str.size())
        n = str.size(); // Для строк - не задаём вопросов

    str.erase(0, n);

    return str;
}

inline std::string removeSuffix(std::string &str, std::size_t n)
{
    // basic_string& erase( size_type index = 0, size_type count = npos );
    // https://en.cppreference.com/w/cpp/string/basic_string/erase

    if (n>str.size())
        n = str.size(); // Для строк - не задаём вопросов

    str.erase(str.size()-n /* , n */ ); // Удаляем до конца

    return str;
}




//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
template<typename IteratorType>
std::size_t ltrim_distance(IteratorType b, IteratorType e)
{
    std::size_t idx = 0;
    for(; b!=e; ++b, ++idx)
    {
        if (!(*b==' ' || *b=='\t' || *b=='\r' || *b=='\n'))
            break;
    }
    return idx;
}

//----------------------------------------------------------------------------
template<typename SomethingStringLike>
std::size_t ltrim_distance(const SomethingStringLike &str) { return ltrim_distance(str.begin(), str.end()); }

inline void             ltrim     (std::string_view &sv)   { removePrefix(sv, ltrim_distance(sv.begin(), sv.end())); }
inline std::string_view ltrim_copy(std::string_view  sv)   { ltrim(sv); return sv; }
inline void             ltrim     (std::string      &sv)   { removePrefix(sv, ltrim_distance(sv.begin(), sv.end())); }
inline std::string      ltrim_copy(std::string       sv)   { ltrim(sv); return sv; }

//----------------------------------------------------------------------------
template<typename IteratorType>
std::size_t rtrim_distance(IteratorType b, IteratorType e)
{
    // IteratorType bcp = b;
    std::size_t idx = 0;
    //for(; e!=b; --e, ++idx)
    //for(; e--!=b; ++idx)
    for(; e!=b; ++idx)
    {
        e--;
        if (!(*e==' ' || *e=='\t' || *e=='\r' || *e=='\n'))
            return idx;
    }
    //return std::distance(bcp, e);
    return idx;
}

    // std::size_t idx = sv.size();
    // while(idx-->0)
    // {
    //     if (!(sv[idx]==' ' || sv[idx]=='\t' || sv[idx]=='\r' || sv[idx]=='\n'))
    //         break;
    // }
    //  
    // std::size_t rmSize = sv.size()-idx-1;
    // sv.remove_suffix(rmSize);
    // sv.remove_suffix(rtrim_distance(sv.begin(), sv.end()));


//----------------------------------------------------------------------------
template<typename SomethingStringLike>
std::size_t rtrim_distance(const SomethingStringLike &str) { return rtrim_distance(str.begin(), str.end()); }

inline void             rtrim     (std::string_view &sv)   { removeSuffix(sv, rtrim_distance(sv.begin(), sv.end())); }
inline std::string_view rtrim_copy(std::string_view  sv)   { rtrim(sv); return sv; }
inline void             rtrim     (std::string      &sv)   { removeSuffix(sv, rtrim_distance(sv.begin(), sv.end())); }
inline std::string      rtrim_copy(std::string       sv)   { rtrim(sv); return sv; }

//----------------------------------------------------------------------------
inline void             trim      (std::string_view &sv)   { ltrim(sv); rtrim(sv); }
inline std::string_view trim_copy (std::string_view  sv)   { return ltrim_copy(rtrim_copy(sv)); }
inline void             trim      (std::string      &sv)   { ltrim(sv); rtrim(sv); }
inline std::string      trim_copy (std::string       sv)   { return ltrim_copy(rtrim_copy(sv)); }

//----------------------------------------------------------------------------
inline
void rtrim(std::vector<std::string_view> &svVec)
{
    for(auto &sv : svVec)
        rtrim(sv);
}

//----------------------------------------------------------------------------
inline
std::vector<std::string_view> rtrim_copy(std::vector<std::string_view> svVec)
{
    rtrim(svVec);
    return svVec;
}

//----------------------------------------------------------------------------
inline
char tosame(char ch)
{
    return ch;
}

//----------------------------------------------------------------------------
inline
char tolower(char ch)
{
    if (ch>='A' && ch<='Z')
        ch = ch - 'A' + 'a';
    return ch;
}

//----------------------------------------------------------------------------
inline
std::string tolower_copy(std::string s)
{
    for(auto &ch : s)
    {
        ch = tolower(ch);
    }

    return s;
}

//----------------------------------------------------------------------------
inline
bool is_equal_chars(char ch1, char ch2, bool ci)
{
    return ci ? tolower(ch1)==tolower(ch2) : ch1==ch2;
}

//----------------------------------------------------------------------------
inline
bool is_equal(std::string_view sv1, std::string_view sv2)
{
    if (sv1.size()!=sv2.size())
        return false;

    return sv1==sv2;
}

//----------------------------------------------------------------------------
inline
bool is_equal(std::string_view sv, const std::string &str)
{
    return is_equal(sv, std::string_view(str.data(), str.size()));
}

//----------------------------------------------------------------------------
inline
bool is_equal(const std::string &str, std::string_view sv)
{
    return is_equal(sv, std::string_view(str.data(), str.size()));
}

//----------------------------------------------------------------------------
template<typename CharEqual>
bool is_equal(std::string_view sv1, std::string_view sv2, CharEqual charEqual)
{
    if (sv1.size()!=sv2.size())
        return false;

    auto it1 = sv1.begin();
    auto it2 = sv2.begin();

    for(; it1!=sv1.end(); ++it1, ++it2)
    {
        if (!charEqual(*it1, *it2))
            return false;
    }

    return true;
}

//----------------------------------------------------------------------------
template<typename CharEqual>
bool is_equal(std::string_view sv, const std::string &str, CharEqual charEqual)
{
    return is_equal(sv, std::string_view(str.data(), str.size()), charEqual);
}

//----------------------------------------------------------------------------
template<typename CharEqual>
bool is_equal(const std::string &str, std::string_view sv, CharEqual charEqual)
{
    return is_equal_ci(sv, std::string_view(str.data(), str.size()), charEqual);
}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
// Модифицирует содержимое вьюхи, забивая все непробельные символы
// Так издеваться можно только над вьюхами, которые сделаны из неконстантных массивов/строк
// Вполне можно использовать, если часть строк в тексте, которые представлены вьюхами, надо забить пробелами
inline
void make_string_view_space_only(std::string_view &sv)
{
    auto ptr = const_cast<char*>(sv.data());
    for(std::size_t i=0u; i!=sv.size(); ++i)
    {
        ptr[i] = ' ';
    }
}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
// Про YAML - https://habr.com/ru/articles/710414/
// JSON - https://www.json.org/json-en.html
// The Jekyll static site generator popularized YAML front matter which is deliminated by YAML section markers.
inline
bool extractYamlFrontMatter(std::vector<std::string_view> &svVec, std::vector<std::string> *pfm=0)
{
    if (svVec.empty())
        return false;

    const std::string_view hiphenus = "---";
    if (rtrim_copy(svVec[0])!=hiphenus)
        return false;

    std::size_t idx = 1;
    for(; idx!=svVec.size(); ++idx)
    {
        if (rtrim_copy(svVec[idx])==hiphenus)
        {
            if (pfm)
                *pfm = std::vector<std::string>(svVec.begin()+std::ptrdiff_t(1), svVec.begin()+std::ptrdiff_t(idx));
            std::size_t size = idx+1;
            for(idx=0; idx!=size; ++idx)
            {
                make_string_view_space_only(svVec[idx]);
                //svVec[idx] = std::string_view();
            }
            return true;
        }
    }

    return false;
}

//----------------------------------------------------------------------------
// Строки должны быть rtrimmed
inline
bool extractMarkeredPart(std::vector<std::string_view> &svVec, const std::string &startMarker, const std::string &endMarker, bool ci, std::vector<std::string> *pMarkered=0)
{
    auto isEqual = [&](auto s1, auto s2)
    {
        return is_equal( s1, s2, [&](char ch1, char ch2) { return is_equal_chars(ch1, ch2, ci); });
    };


    std::size_t idx = 0;
    for(; idx!=svVec.size(); ++idx)
    {
        if (isEqual(rtrim_copy(svVec[idx]), startMarker))
           break;
    }

    if (idx==svVec.size())
        return false;

    std::size_t idxStart = idx;

    for(++idx; idx!=svVec.size(); ++idx)
    {
        if (isEqual(rtrim_copy(svVec[idx]), endMarker))
           break;
    }

    if (idx==svVec.size())
        return false;

    std::size_t idxEnd = idx+1;

    if (pMarkered)
    {
        *pMarkered = std::vector<std::string>(svVec.begin()+std::ptrdiff_t(idxStart), svVec.begin()+std::ptrdiff_t(idxEnd));
    }

    for(idx=idxStart; idx!=idxEnd; ++idx)
    {
        make_string_view_space_only(svVec[idx]);
    }

    return true;
}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
template<typename StringType>
StringType concat_copy(const StringType &s1, const StringType &s2, const StringType &sep)
{
    return rtrim_copy(s1) + sep + ltrim_copy(s2);
}

template<typename StringType>
void concat(StringType &concatTo, const StringType &str, const StringType &sep)
{
    concatTo = concat_copy(concatTo, str, sep);
}

//----------------------------------------------------------------------------
// Разбирает ямловский заголовок, считая, что один параметр - ровно одна строка. Значения тэгов приводятся к нижнему регистру и кладутся в мапу
// Возвращает true, если был найден хоть один тэг
inline
bool simpleParseYamlFrontMatter(const std::vector<std::string> &lines, std::unordered_map<std::string, std::string> &tags)
{
    UMBA_USED(lines);
    UMBA_USED(tags);

    if (lines.empty())
        return false;

    std::size_t baseIndent = 0;

    std::size_t lineIdx = 0;

    // Ищем базовый отступ
    for(; lineIdx!=lines.size(); ++lineIdx)
    {
        auto ltrDistance = ltrim_distance(lines[lineIdx]);
        if (ltrDistance>=lines[lineIdx].size()) // строка пустая
            continue;
        if (ltrDistance>baseIndent)
            baseIndent = ltrDistance;
        break;
    }

    std::string lastKey;

    // Базовый отступ есть, если строка имеет другой отступ, она конкатенируется к предыдущей
    for(lineIdx=0; lineIdx!=lines.size(); ++lineIdx)
    {
        const auto &line = lines[lineIdx];
        auto ltrDistance = ltrim_distance(line);
        if (ltrDistance!=baseIndent)
        {
            concat(tags[lastKey], trim_copy(line), std::string(" "));
        }
        else
        {
            auto colonPos = line.find(':');
            if (colonPos==line.npos)
            {
                lastKey = tolower_copy(trim_copy(line));
            }
            else
            {
                lastKey       = tolower_copy(trim_copy(std::string(line, 0, colonPos)));
                tags[lastKey] = trim_copy(std::string(line, colonPos+1, line.npos));
            }
        }
    }
    
    return !tags.empty();
}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
// text must be LF only
inline
void prepareTextForDiagramParsing(const std::string &text, std::vector<std::string> *pStyle, std::unordered_map<std::string, std::string> *pFmTags)
{
    auto viewsVec = rtrim_copy(makeTextStringViewsHelper(text));

    std::vector<std::string> fmLines;
    if (extractYamlFrontMatter(viewsVec, &fmLines) && pFmTags)
    {
        simpleParseYamlFrontMatter(fmLines, *pFmTags);
    }

    extractMarkeredPart(viewsVec, "<style>", "</style>", true, pStyle);
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

