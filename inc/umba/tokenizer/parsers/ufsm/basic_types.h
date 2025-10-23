/*! \file
    \brief Базовые типы для парсера ufsm
 */

#pragma once

#include "basic_typedefs.h"
#include "exceptions.h"
//
#include <unordered_map>
#include <initializer_list>

#include "../../../undef_min_max.h"

//
#include <exception>
#include <stdexcept>


//----------------------------------------------------------------------------
// umba::tokenizer::ufsm::
namespace umba {
namespace tokenizer {
namespace ufsm {

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
struct FullQualifiedName
{

public: // types

    enum class PathType
    {
        relative,
        absolute
    };

public: // fields


    PositionInfo               positionInfo;

    FullQualifiedNameFlags     flags = FullQualifiedNameFlags::none; // absolute
    std::vector<std::string>   name; 

    using iterator        = typename std::vector<std::string>::iterator      ;
    using const_iterator  = typename std::vector<std::string>::const_iterator;


    static inline std::string namespaceSeparator = "::"; // Можно переопределять, но только глобально

public: // ctors

    UMBA_RULE_OF_FIVE(FullQualifiedName, default, default, default, default, default);

    FullQualifiedName(PathType pt, std::initializer_list<std::string> pathParts)
    : positionInfo()
    , flags(pt==PathType::absolute ? FullQualifiedNameFlags::absolute : FullQualifiedNameFlags::absolute)
    , name(pathParts.begin(), pathParts.end())
    {}

    FullQualifiedName(std::initializer_list<std::string> pathParts)
    : positionInfo()
    , flags(FullQualifiedNameFlags::absolute)
    , name(pathParts.begin(), pathParts.end())
    {}


public: // methods

    std::size_t size() const  { return name.size(); }
    bool empty() const { return name.empty(); }
    std::string& front() { return name.front(); }
    const std::string& front() const { return name.front(); }

    iterator        begin()       { return name.begin (); }
    iterator        end  ()       { return name.end   (); }
    const_iterator  begin() const { return name.begin (); }
    const_iterator  end  () const { return name.end   (); }
    const_iterator cbegin() const { return name.cbegin(); }
    const_iterator cend  () const { return name.cend  (); }

    bool isAbsolute() const;
    FullQualifiedName toRelative() const;
    FullQualifiedName toAbsolute() const;
    FullQualifiedName getTail   () const;
    FullQualifiedName getHead   () const;
    void tailRemove(std::size_t nItems);
    void clear();
    void append(const std::string &n);
    void append(const FullQualifiedName &n);

    std::string getCanonicalName() const;

}; // struct FullQualifiedName
//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
struct ParentListEntry
{
    PositionInfo               positionInfo;
    FullQualifiedName          name;

    ParentListEntryFlags       flags = ParentListEntryFlags::none;
    InheritanceOverrideFlags   overrideFlags = InheritanceOverrideFlags::none;

public: // methods

    std::string getCanonicalName() const;

}; // struct ParentListEntry
//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
struct EventDefinition
{
    PositionInfo    positionInfo;
    std::string     name        ;
    std::string     description ;

    EventFlags      flags = EventFlags::none;

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

    ActionFlags     flags = ActionFlags::none;

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

    PredicateFlags  flags = PredicateFlags::none;

    /*! Выражение для пользовательского (не external) предиката */
    LogicExpression expression  ; // всегда установлено для пользовательского предиката
    //LogicExpression expressionSmp; // simplified

    /*! Список событий, для которых предикат может быть использован. 
        Актуален только если установлен флаг PredicateFlags::validFor */
    std::vector<std::string>          validForList; 
    std::unordered_set<std::uint8_t>  charSet;

    std::string getCanonicalName() const { return name; }

}; // struct PredicateDefinition

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
struct StateActionRefs
{
    std::vector<std::string>    list;

    bool appendImpl(const std::string &actionName);
    bool append    (const std::string &actionName);
    bool push_back (const std::string &actionName);
    bool empty() const;
    std::size_t size() const;

}; // struct StateActionRefs

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
struct StateDefinition
{
    PositionInfo    positionInfo;
    std::string     name        ;
    std::string     description ;

    StateFlags      flags = StateFlags::none;

    std::unordered_map<StateActionKind, StateActionRefs>   actionRefs; 


public: // methods

    std::string getCanonicalName() const { return name; }

    bool addActionRef(StateActionKind ak, const std::string &actionName);

    template<typename StreamType>
    StreamType& print(StreamType& oss, std::size_t indendSize) const;

}; // struct StateDefinition

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
using TransitionActionRefs = StateActionRefs;

//----------------------------------------------------------------------------
struct TransitionSourceState
{
    PositionInfo    positionInfo;
    std::string     name; // state name, not used if `any` flag is set

    TransitionSourceStateFlags flags = TransitionSourceStateFlags::none;


    std::string getCanonicalName() const;

    int compare(const TransitionSourceState&other) const;
    bool isEqual   (const TransitionSourceState &other) const;

    bool operator< (const TransitionSourceState &other) const;
    bool operator<=(const TransitionSourceState &other) const;
    bool operator> (const TransitionSourceState &other) const;
    bool operator>=(const TransitionSourceState &other) const;
    bool operator==(const TransitionSourceState &other) const;
    bool operator!=(const TransitionSourceState &other) const;

    bool isNameEqual(const TransitionSourceState &other) const;
    bool isAny() const;
    bool isExcluded() const;

}; // struct TransitionSourceState

//------------------------------
struct TransitionSourceStates
{
    std::vector<TransitionSourceState>  list;

    std::string getCanonicalName() const;

    int compare(const TransitionSourceStates &other) const;
    bool isEqual(const TransitionSourceStates &other) const;

    bool operator< (const TransitionSourceStates &other) const;
    bool operator<=(const TransitionSourceStates &other) const;
    bool operator> (const TransitionSourceStates &other) const;
    bool operator>=(const TransitionSourceStates &other) const;
    bool operator==(const TransitionSourceStates &other) const;
    bool operator!=(const TransitionSourceStates &other) const;

    void append   ( const TransitionSourceState &st);
    void push_back( const TransitionSourceState &st);
    std::size_t size () const { return list.size (); }
    bool        empty() const { return list.empty(); }

    std::vector<TransitionSourceState> getSortedStates() const;

    // returns true if any number of ANYs found
    bool checkForAny( bool *pHasNormalAny=0
                    , bool *pHasExcludedAny=0) const;

    std::size_t getAnyIndex() const;

}; // struct TransitionSourceStates

//----------------------------------------------------------------------------
struct TransitionEvent
{
    PositionInfo    positionInfo;
    std::string     name        ; // event name, not used if `any`

    TransitionEventFlags  flags = TransitionEventFlags::none;


    std::string getCanonicalName() const;

    int compare(const TransitionEvent &other) const;
    bool isEqual   (const TransitionEvent &other) const;

    bool operator< (const TransitionEvent &other) const;
    bool operator<=(const TransitionEvent &other) const;
    bool operator> (const TransitionEvent &other) const;
    bool operator>=(const TransitionEvent &other) const;
    bool operator==(const TransitionEvent &other) const;
    bool operator!=(const TransitionEvent &other) const;

    bool isNameEqual(const TransitionEvent &other) const;
    bool isAny() const;
    bool isExcluded() const;

}; // struct TransitionEvent

//------------------------------
struct TransitionEvents
{
    std::vector<TransitionEvent>  list;

    std::string getCanonicalName() const;

    int compare(const TransitionEvents &other) const;
    bool isEqual(const TransitionEvents &other) const;

    bool operator< (const TransitionEvents &other) const;
    bool operator<=(const TransitionEvents &other) const;
    bool operator> (const TransitionEvents &other) const;
    bool operator>=(const TransitionEvents &other) const;
    bool operator==(const TransitionEvents &other) const;
    bool operator!=(const TransitionEvents &other) const;

    void append   ( const TransitionEvent &st);
    void push_back( const TransitionEvent &st);
    std::size_t size () const { return list.size (); }
    bool        empty() const { return list.empty(); }

    std::vector<TransitionEvent> getSortedEvents() const;

    // returns true if any number of ANYs found
    bool checkForAny( bool *pHasNormalAny=0
                    , bool *pHasExcludedAny=0) const;

    std::size_t getAnyIndex() const;

}; // struct TransitionEvents

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
struct TransitionDefinition
{
    PositionInfo               positionInfo;
    std::string                description ;
    TransitionSourceStates     sourceStates; 
    TransitionEvents           events      ;
    TransitionFlags            flags;
    LogicExpression            additionalCondition;
    std::string                targetState ;
    TransitionActionRefs       actionRefs  ;


    std::string additionalConditionAsString() const;

    std::string getCanonicalName() const;

    int compare(const TransitionDefinition &other) const;
    bool isEqual(const TransitionDefinition &other) const;

    bool operator< (const TransitionDefinition &other) const;
    bool operator<=(const TransitionDefinition &other) const;
    bool operator> (const TransitionDefinition &other) const;
    bool operator>=(const TransitionDefinition &other) const;
    bool operator==(const TransitionDefinition &other) const;
    bool operator!=(const TransitionDefinition &other) const;

    void appendImpl(const TransitionSourceState &st);
    void appendImpl(const TransitionEvent &te);

    void append   ( const TransitionSourceState &st);
    void push_back( const TransitionSourceState &st);

    void append   ( const TransitionEvent &te);
    void push_back( const TransitionEvent &te);

}; // struct TransitionDefinition

//----------------------------------------------------------------------------




#if 0

    states : override // Перезаписывает то, что было определено в базе, но не в текущем автомате
    {

        turnedOff : initial - "Traffic Light is turned OFF"
        {
            enter { turnOff /*, startAliveTimer */ } // При входе в состояние всегда всё выключаем
        }

        trafficAllowed - "Informs pedestrians that they are can go"
        { enter { runTrafficAllowed } }

        // Светофор показыват уведомление, что скоро произойдёт смена сигнала на запрещающий
        stopNotice - "Informs pedestrians that the RED light is coming soon"
        { enter {runStopNotice} }

        trafficStopped - "Informs pedestrians that the traffic must stops"
        { enter { runTrafficStopped } }

    }

#endif


//----------------------------------------------------------------------------
TypeValueInfo makeTypeValueInfo(const FullQualifiedName &d);
TypeValueInfo makeTypeValueInfo(const ParentListEntry &d);
TypeValueInfo makeTypeValueInfo(const EventDefinition &d);
TypeValueInfo makeTypeValueInfo(const ActionDefinition &d);
TypeValueInfo makeTypeValueInfo(const PredicateDefinition &d);
TypeValueInfo makeTypeValueInfo(const TransitionSourceState &d);
TypeValueInfo makeTypeValueInfo(const TransitionEvent &d);
TypeValueInfo makeTypeValueInfo(const TransitionSourceStates &d);
TypeValueInfo makeTypeValueInfo(const TransitionEvents &d);
TypeValueInfo makeTypeValueInfo(const TransitionDefinition &d);
// TypeValueInfo makeTypeValueInfo(const StateActionRefs &d);
TypeValueInfo makeTypeValueInfo(const StateDefinition &d);
//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
} // namespace ufsm
} // namespace tokenizer
} // namespace umba
// umba::tokenizer::ufsm::


#include "impl/basic_types.h"


