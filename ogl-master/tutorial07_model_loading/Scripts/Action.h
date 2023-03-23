#pragma once
#include <vector>

template<typename Res, typename ... Params>
struct IDelegate
{
public:
    virtual Res Invoke(Params... _params) const = 0;
};

template<typename Res, typename Class, typename ... Params>
struct MemberDelegate : public IDelegate<Res, Params...>
{
private:
    typedef Res(Class::* Function)(Params...);
    Function function = nullptr;
    Class* owner = nullptr;

public:
    MemberDelegate(nullptr_t)
    {
        owner = nullptr;
        function = nullptr;
    }
    MemberDelegate(Class* _instance, Function _function)
    {
        owner = _instance;
        function = _function;
    }

    const Function& GetFunction() const
    {
        return function;
    }
    const Class* GetOwner() const
    {
        return owner;
    }

    Res Invoke(Params... _params) const override
    {
        if (!IsValid())
            throw std::exception("[MemberDelegate] => function or owner is null");
        return (owner->*function)(_params...);
    }

    bool IsValid() const
    {
        return function != nullptr && owner != nullptr;
    }

    void SetDynamic(Class* _instance, Function _function)
    {
        owner = _instance;
        function = _function;
    }

    void operator=(Function _function)
    {
        function = _function;
    }
    void operator=(nullptr_t)
    {
        function = nullptr;
        owner = nullptr;
    }
};

template<typename Res, typename ... Params>
struct StaticDelegate : public IDelegate<Res, Params...>
{
private:
    typedef Res(*Function)(Params...);
    Function function = nullptr;

public:
    StaticDelegate(nullptr_t)
    {
        function = nullptr;
    }
    StaticDelegate(Function _function)
    {
        function = _function;
    }

    const Function& GetFunction() const
    {
        return function;
    }

    Res Invoke(Params... _params) const override
    {
        if (!IsValid())
            throw std::exception("[StaticDelegate] => function is null");
        return function(_params...);
    }

    bool IsValid() const
    {
        return function != nullptr;
    }

    void SetDynamic(Function _function)
    {
        function = _function;
    }

    void operator=(Function _function)
    {
        function = _function;
    }
    void operator=(nullptr_t)
    {
        function = nullptr;
    }
};

template<typename Res, typename Class, typename ... Params>
struct MemberDelegateConst : public IDelegate<Res, Params...>
{
private:
    typedef Res(Class::* Function)(Params...) const;
    Function function = nullptr;
    Class* owner = nullptr;

public:
    MemberDelegateConst(nullptr_t)
    {
        function = nullptr;
        owner = nullptr;
    }
    MemberDelegateConst(Class* _owner, Function _function)
    {
        owner = _owner;
        function = _function;
    }

    const Function& GetFunction() const
    {
        return function;
    }
    const Class* GetOwner() const
    {
        return owner;
    }

    Res Invoke(Params... _params) const override
    {
        if (!IsValid())
            throw std::exception("[MemberDelegate] => function or owner is null");
        return (owner->*function)(_params...);
    }

    bool IsValid() const
    {
        return function != nullptr && owner != nullptr;
    }

    void SetDynamic(Class* _instance, Function _function)
    {
        owner = _instance;
        function = _function;
    }

    void operator=(Function _function)
    {
        function = _function;
    }
    void operator=(nullptr_t)
    {
        function = nullptr;
        owner = nullptr;
    }
};

template<typename Res, typename ... Params>
struct ActionInternal : public IDelegate<Res, Params...>
{
private:
    typedef std::vector<IDelegate<Res, Params...>*> DataType;
    DataType delegates = DataType();

public:
    ActionInternal()
    {
        delegates = DataType();
    }
    ActionInternal(nullptr_t)
    {
        delegates = DataType();
    }

    void AddDynamic(Res(*ptr)(Params...))
    {
        delegates.push_back(new StaticDelegate<Res, Params...>(ptr));
    }

    template<typename Class>
    void AddDynamic(Class* _instance, Res(Class::* ptr)(Params...))
    {
        delegates.push_back(new MemberDelegate<Res, Class, Params...>(_instance, ptr));
    }

    template<typename Class>
    void AddDynamic(Class* _instance, Res(Class::* ptr)(Params...)const)
    {
        delegates.push_back(new MemberDelegateConst<Res, Class, Params...>(_instance, ptr));
    }


    void RemoveDynamic(Res(*ptr)(Params...))
    {
        const size_t& _size = delegates.size();
        for (size_t i = 0; i < _size; ++i)
        {
            StaticDelegate<Res, Params...>* _staticDelegate = reinterpret_cast<StaticDelegate<Res, Params...>*>(delegates[i]);
            if (!_staticDelegate)continue;
            if (_staticDelegate->GetFunction() == ptr)
            {
                delegates.erase(delegates.begin() + i);
                delete _staticDelegate;
                _staticDelegate = nullptr;
                return;
            }
        }
    }

    template<typename Class>
    void RemoveDynamic(Class* _instance, Res(Class::* ptr)(Params...))
    {
        const size_t& _size = delegates.size();
        for (size_t i = 0; i < _size; ++i)
        {
            MemberDelegate<Res, Class, Params...>* _memberDelegate = reinterpret_cast<MemberDelegate<Res, Class, Params...>*>(delegates[i]);
            if (!_memberDelegate)continue;
            if (_memberDelegate->GetFunction() == ptr && _memberDelegate->GetOwner() == _instance)
            {
                delegates.erase(delegates.begin() + i);
                delete _memberDelegate;
                _memberDelegate = nullptr;
                return;
            }
        }
    }

    template<typename Class>
    void RemoveDynamic(Class* _instance, Res(Class::* ptr)(Params...)const)
    {
        const size_t& _size = delegates.size();
        for (size_t i = 0; i < _size; ++i)
        {
            MemberDelegateConst<Res, Class, Params...>* _memberDelegateConst = reinterpret_cast<MemberDelegateConst<Res, Class, Params...>*>(delegates[i]);
            if (!_memberDelegateConst)continue;
            if (_memberDelegateConst->GetFunction() == ptr && _memberDelegateConst->GetOwner() == _instance)
            {
                delegates.erase(delegates.begin() + i);
                delete _memberDelegateConst;
                _memberDelegateConst = nullptr;
                return;
            }
        }
    }

    Res Invoke(Params... _params) const override
    {
        const size_t& _size = delegates.size();

        if constexpr (std::is_same_v<Res, void>)
        {
            for (size_t i = 0; i < _size; ++i)
                delegates[i]->Invoke(_params...);
            return Res();
        }
        else
        {
            Res _result = Res();
            for (size_t i = 0; i < _size; ++i)
                _result = delegates[i]->Invoke(_params...);
            return _result;
        }
    }

    void Clear()
    {
        const size_t& _size = delegates.size();
        for (size_t i = 0; i < _size; ++i)
        {
            delete delegates[i];
            delegates[i] = nullptr;
        }

        delegates.clear();
    }

};

template<typename ... Params>
using Action = ActionInternal<void, Params...>;

template<typename Res, typename ... Params>
using Func = ActionInternal<Res, Params...>;

template<typename ... Params>
using Predicate = ActionInternal<bool, Params...>;