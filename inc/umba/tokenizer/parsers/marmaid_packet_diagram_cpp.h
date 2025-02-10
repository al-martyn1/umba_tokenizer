/*! \file
    \brief Хелперы для вывода в сишечку и плюсики
 */


#pragma once

#include "marmaid_packet_diagram_parser.h"
#include "marty_cpp/marty_cpp.h"
//

#include <string>
#include <sstream>
#include <iomanip>


// umba::tokenizer::marmaid::cpp::
namespace umba {
namespace tokenizer {
namespace marmaid {
namespace cpp {



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
                std::ostringstream strss;
                strss << std::uppercase << std::hex << item.orgAddress;

                oss << std::string(nameExtraSpace, ' ') << " // ORG " << "0x" << strss.str() << "\n";
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

#if 1
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

// std::vector<PacketDiagram> splitToSimple() const

#endif



} // namespace cpp
} // namespace marmaid
} // namespace tokenizer
} // namespace umba

