/*! \file
    \brief Хклперы для вывода в сишечку и плюсики
 */


#pragma once

#include "marmaid_packet_diagram_parser.h"
#include "marty_cpp/marty_cpp.h"
//

#include <string>


// umba::tokenizer::marmaid::cpp::
namespace umba {
namespace tokenizer {
namespace marmaid {
namespace cpp {



template<typename StreamType, typename TokenCollectionItemType>
void printPacketDiagramItem( StreamType &oss
                           , const PacketDiagramItem<TokenCollectionItemType> &item
                           , bool fCpp=true
                           , marty_cpp::NameStyle nameStyle=marty_cpp::NameStyle::camelStyle
                           , std::size_t indent=4
                           , std::size_t width=24
                           )
{
    oss << std::string(indent, ' ');

    std::string typeName = item.getCppOrCTypeName(fCpp);

    std::size_t nExtraSpace = 0;
    if (typeName.size()<width)
        nExtraSpace = width - typeName.size();

    oss << typeName << std::string(nExtraSpace, ' ');

    
    oss << " " << marty_cpp::formatName( item.getCppOrCFieldName(fCpp), nameStyle, true /* fixStartDigit */ , true /* fixKeywords */ );

    if (item.isArray())
        oss << "[" << item.getArraySize() << "]";

    oss << ";";
}

template<typename StreamType, typename TokenCollectionItemType>
void printCppPacketDiagram( StreamType &oss
                          , const PacketDiagram<TokenCollectionItemType> &diagram
                          , bool fCpp=true
                          , marty_cpp::NameStyle itemStyle=marty_cpp::NameStyle::camelStyle
                          , marty_cpp::NameStyle structStyle=marty_cpp::NameStyle::pascalStyle
                          , std::size_t indent=0
                          , std::size_t itemIndent=4
                          , std::size_t width=24
                          )
{
    std::string structName = marty_cpp::formatName( diagram.getCppOrCTitle(fCpp), structStyle, true /* fixStartDigit */ , true /* fixKeywords */ );
    auto indentStr = std::string(indent, ' ');
    oss << indentStr << "struct " << structName << "\n";
    oss << indentStr << "{\n";

    for(auto &&item : diagram.data)
    {
        oss << indentStr;
        printPacketDiagramItem( oss, item, fCpp, itemStyle, itemIndent, width);
        oss << "\n";
    }

    oss << "\n";
    oss << indentStr << "}; // struct " << structName << "\n\n";

}



} // namespace cpp
} // namespace marmaid
} // namespace tokenizer
} // namespace umba

