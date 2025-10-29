/*! \file
    \brief Типы для парсера ufsm
 */

#pragma once

#include "basic_types.h"

//
#include "marty_containers/insertion_ordered_map.h"

//
#include "../../../undef_min_max.h"

//
#include <functional>
#include <type_traits>
#include <variant>
#include <iterator>



//----------------------------------------------------------------------------
// umba::tokenizer::ufsm::
namespace umba {
namespace tokenizer {
namespace ufsm {

//----------------------------------------------------------------------------
struct StateMachineDefinition;
struct NamespaceDefinition;

//----------------------------------------------------------------------------
TypeValueInfo makeTypeValueInfo(const StateMachineDefinition &d);
TypeValueInfo makeTypeValueInfo(const NamespaceDefinition &d);

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------


template<typename ValueType>
using insertion_ordered_map = marty::containers::insertion_ordered_map<std::string, ValueType>; // ContainerT, MapT - used default (vector and unordered_map)

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
struct StateMachineDefinition
{
    PositionInfo      positionInfo;
    std::string       name        ;
    std::string       description ;

    StateMachineFlags flags = StateMachineFlags::none; // none, stateMachine

    insertion_ordered_map<ParentListEntry     >    parents    ;

    insertion_ordered_map<EventDefinition     >    events     ;
    insertion_ordered_map<ActionDefinition    >    actions    ;
    insertion_ordered_map<PredicateDefinition >    predicates ;
    insertion_ordered_map<StateDefinition     >    states     ;
    insertion_ordered_map<TransitionDefinition>    transitions;


protected: // methods

    // Данный метод проверяет только существование определения в рамках текущего определения FSM
    template< typename ErrorType
            , typename ValueType
            >
    void addDefinitionImpl(const ValueType &v, insertion_ordered_map<ValueType> &holder)
    {
        auto canonicalName = v.getCanonicalName();
        auto it = holder.find(canonicalName);
        if (it!=holder.end())
        {
            throw ErrorType(makeTypeValueInfo(it->second), makeTypeValueInfo(v));
        }

        holder[canonicalName] = v;
    }

public: // methods

    template<typename StreamType>
    StreamType& print(StreamType& oss, std::size_t indendSize=0) const;

    // Тут проблема только в том, что мы сначала читаем определение, разбираем в нём ошибки,
    // а потом - добавляем, и у нас может выскочить ошибка, что такое определение уже есть.
    // Т.е. мы сначала вылижем определение, а потом окажется, что оно уже есть.
    void addDefinition(const ParentListEntry      &v) { addDefinitionImpl<already_inherited_error>(v, parents    ); }
    void addDefinition(const EventDefinition      &v) { addDefinitionImpl<already_declared_error >(v, events     ); }
    void addDefinition(const ActionDefinition     &v) { addDefinitionImpl<already_declared_error >(v, actions    ); }
    void addDefinition(const PredicateDefinition  &v) { addDefinitionImpl<already_declared_error >(v, predicates ); }
    void addDefinition(const StateDefinition      &v) { addDefinitionImpl<already_declared_error >(v, states     ); }
    void addDefinition(const TransitionDefinition &v) { addDefinitionImpl<already_defined_error  >(v, transitions); }

    // Ещё нам надо будет потом мержить и наследовать

    std::string getCanonicalName() const { return name; }

    bool hasSamePrerequisitesTransition(const TransitionDefinition &tOther) const; // Сравниваются исх состояние, событие, доп условия. 

    std::vector<TransitionDefinition> getTransitionsVector() const;
    std::vector<TransitionDefinition> getPrioritySortedTransitions() const;
    void prioritySortTransitions();
    std::vector<std::string> getSourceStateNamesList(bool skipFinal=true) const;
    std::set<std::string> getSourceStateNamesSet(bool skipFinal=true) const;
    std::vector<std::string> getEventNamesList() const;
    std::set<std::string> getEventNamesSet() const;
    bool expandTransitions();
    std::vector<TransitionDefinition> makeExpandedTransitionsForGraph() const;


}; // struct StateMachineDefinition

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
struct NamespaceDefinition;


//----------------------------------------------------------------------------
using NamespaceEntry = std::variant<NamespaceDefinition, StateMachineDefinition>;
TypeValueInfo makeTypeValueInfo(const NamespaceEntry &d);

//------------------------------
std::string getNamespaceEntryName(const NamespaceEntry &e);
NamespaceEntryKind getNamespaceEntryKind(const NamespaceEntry &e);
std::string getNamespaceEntryKindString(NamespaceEntryKind k, StateMachineFlags flags);
std::string getNamespaceEntryKindString(NamespaceEntryKind k);
std::string getNamespaceEntryKindString(const NamespaceEntry &e);
PositionInfo getNamespaceEntryPositionInfo(const NamespaceEntry &e);
StateMachineFlags getNamespaceEntryStateMachineFlags(const NamespaceEntry &e);

template<typename StreamType>
StreamType& print(StreamType &oss, const NamespaceEntry &e, std::size_t indendSize=0u);


//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
struct NamespaceDefinition
{
    PositionInfo      positionInfo;
    std::string       name        ;

    insertion_ordered_map<NamespaceEntry>    members;

    using iterator               = typename insertion_ordered_map<NamespaceEntry>::iterator      ;
    using const_iterator         = typename insertion_ordered_map<NamespaceEntry>::const_iterator;


public: // methods

    template<typename StreamType>
    StreamType& print(StreamType& oss, std::size_t indendSize=0) const;

    iterator find(const std::string &n)
    {
        return members.find(n);
    }

    const_iterator find(const std::string &n) const
    {
        return members.find(n);
    }

    iterator        begin()       { return members.begin (); }
    iterator        end  ()       { return members.end   (); }
    const_iterator  begin() const { return members.begin (); }
    const_iterator  end  () const { return members.end   (); }
    const_iterator cbegin() const { return members.cbegin(); }
    const_iterator cend  () const { return members.cend  (); }

    bool findEntry(const FullQualifiedName &name_, NamespaceDefinition *pRootNs, NamespaceEntry **ppe = 0)
    {
        if (name_.isAbsolute() && pRootNs)
            return pRootNs->findEntry(name_.toRelative(), pRootNs, ppe);

        if (name_.isAbsolute())
            throw std::runtime_error("umba::tokenizer::ufsm::NamespaceDefinition::findEntry: can't get entry by absolute name");

        if (name_.empty())
            throw std::runtime_error("umba::tokenizer::ufsm::NamespaceDefinition::findEntry: can't get entry by empty name");

        auto it = find(name_.front());
        if (it==end())
            return false;

        if (name_.size()>1)
        {
            NamespaceEntryKind kind = getNamespaceEntryKind(it->second);
            if (kind!=NamespaceEntryKind::nsDefinition)
                return false;

            auto &foundNs = std::get<NamespaceDefinition>(it->second);

        // NamespaceEntryKind kind = getNamespaceEntryKind(it->second);
        // if (kind!=NamespaceEntryKind::nsDefinition)
        // {
        //     NamespaceDefinition tmpNs;
        //     tmpNs.positionInfo = newPosInfo;
        //     tmpNs.name         = name_.front();
        //  
        //     const auto &prevDef = std::get<StateMachineDefinition>(it->second);
        //  
        //     throw looking_for_error(makeTypeValueInfo(prevDef), makeTypeValueInfo(tmpNs));
        // }

            return foundNs.findEntry(name_.getTail(), ppe);
        }

        // not empty, e.g. size==1
        if (ppe)
           *ppe = &it->second;

        return true;
    }

    bool findEntry(const FullQualifiedName &name_, NamespaceEntry **ppe = 0)
    {
        return findEntry(name_, 0, ppe);
    }

    // Находит определение по имени. Если имя относительное
    // используется алгоритм поиска, как в C++:
    // относительно текущего NS, затем по шагам переходим на уровень выше
    NamespaceEntryKind findAnyDefinition( const FullQualifiedName &name_
                                        , const FullQualifiedName &curNsName
                                        , NamespaceDefinition *pRootNs
                                        , FullQualifiedName *pFoundName = 0 // found full name
                                        )
    {
        NamespaceEntry *pFoundEntry = 0;

        if (name_.isAbsolute())
        {
            if (!pRootNs)
                return NamespaceEntryKind::none;

            if (!pRootNs->findEntry(name_.toRelative(), pRootNs, &pFoundEntry))
                return NamespaceEntryKind::none;

            if (!pFoundEntry)
                return NamespaceEntryKind::none;

            NamespaceEntryKind kind = getNamespaceEntryKind(*pFoundEntry);
            if (kind==NamespaceEntryKind::none)
                return kind;

            if (pFoundName)
               *pFoundName = name_;

            return kind;
        }

        FullQualifiedName baseNs = curNsName;
        do
        {
            auto checkName = baseNs;
            checkName.append(name_);
            
            if (pRootNs->findEntry(checkName.toRelative(), pRootNs, &pFoundEntry))
            {
                NamespaceEntryKind kind = getNamespaceEntryKind(*pFoundEntry);
                if (kind==NamespaceEntryKind::none)
                    return kind;

                if (pFoundName)
                   *pFoundName = checkName;

                return kind;
            }

            baseNs = baseNs.getHead();

        } while(!baseNs.empty());

        return NamespaceEntryKind::none;
    }


    template< typename ValueType
            >
    void addDefinition(const ValueType &v)
    {
        auto canonicalName = getNamespaceEntryName(v);
        auto it = members.find(canonicalName);
        if (it!=members.end())
        {
            throw already_declared_error(makeTypeValueInfo(it->second), makeTypeValueInfo(v));
        }

        members[canonicalName] = NamespaceEntry{v};
    }

    NamespaceDefinition* getNamespace(const FullQualifiedName &name_, NamespaceDefinition *pRootNs, PositionInfo newPosInfo)
    {
        if (name_.isAbsolute() && pRootNs)
            return pRootNs->getNamespace(name_.toRelative(), pRootNs, newPosInfo);

        if (name_.isAbsolute())
            throw std::runtime_error("umba::tokenizer::ufsm::NamespaceDefinition::getNamespace: can't get entry by absolute name");

        if (name_.empty())
            return this;

        // Имя - не пустое
        auto it = find(name_.front());
        if (it==end())
        {
            NamespaceDefinition newNs;
            newNs.positionInfo = newPosInfo;
            newNs.name         = name_.front();
            addDefinition(newNs);
            it = find(name_.front());
            if (it==end())
                throw std::runtime_error("umba::tokenizer::ufsm::NamespaceDefinition::getNamespace: something goes wrong");
        }

        NamespaceEntryKind kind = getNamespaceEntryKind(it->second);
        if (kind!=NamespaceEntryKind::nsDefinition)
        {
            NamespaceDefinition tmpNs;
            tmpNs.positionInfo = newPosInfo;
            tmpNs.name         = name_.front();

            const auto &prevDef = std::get<StateMachineDefinition>(it->second);

            throw looking_for_error(makeTypeValueInfo(prevDef), makeTypeValueInfo(tmpNs));
        }

        auto &childNsDef = std::get<NamespaceDefinition>(it->second);

        return childNsDef.getNamespace(name_.getTail(), newPosInfo);
    }

    NamespaceDefinition* getNamespace(const FullQualifiedName &name_, PositionInfo newPosInfo)
    {
        return getNamespace(name_, 0, newPosInfo);
    }



public: // methods

    std::string getCanonicalName() const { return name; }

}; // struct NamespaceDefinition

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
TypeValueInfo makeTypeValueInfo(const StateMachineDefinition &d);
TypeValueInfo makeTypeValueInfo(const NamespaceDefinition &d);

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
std::string getNamespaceEntryName(const NamespaceEntry &e);
NamespaceEntryKind getNamespaceEntryKind(const NamespaceEntry &e);
PositionInfo getNamespaceEntryPositionInfo(const NamespaceEntry &e);
StateMachineFlags getNamespaceEntryStateMachineFlags(const NamespaceEntry &e);
std::string getNamespaceEntryKindString(NamespaceEntryKind kind, StateMachineFlags flags);
std::string getNamespaceEntryKindString(NamespaceEntryKind kind);
std::string getNamespaceEntryKindString(const NamespaceEntry &e);
TypeValueInfo makeTypeValueInfo(const NamespaceEntry &d);

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
} // namespace ufsm
} // namespace tokenizer
} // namespace umba
// umba::tokenizer::ufsm::


#if defined(_DEBUG) || defined(DEBUG)
    #include "inserters.h"
#endif


#include "impl/types.h"


