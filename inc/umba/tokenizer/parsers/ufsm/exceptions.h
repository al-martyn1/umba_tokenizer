/*! \file
    \brief Исключения для библиотеки ufsm
 */

#pragma once

#include "basic_typedefs.h"

//
#include <exception>
#include <stdexcept>
#include <string>
//
#include "../../../undef_min_max.h"


//----------------------------------------------------------------------------
// umba::tokenizer::ufsm::
namespace umba {
namespace tokenizer {
namespace ufsm {

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
struct TypeValueInfo
{
    PositionInfo    positionInfo;
    std::string     name        ;
    std::string     typeName    ; // In DSL terms

}; // struct TypeValueInfo

//----------------------------------------------------------------------------



#define UMBA_TOKENIZER_UFSM_PARSER_DECLARE_EXCEPTION_CLASS_BEGIN(cls, clsBase)    \
             class cls : public clsBase                                           \
             {                                                                    \
             public:                                                              \
                                                                                  \
                 cls(const std::string &what_arg) : clsBase(what_arg) {}          \
                 cls(const char* what_arg       ) : clsBase(what_arg) {}          \
                 cls(const cls &e)                : clsBase(e) {}                 \
                 cls& operator=(const cls& e)     { clsBase::operator=(e); assignExtra(e); return *this; }


#define UMBA_TOKENIZER_UFSM_PARSER_DECLARE_EXCEPTION_CLASS_END(cls, clsBase)      \
             };


UMBA_TOKENIZER_UFSM_PARSER_DECLARE_EXCEPTION_CLASS_BEGIN(base_error, std::runtime_error)
    void assignExtra(const base_error &) {}
UMBA_TOKENIZER_UFSM_PARSER_DECLARE_EXCEPTION_CLASS_END(base_error, std::runtime_error)


UMBA_TOKENIZER_UFSM_PARSER_DECLARE_EXCEPTION_CLASS_BEGIN(already_declared_error, base_error)

    void assignExtra(const base_error &) {}

    static std::string makeSimpleDeclInfoString(const TypeValueInfo &declInfo, const std::string &strExtra)
    {
        std::string res = declInfo.typeName + " '" + declInfo.name + "' already declared";
        if (!strExtra.empty())
            res += " " + strExtra; // As sample: "in..."
        return res;
    }

    already_declared_error(const TypeValueInfo &prevDeclInfo_, const TypeValueInfo &curDeclInfo_, const std::string &strExtra_=std::string())
    : base_error(makeSimpleDeclInfoString(curDeclInfo_, strExtra_))
    , prevDeclInfo(prevDeclInfo_)
    , curDeclInfo (curDeclInfo_ )
    , strExtra    (strExtra_)
    {}

    TypeValueInfo  prevDeclInfo;
    TypeValueInfo  curDeclInfo ;
    std::string    strExtra    ;


UMBA_TOKENIZER_UFSM_PARSER_DECLARE_EXCEPTION_CLASS_END(already_declared_error, base_error)








//----------------------------------------------------------------------------
} // namespace ufsm
} // namespace tokenizer
} // namespace umba
// umba::tokenizer::ufsm::


