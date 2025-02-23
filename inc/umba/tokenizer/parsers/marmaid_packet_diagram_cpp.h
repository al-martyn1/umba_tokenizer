/*! \file
    \brief Хелперы для вывода в сишечку и плюсики, а также в текстовые диаграммы
 */


#pragma once

#include "marmaid_packet_diagram_parser.h"
#include "marty_cpp/marty_cpp.h"
//

#include <string>
#include <sstream>
#include <iomanip>


//----------------------------------------------------------------------------
// umba::tokenizer::marmaid::cpp::
namespace umba {
namespace tokenizer {
namespace marmaid {
namespace cpp {

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
template<typename StreamType>
std::size_t simplePrintStructMember( StreamType &oss
                                   , const std::string &typeName
                                   , const std::string &memberName
                                   , std::uint64_t arraySize
                                   , std::size_t indent=4
                                   , std::size_t typeFieldWidth=16
                                   , std::size_t nameFieldWidth=24
                                   )
{
    std::size_t nExtraSpaceTypeName = 0;
    if (typeName.size()<typeFieldWidth)
        nExtraSpaceTypeName = typeFieldWidth - typeName.size();

    std::string arrayStr;
    if (arraySize!=std::uint64_t(-1))
        arrayStr = "[" + std::to_string(arraySize) + "]";

    std::size_t memberNameWidth = memberName.size() + arrayStr.size();
    std::size_t nExtraSpaceMemberName = 0;
    if (memberNameWidth<nameFieldWidth)
        nExtraSpaceMemberName = nameFieldWidth - memberNameWidth;

    oss << std::string(indent, ' ') 
        << typeName << std::string(nExtraSpaceTypeName, ' ')
        << memberName << arrayStr << ";";

    return nExtraSpaceMemberName;
}

//----------------------------------------------------------------------------
template<typename StreamType, typename TokenCollectionItemType>
void printPacketDiagramItem( StreamType &oss
                           , const PacketDiagramItem<TokenCollectionItemType> &item
                           , bool fCpp=true
                           , marty_cpp::NameStyle nameStyle=marty_cpp::NameStyle::camelStyle
                           , std::size_t indent=4
                           , std::size_t typeFieldWidth=16
                           , std::size_t nameFieldWidth=24
                           )
{
    std::size_t nameExtraSpace = simplePrintStructMember( oss
                                                        , item.getCppOrCTypeName(fCpp)
                                                        , marty_cpp::formatName( item.getCppOrCFieldName(fCpp), nameStyle, true /* fixStartDigit */ , true /* fixKeywords */ )
                                                        , item.isArray() ? item.getArraySize() : std::uint64_t(-1)
                                                        , indent, typeFieldWidth, nameFieldWidth
                                                        );
    oss << std::string(nameExtraSpace, ' ') << " // " << item.addressRange.start;
    if (item.addressRange.start!=item.addressRange.end)
        oss << " - " << item.addressRange.end;
}

//----------------------------------------------------------------------------
// Простой вывод, для тестов
template<typename StreamType, typename TokenCollectionItemType>
void simplePrintCppPacketDiagram( StreamType &oss
                                , const PacketDiagram<TokenCollectionItemType> &diagram
                                , bool skipNormalRecords       = false // print only fill and struct
                                , bool printOrgsAsStructFields = false
                                , bool printOrgsComment        = false
                                , bool fCpp=true
                                , marty_cpp::NameStyle itemStyle=marty_cpp::NameStyle::camelStyle
                                , marty_cpp::NameStyle structStyle=marty_cpp::NameStyle::pascalStyle
                                , std::size_t indent=0
                                , std::size_t itemIndent=4
                                , std::size_t typeFieldWidth=16
                                , std::size_t nameFieldWidth=24
                                )
{
    std::string structName = marty_cpp::formatName( diagram.getCppOrCTitle(fCpp), structStyle, true /* fixStartDigit */ , true /* fixKeywords */ );
    auto indentStr = std::string(indent, ' ');
    oss << indentStr << "struct " << structName << "\n";
    oss << indentStr << "{\n";

    for(auto &&item : diagram.data)
    {
        if (item.itemType==EPacketDiagramItemType::org)
        {
            if (printOrgsAsStructFields)
            {
                std::string fieldStructName = item.text;
                if (!item.reusedOrg.empty())
                    fieldStructName = item.reusedOrg;
                oss << indentStr;
                std::size_t nameExtraSpace = simplePrintStructMember( oss
                                                                    , marty_cpp::formatName( fieldStructName, structStyle, true /* fixStartDigit */ , true /* fixKeywords */ )
                                                                    , marty_cpp::formatName( item.getCppOrCFieldName(fCpp), itemStyle, true /* fixStartDigit */ , true /* fixKeywords */ )
                                                                    ,  /* item.isArray() ? item.getArraySize() : */  std::uint64_t(-1)
                                                                    , itemIndent, typeFieldWidth, nameFieldWidth
                                                                    );
                // std::ostringstream strss;
                // strss << std::uppercase << std::hex << item.orgAddress;
                // ConstMemoryIteratorType createConstMemoryIterator(const PacketDiagramItemType &item, marty::mem::Memory *pMem = 0) const
                auto iter = diagram.createConstMemoryIterator(item);

                oss << std::string(nameExtraSpace, ' ') << " // ORG " << std::string(iter) << "\n"; // << "0x" << strss.str() << "\n";
            }
            else
            {
                if (printOrgsComment)
                {
                    oss << "\n";
                    auto subStructName = marty_cpp::formatName( item.getCppOrCFieldName(fCpp), structStyle, true /* fixStartDigit */ , true /* fixKeywords */ );
                    oss << indentStr << std::string(itemIndent, ' ') << "// " << subStructName << "\n";
                }
            }
            continue;
        }

        if (skipNormalRecords != item.fillEntry)
            continue;

        oss << indentStr;
        printPacketDiagramItem( oss, item, fCpp, itemStyle, itemIndent, typeFieldWidth, nameFieldWidth);
        oss << "\n";
    }

    oss << "\n";
    oss << indentStr << "}; // struct " << structName << "\n\n";

}

//----------------------------------------------------------------------------
template<typename StreamType, typename TokenCollectionItemType>
void printCppPacketDiagram( StreamType &oss
                          , const PacketDiagram<TokenCollectionItemType> &diagram
                          , bool fCpp=true
                          , marty_cpp::NameStyle itemStyle=marty_cpp::NameStyle::camelStyle
                          , marty_cpp::NameStyle structStyle=marty_cpp::NameStyle::pascalStyle
                          , std::size_t indent=0
                          , std::size_t itemIndent=4
                          , std::size_t typeFieldWidth=16
                          , std::size_t nameFieldWidth=24
                          )
{
    std::vector<PacketDiagram<TokenCollectionItemType>> simpleVec = diagram.splitToSimple();

    for(auto &&d : simpleVec)
    {
        simplePrintCppPacketDiagram( oss, d
                                   , false // !skipNormalRecords - print only fill and struct
                                   , false // !printOrgsAsStructFields
                                   , false // !printOrgsComment
                                   , fCpp
                                   , itemStyle
                                   , structStyle
                                   , indent
                                   , itemIndent
                                   , typeFieldWidth
                                   , nameFieldWidth
                                   );
        oss << "\n";
    }

    simplePrintCppPacketDiagram( oss, diagram
                               , true // skipNormalRecords - print only fill and struct
                               , true // printOrgsAsStructFields
                               , false // !printOrgsComment
                               , fCpp
                               , itemStyle
                               , structStyle
                               , indent
                               , itemIndent
                               , typeFieldWidth
                               , nameFieldWidth
                               );
}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
inline
std::string makeIntegralTypeTextDiagramRepresentation(std::uint64_t size, bool showByteBoundaries=false, bool bCompact=false)
{
    UMBA_ASSERT(size>=1 && size<=8);

    // auto byteBoundary = 8 * (bCompact?1u:2u);
    auto byteSize     = 8 * (bCompact?1u:2u);
    auto charsSize    = size * byteSize;
    // auto inc          = std::uint64_t(bCompact?1u:2u);
    std::string res = std::string(std::size_t(charsSize), ' ');

    if (showByteBoundaries)
    {
        for(auto i=0u; i<charsSize; i+=byteSize)
        {
            if (i)
                res[i-1] = '|';
        }
    }

    res.front() = '[';
    res.back()  = ']';

    return res;
}

// template<typename TokenCollectionItemType>
// std::string makePacketDiagramItemTextDiagram(const PacketDiagramItem<TokenCollectionItemType> &item, bool bCompact)
// {
//  
// }



//template<typename StreamType, typename TokenCollectionItemType>

// template<typename TokenCollectionItemType>
// struct PacketDiagramItem
// {
//     EPacketDiagramItemType          itemType = EPacketDiagramItemType::undefined;
//     EOrgType                        orgType  = EOrgType::undefined;
//     umba::tokenizer::payload_type   explicitTypeTokenId;
//     std::string                     text;
//     bool                            fillEntry     = false;
//     std::string                     reusedOrg;
//     bool                            emptyOrg      = false; // no data records after this
//     bool                            textGenerated = false; // text is auto-generated
//     bool                            asciiZet      = false; // can be used only with ranges or byte/char arrays
//  
//     Endianness                      endianness   = Endianness::undefined; // use project endianness or override endianness for this entry
//     AddressRange                    addressRange; // Если указан тип - вычисляем при добавлении, 
//     std::uint64_t                   orgAddress   = 0;
//     std::uint64_t                   arraySize    = std::uint64_t(-1); // работает только для явно заданного типа
//  
//     std::string                     realTypeName; // For C++ output, full qualified
//  
//     std::size_t                     checksumIndex = std::size_t(-1); // При разборе строки записи, если там нет опции CRC, то выходной параметр 
//                                                                 // ChecksumOptions не заполнен и не валиден, это поле содержит -1.
//                                                                 // Если опции CRC корректно заданы, то это поле устанавливается в 0,
//                                                                 // и парсер должен добавить опции CRC в отдельный массив, а сюда поместить
//                                                                 // реальный индекс.
//  
//     const TokenCollectionItemType   *pTokenInfo = 0; // стартовый токен, можно получить номер строки



/*

    Отображение структуры в виде текста

    Каждый бит в байте (слове int64) мы хотим как минимум пронумеровать. Или нет?

    Отдельный byte/word/dword/qword представляем как 8/16/32/64 пробела, первый и последний символы -
    '[' и ']' соответственно.

    word/dword/qword разделяем на байты символом '|' на каждой седьмой позиции.

    Имя поля накладываем поверх, выравнивая по центру. При этом разделители байтов в словах могут быть перезатёрты.
    Имя поля, если не влезает - обрезаем.

    Если есть перенос слова на новую строку, и в первом символе пробел - ставим '|'. Также при переносе, если в 
    последнем символе первой строки пробел - тоже ставим '|'.

    Над каждым началом и концом byte/word/dword/qword печатаем его индекс (отдельная строка).
    Но сначала надо научится выводить нормально без доп инфы.

    Как быть с массивами?
    Если массив байт - печатаем, как слово произвольной длины, с тем же разделением на байты, и тем же способом вывода метки.
    Если массив элементов с битовым размером больше 8ми - определяем размерность, сколько символов надо для вывода номера 
    индекса (dec формат), в каждом слове выводим имя поля + индекс, обрезку имени делаем до добавления индекса.

    А если имя уже содержит цифры в конце? Выводим через подчеркивание? На байтовое поле итак 6 символов в компакт варианте

    ---

    Варианты - compact - один бит - один пробел, и normal - один бит - два пробела. 

    В normal варианте влезет в два раза меньше полей по ширине, но зато имена полей будут обрезаться меньше.
    Для печатного документа 64 символа на строку - это макс. Соответственно, в normal режиме в строку будет влезать
    только 4 байта. Но оригинальная marmaid packet diagram тоже рисует по 32 бита на строку, так что вроде норм.

    В compact варианте влезет 64 бита в строку, и его можно использовать, если для байтовых полей использовать короткие имена.

    Варианты отображения - имя поля впечатывается прямо в строку "формата" поля, и тогда оно перекрывает внутреннюю структуру 
    многобайтных полей.

    Если же у нас отображение многострочное, то имя можно выводить сверху


*/







} // namespace cpp
} // namespace marmaid
} // namespace tokenizer
} // namespace umba

