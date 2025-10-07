/*! \file
    \brief Базовые типы для парсера ufsm - реализация функций
 */

#pragma once


//----------------------------------------------------------------------------
// umba::tokenizer::ufsm::
namespace umba {
namespace tokenizer {
namespace ufsm {

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
inline
TypeValueInfo makeTypeValueInfo(const FullQualifiedName &d)
{
    return TypeValueInfo{ d.positionInfo, d.getCanonicalName(), std::string("name") };
}

inline
TypeValueInfo makeTypeValueInfo(const ParentListEntry &d)
{
    return TypeValueInfo{ d.positionInfo, d.getCanonicalName(), std::string("parent list entry") };
}

inline
TypeValueInfo makeTypeValueInfo(const EventDefinition &d)
{
    return TypeValueInfo{ d.positionInfo, d.name, std::string("event") };
}

inline
TypeValueInfo makeTypeValueInfo(const ActionDefinition &d)
{
    return TypeValueInfo{ d.positionInfo, d.name, std::string("action") };
}

inline
TypeValueInfo makeTypeValueInfo(const PredicateDefinition &d)
{
    return TypeValueInfo{ d.positionInfo, d.name, std::string("predicate") };
}

inline
TypeValueInfo makeTypeValueInfo(const TransitionSourceState &d)
{
    return TypeValueInfo{ d.positionInfo, d.getCanonicalName(), std::string("transition source states list element") };
}

inline
TypeValueInfo makeTypeValueInfo(const TransitionEvent &d)
{
    return TypeValueInfo{ d.positionInfo, d.name, std::string("transition events list element") };
}

inline
TypeValueInfo makeTypeValueInfo(const TransitionSourceStates &d)
{
    // Обычно у нас вызывается makeTypeValueInfo для данного списка только в тех случаях, когда хоть один элемент уже есть
    return TypeValueInfo{ d.list[0].positionInfo, d.getCanonicalName(), std::string("transition source states") };
}

inline
TypeValueInfo makeTypeValueInfo(const TransitionEvents &d)
{
    // Обычно у нас вызывается makeTypeValueInfo для данного списка только в тех случаях, когда хоть один элемент уже есть
    return TypeValueInfo{ d.list[0].positionInfo, d.getCanonicalName(), std::string("transition events") };
}

inline
TypeValueInfo makeTypeValueInfo(const TransitionDefinition &d)
{
    return TypeValueInfo{ d.positionInfo, d.getCanonicalName(), std::string("transition") };
}

// struct TransitionDefinition
// {
//     PositionInfo               positionInfo;
//     TransitionSourceStates     sourceStates; 
//     TransitionEvents           events      ;
//     TransitionFlags            flags;
//     LogicExpression            additionalCondition;
//  
//  
//     std::string additionalConditionAsString() const;
//  
//     std::string getCanonicalName() const;


// inline
// TypeValueInfo makeTypeValueInfo(const StateActionRefs &d)
// {
//  
// }

inline
TypeValueInfo makeTypeValueInfo(const StateDefinition &d)
{
    return TypeValueInfo{ d.positionInfo, d.name, std::string("state") };
}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
inline bool FullQualifiedName::isAbsolute() const { return (flags&FullQualifiedNameFlags::absolute)!=0; }

inline
std::string FullQualifiedName::getCanonicalName() const
{
    //static const std::string sep = "::";
    std::string resName;

    // Делаем всегда полностью квалифицированное имя от корня
    for(const auto &n : name)
    {
        resName.append(namespaceSeparator);
        resName.append(n);
    }

    // Если имя не абсолютное, и в начале лежит scope access
    // второй символ не сравниваем, потому что не факт, что он есть, правильно вообще сделать startsWidth
    if ( !isAbsolute() && resName.size()>2 
       //&& resName[0]==namespaceSeparator[0]
      && resName.compare(0, namespaceSeparator.size(), namespaceSeparator)==0
       ) 
    {
        resName.erase(0, namespaceSeparator.size());
    }

    return resName;
}

inline
FullQualifiedName FullQualifiedName::toRelative() const
{
    FullQualifiedName res = *this;
    res.flags &= ~FullQualifiedNameFlags::absolute;
    return res;
}

inline
FullQualifiedName FullQualifiedName::toAbsolute() const
{
    FullQualifiedName res = *this;
    res.flags |= FullQualifiedNameFlags::absolute;
    return res;
}

inline
FullQualifiedName FullQualifiedName::getTail() const
{
    if (empty())
        throw std::runtime_error("umba::tokenizer::ufsm::FullQualifiedName::getTail: can't get tail from empty name");

    FullQualifiedName res;
    res.positionInfo = positionInfo;
    res.flags = flags & ~FullQualifiedNameFlags::absolute; // сбрасывает признак абсолютности
    auto fromIt = name.begin(); ++fromIt;
    res.name.insert(res.name.end(), fromIt, name.end());
    return res;
}

inline
FullQualifiedName FullQualifiedName::getHead() const
{
    FullQualifiedName res;
    res.positionInfo = positionInfo;
    res.flags = flags; // признак абсолютности остаётся таким, как был

    if (name.empty())
        return res;

    auto toIt = name.begin(); std::advance(toIt, std::ptrdiff_t(name.size()-1));
    res.name.insert(res.name.end(), name.begin(), toIt);
    return res;
}


inline
void FullQualifiedName::tailRemove(std::size_t nItems)
{
    if (nItems==std::size_t(-1))
    {
        name.clear();
        return;
    }

    if (nItems>name.size())
        throw std::runtime_error("umba::tokenizer::ufsm::FullQualifiedName::tailRemove: too many items to remove");

    for(std::size_t i=0; i!=nItems; ++i)
        name.pop_back();
}

inline void FullQualifiedName::clear() { tailRemove(std::size_t(-1)); }
inline void FullQualifiedName::append(const std::string &n) { name.emplace_back(n); }
inline void FullQualifiedName::append(const FullQualifiedName &n)
{
    // for(auto &&nm : n) append(nm);
    name.insert(name.end(), n.name.begin(), n.name.end());
}


//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
inline
std::string ParentListEntry::getCanonicalName() const
{
    return name.getCanonicalName();
}
//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
inline
bool TransitionSourceState::isNameEqual(const TransitionSourceState &other) const
{
    if (isAny() && other.isAny())
        return true;
    return name==name;
}

inline bool TransitionSourceState::isAny() const      { return (flags & TransitionSourceStateFlags::any)!=0; }

inline bool TransitionSourceState::isExcluded() const { return (flags & TransitionSourceStateFlags::excluded)!=0; }

inline
std::string TransitionSourceState::getCanonicalName() const
{
    std::string res = isAny() ? std::string("*") : name;
    return std::string(isExcluded() ? "!" : "") + res;
}

inline
int TransitionSourceState::compare(const TransitionSourceState&other) const
{
    if (isAny() || other.isAny())
    {
        // Есть один any

        if (isAny() && other.isAny()) // Оба - any?
        {
            if (isExcluded() && other.isExcluded())
                return 0; // Оба - !*

            if (!isExcluded() && !other.isExcluded())
                return 0; // Оба - *
            
            return isExcluded() ? -1 : 1; // С отрицанием - всегда меньше
        }
        else // Один - any, другой - не any. Any всегда меньше, вне зависимости от отрицания
        {
            return isAny() ? -1 : 1;
        }
    }
    else
    {
        if (isExcluded() != other.isExcluded()) // Один с отрицанием, другой - без
            return isExcluded() ? -1 : 1; // С отрицанием - всегда меньше

        return name.compare(other.name);
    }
}

inline bool TransitionSourceState::isEqual   (const TransitionSourceState &other) const { return isAny()==other.isAny() && isExcluded()==other.isExcluded() && name==other.name; }

inline bool TransitionSourceState::operator< (const TransitionSourceState &other) const { return compare(other)< 0; }
inline bool TransitionSourceState::operator<=(const TransitionSourceState &other) const { return compare(other)<=0; }
inline bool TransitionSourceState::operator> (const TransitionSourceState &other) const { return compare(other)> 0; }
inline bool TransitionSourceState::operator>=(const TransitionSourceState &other) const { return compare(other)>=0; }
inline bool TransitionSourceState::operator==(const TransitionSourceState &other) const { return isEqual(other); }
inline bool TransitionSourceState::operator!=(const TransitionSourceState &other) const { return !isEqual(other); }

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
inline
bool TransitionEvent::isNameEqual(const TransitionEvent &other) const
{
    if (isAny() && other.isAny())
        return true;
    return name==name;
}

inline bool TransitionEvent::isAny() const      { return (flags & TransitionEventFlags::any)!=0; }
inline bool TransitionEvent::isExcluded() const { return (flags & TransitionEventFlags::excluded)!=0; }

inline
std::string TransitionEvent::getCanonicalName() const
{
    std::string res = isAny() ? std::string("*") : name;
    return std::string(isExcluded() ? "!" : "") + res;
}

inline
int TransitionEvent::compare(const TransitionEvent &other) const
{
    if (isAny() || other.isAny())
    {
        // Есть один any

        if (isAny() && other.isAny()) // Оба - any?
        {
            if (isExcluded() && other.isExcluded())
                return 0; // Оба - !*

            if (!isExcluded() && !other.isExcluded())
                return 0; // Оба - *
            
            return isExcluded() ? -1 : 1; // С отрицанием - всегда меньше
        }
        else // Один - any, другой - не any. Any всегда меньше, вне зависимости от отрицания
        {
            return isAny() ? -1 : 1;
        }
    }
    else
    {
        if (isExcluded() != other.isExcluded()) // Один с отрицанием, другой - без
            return isExcluded() ? -1 : 1; // С отрицанием - всегда меньше

        return name.compare(other.name);
    }
}

inline bool TransitionEvent::isEqual   (const TransitionEvent &other) const { return isAny()==other.isAny() && isExcluded()==other.isExcluded() && name==other.name; }

inline bool TransitionEvent::operator< (const TransitionEvent &other) const { return compare(other)< 0; }
inline bool TransitionEvent::operator<=(const TransitionEvent &other) const { return compare(other)<=0; }
inline bool TransitionEvent::operator> (const TransitionEvent &other) const { return compare(other)> 0; }
inline bool TransitionEvent::operator>=(const TransitionEvent &other) const { return compare(other)>=0; }
inline bool TransitionEvent::operator==(const TransitionEvent &other) const { return isEqual(other); }
inline bool TransitionEvent::operator!=(const TransitionEvent &other) const { return !isEqual(other); }

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
inline void TransitionSourceStates::append   ( const TransitionSourceState &st) { list.push_back(st); }

inline void TransitionSourceStates::push_back( const TransitionSourceState &st) { list.push_back(st); }

inline
bool TransitionSourceStates::checkForAny(bool *pHasNormalAny, bool *pHasExcludedAny) const
{
    bool hasNormalAny   = false;
    bool hasExcludedAny = false;

    std::size_t anyCount = 0;

    for(const auto &s : list)
    {
        if (!s.isAny())
            continue;

        ++anyCount;

        if (!s.isExcluded())
            hasNormalAny   = true;
        else
            hasExcludedAny = true;
    }

    if (pHasNormalAny)
       *pHasNormalAny = hasNormalAny;

    if (pHasExcludedAny)
       *pHasExcludedAny = hasExcludedAny;

    return anyCount > 0;
}

inline
std::size_t TransitionSourceStates::getAnyIndex() const
{
    for(std::size_t idx=0; idx!=list.size(); ++idx)
    {
        if (!list[idx].isAny())
            continue;
        return idx;
    }

    return std::size_t(-1);
}

inline std::vector<TransitionSourceState> TransitionSourceStates::getSortedStates() const  { auto res = list; std::sort(res.begin(), res.end()); return res; };

inline
std::string TransitionSourceStates::getCanonicalName() const
{
    auto sorted = getSortedStates();
    std::string res;
    for(const auto &s : sorted)
    {
        if (!res.empty())
            res.append(1, ',');
        res.append(s.getCanonicalName());
    }

    return res;
}

inline
bool TransitionSourceStates::isEqual(const TransitionSourceStates &other) const
{
    if (list.size()!=other.list.size())
        return false;

    auto s1 = getSortedStates();
    auto s2 = other.getSortedStates();

    for(std::size_t i=0u; i!=s1.size(); ++i)
    {
        if (!s1[i].isEqual(s2[i]))
            return false;
    }

    return true;
}

inline
int TransitionSourceStates::compare(const TransitionSourceStates &other) const
{
    auto s1 = getSortedStates();
    auto s2 = other.getSortedStates();

    std::size_t sz = std::min(s1.size(), s2.size());

    for(std::size_t i=0u; i!=sz; ++i)
    {
        if (s1[i].isEqual(s2[i]))
            continue;
        if (s1[i].compare(s2[i])<0)
            return -1;
        else
            return  1;
    }

    // общую часть прошли
    if (s1.size()==s2.size())
        return 0; // При равном размере вектора равны

    return s1.size()<s2.size() ? -1 : 1; // Более короткая строка меньше
}

inline bool TransitionSourceStates::operator< (const TransitionSourceStates &other) const { return compare(other)< 0; }
inline bool TransitionSourceStates::operator<=(const TransitionSourceStates &other) const { return compare(other)<=0; }
inline bool TransitionSourceStates::operator> (const TransitionSourceStates &other) const { return compare(other)> 0; }
inline bool TransitionSourceStates::operator>=(const TransitionSourceStates &other) const { return compare(other)>=0; }
inline bool TransitionSourceStates::operator==(const TransitionSourceStates &other) const { return isEqual(other); }
inline bool TransitionSourceStates::operator!=(const TransitionSourceStates &other) const { return !isEqual(other); }

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
//struct TransitionEvents

inline void TransitionEvents::append   ( const TransitionEvent &st) { list.push_back(st); }
inline void TransitionEvents::push_back( const TransitionEvent &st) { list.push_back(st); }

inline
bool TransitionEvents::checkForAny(bool *pHasNormalAny, bool *pHasExcludedAny) const
{
    bool hasNormalAny   = false;
    bool hasExcludedAny = false;
 
    std::size_t anyCount = 0;
 
    for(const auto &s : list)
    {
        if (!s.isAny())
            continue;
 
        ++anyCount;
 
        if (!s.isExcluded())
            hasNormalAny   = true;
        else
            hasExcludedAny = true;
    }
 
    if (pHasNormalAny)
       *pHasNormalAny = hasNormalAny;
 
    if (pHasExcludedAny)
       *pHasExcludedAny = hasExcludedAny;
 
    return anyCount > 0;
}

inline
std::size_t TransitionEvents::getAnyIndex() const
{
    for(std::size_t idx=0; idx!=list.size(); ++idx)
    {
        if (!list[idx].isAny())
            continue;
        return idx;
    }

    return std::size_t(-1);
}

inline
std::vector<TransitionEvent> TransitionEvents::getSortedEvents() const  { auto res = list; std::sort(res.begin(), res.end()); return res; };

inline
std::string TransitionEvents::getCanonicalName() const
{
    auto sorted = getSortedEvents();
    std::string res;
    for(const auto &s : sorted)
    {
        if (!res.empty())
            res.append(1, ',');
        res.append(s.getCanonicalName());
    }
 
    return res;
}

inline
bool TransitionEvents::isEqual(const TransitionEvents &other) const
{
    if (list.size()!=other.list.size())
        return false;
 
    auto s1 = getSortedEvents();
    auto s2 = other.getSortedEvents();
 
    for(std::size_t i=0u; i!=s1.size(); ++i)
    {
        if (!s1[i].isEqual(s2[i]))
            return false;
    }
 
    return true;
}

inline
int TransitionEvents::compare(const TransitionEvents &other) const
{
    auto s1 = getSortedEvents();
    auto s2 = other.getSortedEvents();
 
    std::size_t sz = std::min(s1.size(), s2.size());
 
    for(std::size_t i=0u; i!=sz; ++i)
    {
        if (s1[i].isEqual(s2[i]))
            continue;
        if (s1[i].compare(s2[i])<0)
            return -1;
        else
            return  1;
    }
 
    // общую часть прошли
    if (s1.size()==s2.size())
        return 0; // При равном размере вектора равны
 
    return s1.size()<s2.size() ? -1 : 1; // Более короткая строка меньше
}

inline bool TransitionEvents::operator< (const TransitionEvents &other) const { return compare(other)< 0; }
inline bool TransitionEvents::operator<=(const TransitionEvents &other) const { return compare(other)<=0; }
inline bool TransitionEvents::operator> (const TransitionEvents &other) const { return compare(other)> 0; }
inline bool TransitionEvents::operator>=(const TransitionEvents &other) const { return compare(other)>=0; }
inline bool TransitionEvents::operator==(const TransitionEvents &other) const { return isEqual(other); }
inline bool TransitionEvents::operator!=(const TransitionEvents &other) const { return !isEqual(other); }

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
inline
std::string TransitionDefinition::additionalConditionAsString() const
{
    // Хотя данный флаг обычно должен проверяться до вызова additionalConditionAsString
    // на всякий случай вернём пустую строку, если он не установлен
    // Чтобы избежать преобразования в строку пустого выражения (а то мало ли что)
    if ((flags&TransitionFlags::conditional)==0)
        return std::string();
    return LogicExpressionEvaluator(makeLogicExpressionOperatorTraits()).toString(additionalCondition);
}

inline
std::string TransitionDefinition::getCanonicalName() const
{
    std::string name = sourceStates.getCanonicalName() + std::string(":") + events.getCanonicalName();

    if ((flags&TransitionFlags::conditional)!=0)
    {
        name.append(1,'?');
        name.append(additionalConditionAsString());
    }

    return name;
}

inline
int  TransitionDefinition::compare(const TransitionDefinition &other) const
{
    int cmp = sourceStates.compare(other.sourceStates);
    if (cmp!=0)
        return cmp;

    cmp = events.compare(other.events);
    if (cmp!=0)
        return cmp;

    if ((flags&TransitionFlags::conditional)==0) // флаг не установлен - не участвует в сравнении
        return cmp;

    return additionalConditionAsString().compare(other.additionalConditionAsString());
}

inline
bool TransitionDefinition::isEqual(const TransitionDefinition &other) const
{
    return !(!sourceStates.isEqual(other.sourceStates)
          || !events.isEqual(other.events)
          || additionalConditionAsString().compare(other.additionalConditionAsString())!=0
            );
}

inline bool TransitionDefinition::operator< (const TransitionDefinition &other) const { return compare(other)< 0; }
inline bool TransitionDefinition::operator<=(const TransitionDefinition &other) const { return compare(other)<=0; }
inline bool TransitionDefinition::operator> (const TransitionDefinition &other) const { return compare(other)> 0; }
inline bool TransitionDefinition::operator>=(const TransitionDefinition &other) const { return compare(other)>=0; }

inline
void TransitionDefinition::appendImpl(const TransitionSourceState &st)
{
    for(const auto &v : sourceStates.list)
    {
        if (v.isNameEqual(st))
            throw already_declared_error(makeTypeValueInfo(v), makeTypeValueInfo(st));
    }

    sourceStates.append(st);
}

inline
void TransitionDefinition::appendImpl(const TransitionEvent &te)
{
    for(const auto &v : events.list)
    {
        if (v.isNameEqual(te))
            throw already_declared_error(makeTypeValueInfo(v), makeTypeValueInfo(te));
    }

    events.append(te);
}

inline void TransitionDefinition::append   ( const TransitionSourceState &st) { appendImpl(st); }
inline void TransitionDefinition::push_back( const TransitionSourceState &st) { appendImpl(st); }

inline void TransitionDefinition::append   ( const TransitionEvent &te) { appendImpl(te); }
inline void TransitionDefinition::push_back( const TransitionEvent &te) { appendImpl(te); }

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
inline
bool StateActionRefs::appendImpl(const std::string &actionName)
{
    for(const auto &st: list)
    {
        if (st==actionName)
            return false;
    }

    list.emplace_back(actionName);
    return true;
}

inline bool StateActionRefs::append   ( const std::string &actionName) { return appendImpl(actionName); }
inline bool StateActionRefs::push_back( const std::string &actionName) { return appendImpl(actionName); }
inline bool StateActionRefs::empty() const       { return list.empty(); }
inline std::size_t StateActionRefs::size() const { return list.size (); }

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
bool StateDefinition::addActionRef(StateActionKind ak, const std::string &actionName)
{
    return actionRefs[ak].append(actionName);
}

//----------------------------------------------------------------------------





//----------------------------------------------------------------------------
} // namespace ufsm
} // namespace tokenizer
} // namespace umba
// umba::tokenizer::ufsm::

