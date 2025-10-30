/*! \file
    \brief Базовые типы для парсера ufsm
 */

#pragma once

#include "umba/types/FullQualifiedName.h"
//
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
struct FullQualifiedName : public umba::types::FullQualifiedName<std::string>
{

public: // types

    using BaseType        = umba::types::FullQualifiedName<std::string>;
    using base_type       = umba::types::FullQualifiedName<std::string>;

    // using Scheme          = BaseType::Scheme;

    using iterator        = typename BaseType::iterator       ;
    using const_iterator  = typename BaseType::const_iterator ;
    using reference       = typename BaseType::reference      ;
    using const_reference = typename BaseType::const_reference;


public: // fields


    PositionInfo               positionInfo;

    static inline std::string namespaceSeparator = "::"; // Можно переопределять, но только глобально

public: // ctors

    UMBA_RULE_OF_FIVE(FullQualifiedName, default, default, default, default, default);

    FullQualifiedName(Scheme sch) : BaseType(sch) {}

    FullQualifiedName(Scheme sch, const StringType &p1) : BaseType(sch, p1) {}
    FullQualifiedName(const StringType &p1)             : BaseType(p1) {}

    FullQualifiedName(Scheme sch, std::initializer_list<StringType> pathParts) : BaseType(sch, pathParts) {}
    FullQualifiedName(std::initializer_list<StringType> pathParts)             : BaseType(pathParts) {}


public: // methods

    FullQualifiedName& makeRelative() { BaseType::makeRelative(); return *this; }
    FullQualifiedName& makeAbsolute() { BaseType::makeAbsolute(); return *this; }

    FullQualifiedName toRelative() const { auto res = *this; return res.makeRelative(); }
    FullQualifiedName toAbsolute() const { auto res = *this; return res.makeAbsolute(); }

    FullQualifiedName& removeTail(std::size_t nItems=1u) { BaseType::removeTail(nItems); return *this; }
    FullQualifiedName& removeHead(std::size_t nItems=1u) { BaseType::removeHead(nItems); return *this; }

    // Возвращает хвост, удаляя голову. Также меняет схему на relative
    FullQualifiedName getTail() const
    {
        if (empty())
            throw std::runtime_error("umba::tokenizer::ufsm::FullQualifiedName::getTail: can't get tail from empty name");

        auto res = *this;
        return res.removeHead(1);
    }

    // Возвращает голову (всё оставшееся), удаляя хвост (последний элемент). Не меняет scheme
    FullQualifiedName getHead   () const
    {
        if (empty())
            throw std::runtime_error("umba::tokenizer::ufsm::FullQualifiedName::getHead: can't get head from empty name");
    
        auto res = *this;
        return res.removeTail(1);
    }

    FullQualifiedName& operator+=(const StringType &n)        { append(n); return *this; }
    FullQualifiedName& operator/=(const StringType &n)        { append(n); return *this; }
    FullQualifiedName& operator+=(const FullQualifiedName &n) { append(n); return *this; }
    FullQualifiedName& operator/=(const FullQualifiedName &n) { append(n); return *this; }

    FullQualifiedName  operator+(const StringType &n)        { FullQualifiedName res = *this; res.append(n); return res; }
    FullQualifiedName  operator/(const StringType &n)        { FullQualifiedName res = *this; res.append(n); return res; }
    FullQualifiedName  operator+(const FullQualifiedName &n) { FullQualifiedName res = *this; res.append(n); return res; }
    FullQualifiedName  operator/(const FullQualifiedName &n) { FullQualifiedName res = *this; res.append(n); return res; }

    std::string getCanonicalName() const
    {
        return toString(namespaceSeparator);
    }


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

    std::size_t     id = 0;

public: // methods

    std::string getCanonicalName() const { return name; }

    bool addActionRef(StateActionKind ak, const std::string &actionName);

    template<typename StreamType>
    StreamType& print(StreamType& oss, std::size_t indendSize) const;

    bool isInitial() const { return (flags&StateFlags::initial)!=0; }
    bool isFinal()   const { return (flags&StateFlags::final  )!=0; }
    bool isError()   const { return (flags&StateFlags::error  )!=0; }

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


    using iterator        = typename std::vector<TransitionSourceState>::iterator       ;
    using const_iterator  = typename std::vector<TransitionSourceState>::const_iterator ;
    using reference       = typename std::vector<TransitionSourceState>::reference      ;
    using const_reference = typename std::vector<TransitionSourceState>::const_reference;

    iterator        begin()       { return list.begin (); }
    iterator        end  ()       { return list.end   (); }
    const_iterator  begin() const { return list.begin (); }
    const_iterator  end  () const { return list.end   (); }
    const_iterator cbegin() const { return list.cbegin(); }
    const_iterator cend  () const { return list.cend  (); }

    reference       front()       { return list.front(); }
    const_reference front() const { return list.front(); }
    reference       back()        { return list.back() ; }
    const_reference back() const  { return list.back() ; }


    int getWeightForCompareForPrioritySort() const;
    //int compareForPrioritySort(const TransitionSourceStates &other) const;
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
    void        clear()       { list.clear(); }

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


    using iterator        = typename std::vector<TransitionEvent>::iterator       ;
    using const_iterator  = typename std::vector<TransitionEvent>::const_iterator ;
    using reference       = typename std::vector<TransitionEvent>::reference      ;
    using const_reference = typename std::vector<TransitionEvent>::const_reference;

    iterator        begin()       { return list.begin (); }
    iterator        end  ()       { return list.end   (); }
    const_iterator  begin() const { return list.begin (); }
    const_iterator  end  () const { return list.end   (); }
    const_iterator cbegin() const { return list.cbegin(); }
    const_iterator cend  () const { return list.cend  (); }


    int getWeightForCompareForPrioritySort() const;
    //int compareForPrioritySort(const TransitionEvents &other) const;
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
    void        clear()       { list.clear(); }

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
    TransitionFlags            flags       ;
    LogicExpression            additionalCondition;
    std::string                targetState ;
    TransitionActionRefs       actionRefs  ;

    std::size_t                id = 0;


    std::string additionalConditionAsString() const;
    std::string eventsAsString() const;

    std::string getCanonicalName() const;

    bool hasAnyAny() const; // has any in sourceStates or in events
    bool hasAdditionalCondition() const;
    int compareAdditionalCondition(const TransitionDefinition &other) const;
    int getWeightForCompareForPrioritySort() const;
    int comparePrerequisites(const TransitionDefinition &other) const;
    bool isEqualPrerequisites(const TransitionDefinition &other) const;
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

    std::vector<TransitionDefinition> expandEvents(const std::vector<std::string> &allEventNames) const;
    std::vector<TransitionDefinition> expandSourceStates(const std::vector<std::string> &allSourceStateNames) const;
    std::vector<TransitionDefinition> expandEventsAndSourceStates(const std::vector<std::string> &allEventNames, const std::vector<std::string> &allSourceStateNames) const;

    std::string getSourceState() const;
    std::string getTargetState() const;

    bool isSelfTarget() const { return (flags&TransitionFlags::selfTarget)!=0; }
    

    

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


