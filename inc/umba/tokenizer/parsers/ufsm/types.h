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
    insertion_ordered_map<TransitionDefinition>    transitions;
    insertion_ordered_map<StateDefinition     >    states     ;


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

    // Тут проблема только в том, что мы сначала читаем определение, разбираем в нём ошибки,
    // а потом - добавляем, и у нас может выскочить ошибка, что такое определение уже есть.
    // Т.е. мы сначала вылижем определение, а потом окажется, что оно уже есть.
    void addDefinition(const ParentListEntry      &v) { addDefinitionImpl<already_used_error    >(v, parents    ); }
    void addDefinition(const EventDefinition      &v) { addDefinitionImpl<already_declared_error>(v, events     ); }
    void addDefinition(const ActionDefinition     &v) { addDefinitionImpl<already_declared_error>(v, actions    ); }
    void addDefinition(const PredicateDefinition  &v) { addDefinitionImpl<already_declared_error>(v, predicates ); }
    void addDefinition(const TransitionDefinition &v) { addDefinitionImpl<already_declared_error>(v, transitions); }
    void addDefinition(const StateDefinition      &v) { addDefinitionImpl<already_declared_error>(v, states     ); }

    // Ещё нам надо будет потом мержить и наследовать

    std::string getCanonicalName() const { return name; }


}; // struct StateMachineDefinition

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
struct NamespaceDefinition;


//----------------------------------------------------------------------------
using NamespaceEntry = std::variant<NamespaceDefinition, StateMachineDefinition>;

//------------------------------
std::string getNamespaceEntryName(const NamespaceEntry &e);
NamespaceEntryKind getNamespaceEntryKind(const NamespaceEntry &e);

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

    iterator find(const std::string &name)
    {
        return members.find(name);
    }

    const_iterator find(const std::string &name) const
    {
        return members.find(name);
    }

    iterator        begin()       { return members.begin (); }
    iterator        end  ()       { return members.end   (); }
    const_iterator  begin() const { return members.begin (); }
    const_iterator  end  () const { return members.end   (); }
    const_iterator cbegin() const { return members.cbegin(); }
    const_iterator cend  () const { return members.cend  (); }


    bool findEntry(const FullQualifiedName &name, NamespaceEntry *pe = 0)
    {
        if (name.empty())
            throw std::runtime_error("umba::tokenizer::ufsm::NamespaceDefinition::findEntry: can't get entry by empty name");

        if (name.isAbsolute())
            throw std::runtime_error("umba::tokenizer::ufsm::NamespaceDefinition::findEntry: can't get entry by absolute name");

        auto it = find(name.front());
        if (it==end())
            return false;

        if (it->second.size()>1)
            return findEntry(name.getTail(), pe);

        // not empty, e.g. size==1
        if (pe)
           *pe = &it->second;

        return true;
    }

    //NamespaceEntryKind getNamespaceEntryKind(const NamespaceEntry &e)


public: // methods

    std::string getCanonicalName() const { return name; }

}; // struct NamespaceDefinition

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
inline
TypeValueInfo makeTypeValueInfo(const StateMachineDefinition &d)
{
    return TypeValueInfo{ d.positionInfo, d.getCanonicalName(), std::string("state machine") };
}

TypeValueInfo makeTypeValueInfo(const NamespaceDefinition &d)
{
    return TypeValueInfo{ d.positionInfo, d.getCanonicalName(), std::string("namespace") };
}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
inline
std::string getNamespaceEntryName(const NamespaceEntry &e)
{
    return std::visit( [](const auto &a)
                       {
                           return e.getCanonicalName();
                       }
                     , e
                     );
}

//------------------------------
inline
NamespaceEntryKind getNamespaceEntryKind(const NamespaceEntry &e)
{
    return std::visit( [](const auto &a) -> NamespaceEntryKind
                       {
                           using ArgType = std::decay_t<decltype(a)>;
                           if constexpr (std::is_same_v <ArgType, NamespaceDefinition    >) return NamespaceEntryKind::nsDefinition ;
                           if constexpr (std::is_same_v <ArgType, StateMachineDefinition >) return NamespaceEntryKind::fsmDefinition;
                       }
                     , e
                     );
}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
} // namespace ufsm
} // namespace tokenizer
} // namespace umba
// umba::tokenizer::ufsm::


