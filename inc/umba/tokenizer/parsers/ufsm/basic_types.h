/*! \file
    \brief Базовые типы для парсера ufsm
 */

#pragma once

#include "basic_typedefs.h"

//
#include "../../../undef_min_max.h"


//----------------------------------------------------------------------------
// umba::tokenizer::ufsm::
namespace umba {
namespace tokenizer {
namespace ufsm {

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
struct EventDefinition
{
    PositionInfo    positionInfo;
    std::string     name        ;
    std::string     description ;

    EventFlags      flags = EventFlags::none; // none, external, generated

    /*! Список событий, составляющих данное (если событие не external
        - не установлен флаг EventFlags::external) */
    std::vector<std::string>   basicEvents ; 


public: // methods

    std::string getCanonicalName() const { return name; }

}; // struct EventDefinition

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
struct ActionDefinition
{
    PositionInfo    positionInfo;
    std::string     name        ;
    std::string     description ;

    ActionFlags     flags = ActionFlags::none; // none, external, generates

    /*! Список действий, составляющих данное действие (если действие не 
        external - не установлен флаг ActionFlags::external) */
    std::vector<std::string>   basicActions;

    /*! Список список событий, которые может генерировать данное действие
        (флаг ActionFlags::generates) */
    std::vector<std::string>   generates   ; 


public: // methods

    std::string getCanonicalName() const { return name; }

}; // struct ActionDefinition

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
struct PredicateDefinition
{
    PositionInfo    positionInfo;
    std::string     name        ;
    std::string     description ;

    PredicateFlags  flags = PredicateFlags::none; // none, external, validFor

    /*! Выражение для пользовательского (не external)  предиката */
    LogicExpression expression  ;

    /*! Список событий, для которых предикат может быть использован. 
        Актуален только если установлен флаг PredicateFlags::validFor */
    std::vector<std::string>   validForList; 

    std::string getCanonicalName() const { return name; }

}; // struct PredicateDefinition

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
struct TransitionSourceState
{
    PositionInfo    positionInfo;
    std::string     name; // state name, not used if `any` flag is set

    // none, any, excluded
    TransitionSourceStateFlags flags = TransitionSourceStateFlags::none;


    std::string getCanonicalName() const;

    int compare(const TransitionSourceState&other) const;
    bool isEqual   (const TransitionSourceState &other) const;

    bool operator< (const TransitionSourceState &other) const;
    bool operator==(const TransitionSourceState &other) const;
    bool operator!=(const TransitionSourceState &other) const;

    bool isAny() const;
    bool isExcluded() const;

}; // struct TransitionSourceState

//------------------------------
struct TransitionSourceStates
{
    std::vector<TransitionSourceState>  stateList;

    std::string getCanonicalName() const;

    int compare(const TransitionSourceStates &other) const;
    bool isEqual(const TransitionSourceStates &other) const;

    bool operator< (const TransitionSourceStates &other) const;
    bool operator==(const TransitionSourceStates &other) const;
    bool operator!=(const TransitionSourceStates &other) const;

    void append   ( const TransitionSourceState &st);
    void push_back( const TransitionSourceState &st);

    std::vector<TransitionSourceState> getSortedStates() const;

    // returns true if no multiple ANYs found
    bool checkForAny(bool *pHasNormalAny=0, bool *pHasExcludedAny=0) const;

}; // struct TransitionSourceStates

//----------------------------------------------------------------------------
struct TransitionEvent
{
    PositionInfo    positionInfo;
    std::string     name        ; // event name, not used if `any`

    // none, any, excluded
    TransitionEventFlags  flags = TransitionEventFlags::none;


    std::string getCanonicalName() const;

    int compare(const TransitionEvent &other) const;
    bool isEqual   (const TransitionEvent &other) const;

    bool operator< (const TransitionEvent &other) const;
    bool operator==(const TransitionEvent &other) const;
    bool operator!=(const TransitionEvent &other) const;

    bool isAny() const;
    bool isExcluded() const;

}; // struct TransitionEvent

//------------------------------
struct TransitionEvents
{
    std::vector<TransitionEvent>  eventList;

    std::string getCanonicalName() const;

    int compare(const TransitionEvents &other) const;
    bool isEqual(const TransitionEvents &other) const;

    bool operator< (const TransitionEvents &other) const;
    bool operator==(const TransitionEvents &other) const;
    bool operator!=(const TransitionEvents &other) const;

    void append   ( const TransitionEvent &st);
    void push_back( const TransitionEvent &st);

    std::vector<TransitionEvent> getSortedEvents() const;

    // returns true if no multiple ANYs found
    bool checkForAny(bool *pHasNormalAny=0, bool *pHasExcludedAny=0) const;

}; // struct TransitionEvents

//----------------------------------------------------------------------------


/*
  Тут уже можно описать структуру TransitionDefinition, но у нас пока нет подходящего контейнера 

*/



//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
} // namespace ufsm
} // namespace tokenizer
} // namespace umba
// umba::tokenizer::ufsm::


#include "impl/basic_types.h"


