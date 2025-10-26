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
    //std::string     className   ; // Owning FSM

}; // struct TypeValueInfo

//----------------------------------------------------------------------------



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

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
UMBA_TOKENIZER_UFSM_PARSER_DECLARE_EXCEPTION_CLASS_BEGIN(base_error, std::runtime_error)

    void assignExtra(const base_error &) {}

    static std::string makeSimpleDeclInfoString(const TypeValueInfo &declInfo, const std::string &strExtra)
    {
        std::string res = declInfo.typeName + " '" + declInfo.name + "'";
        if (!strExtra.empty())
            res += " " + strExtra;
        return res;
    }

UMBA_TOKENIZER_UFSM_PARSER_DECLARE_EXCEPTION_CLASS_END(base_error, std::runtime_error)

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
UMBA_TOKENIZER_UFSM_PARSER_DECLARE_EXCEPTION_CLASS_BEGIN(already_error_base, base_error)

    void assignExtra(const already_error_base &e)
    {
        declInfoPrev = e.declInfoPrev;
        declInfoCur  = e.declInfoCur ;
    }

    already_error_base(const TypeValueInfo &prevDeclInfo_, const TypeValueInfo &curDeclInfo_, const std::string &strExtra_=std::string())
    : base_error(makeSimpleDeclInfoString(curDeclInfo_, strExtra_))
    , declInfoPrev(prevDeclInfo_)
    , declInfoCur (curDeclInfo_ )
    {}

    TypeValueInfo  declInfoPrev;
    TypeValueInfo  declInfoCur ;

UMBA_TOKENIZER_UFSM_PARSER_DECLARE_EXCEPTION_CLASS_END(already_error_base, base_error)

//----------------------------------------------------------------------------
UMBA_TOKENIZER_UFSM_PARSER_DECLARE_EXCEPTION_CLASS_BEGIN(already_declared_error, already_error_base)

    void assignExtra(const already_declared_error &) {}

    already_declared_error(const TypeValueInfo &prevDeclInfo_, const TypeValueInfo &curDeclInfo_)
    : already_error_base(prevDeclInfo_, curDeclInfo_, "already declared")
    {}

UMBA_TOKENIZER_UFSM_PARSER_DECLARE_EXCEPTION_CLASS_END(already_declared_error, already_error_base)

//----------------------------------------------------------------------------
UMBA_TOKENIZER_UFSM_PARSER_DECLARE_EXCEPTION_CLASS_BEGIN(already_defined_error, already_error_base)

    void assignExtra(const already_defined_error &) {}

    already_defined_error(const TypeValueInfo &prevDeclInfo_, const TypeValueInfo &curDeclInfo_)
    : already_error_base(prevDeclInfo_, curDeclInfo_, "already defined")
    {}

UMBA_TOKENIZER_UFSM_PARSER_DECLARE_EXCEPTION_CLASS_END(already_defined_error, already_error_base)

//----------------------------------------------------------------------------
UMBA_TOKENIZER_UFSM_PARSER_DECLARE_EXCEPTION_CLASS_BEGIN(already_used_error, already_error_base)

    void assignExtra(const already_used_error &) {}

    already_used_error(const TypeValueInfo &prevDeclInfo_, const TypeValueInfo &curDeclInfo_)
    : already_error_base(prevDeclInfo_, curDeclInfo_, "already used")
    {}

UMBA_TOKENIZER_UFSM_PARSER_DECLARE_EXCEPTION_CLASS_END(already_used_error, already_error_base)

//----------------------------------------------------------------------------
UMBA_TOKENIZER_UFSM_PARSER_DECLARE_EXCEPTION_CLASS_BEGIN(already_inherited_error, already_error_base)

    void assignExtra(const already_inherited_error &) {}

    already_inherited_error(const TypeValueInfo &prevDeclInfo_, const TypeValueInfo &curDeclInfo_)
    : already_error_base(prevDeclInfo_, curDeclInfo_, "already in inherits/uses list")
    {}

UMBA_TOKENIZER_UFSM_PARSER_DECLARE_EXCEPTION_CLASS_END(already_inherited_error, already_error_base)

//----------------------------------------------------------------------------
UMBA_TOKENIZER_UFSM_PARSER_DECLARE_EXCEPTION_CLASS_BEGIN(looking_for_error, already_error_base)

    void assignExtra(const looking_for_error &) {}

    looking_for_error(const TypeValueInfo &prevDeclInfo_, const TypeValueInfo &curDeclInfo_)
    : already_error_base(prevDeclInfo_, curDeclInfo_, "looking for, but found something else")
    {}

UMBA_TOKENIZER_UFSM_PARSER_DECLARE_EXCEPTION_CLASS_END(looking_for_error, already_error_base)

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
UMBA_TOKENIZER_UFSM_PARSER_DECLARE_EXCEPTION_CLASS_BEGIN(not_found_error_base, base_error)

    void assignExtra(const not_found_error_base &e)
    {
        declInfo  = e.declInfo;
    }

    not_found_error_base(const TypeValueInfo &declInfo_, const std::string &strExtra_=std::string())
    : base_error(makeSimpleDeclInfoString(declInfo_, strExtra_))
    , declInfo(declInfo_)
    {}

    TypeValueInfo  declInfo;

UMBA_TOKENIZER_UFSM_PARSER_DECLARE_EXCEPTION_CLASS_END(not_found_error_base, base_error)

//----------------------------------------------------------------------------
UMBA_TOKENIZER_UFSM_PARSER_DECLARE_EXCEPTION_CLASS_BEGIN(state_not_found, not_found_error_base)

    void assignExtra(const state_not_found &) {}

    state_not_found(const TypeValueInfo &declInfo_)
    : not_found_error_base(declInfo_, "state not found")
    {}

UMBA_TOKENIZER_UFSM_PARSER_DECLARE_EXCEPTION_CLASS_END(state_not_found, not_found_error_base)

//----------------------------------------------------------------------------
UMBA_TOKENIZER_UFSM_PARSER_DECLARE_EXCEPTION_CLASS_BEGIN(event_not_found, not_found_error_base)

    void assignExtra(const event_not_found &) {}

    event_not_found(const TypeValueInfo &declInfo_)
    : not_found_error_base(declInfo_, "event not found")
    {}

UMBA_TOKENIZER_UFSM_PARSER_DECLARE_EXCEPTION_CLASS_END(event_not_found, not_found_error_base)

//----------------------------------------------------------------------------
UMBA_TOKENIZER_UFSM_PARSER_DECLARE_EXCEPTION_CLASS_BEGIN(state_not_found_in_dead, not_found_error_base)

    void assignExtra(const state_not_found_in_dead &) {}

    state_not_found_in_dead(const TypeValueInfo &declInfo_)
    : not_found_error_base(declInfo_, "state not found in dead states")
    {}

UMBA_TOKENIZER_UFSM_PARSER_DECLARE_EXCEPTION_CLASS_END(state_not_found_in_dead, not_found_error_base)

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
} // namespace ufsm
} // namespace tokenizer
} // namespace umba
// umba::tokenizer::ufsm::


