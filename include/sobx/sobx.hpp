#ifndef SOBX_HPP
#define SOBX_HPP
#include <cassert>
#include <iterator>
#include <stdexcept>
#include <exception>
#include <memory>
#include <type_traits>
#include <optional>

namespace sobx
{

template <class T, class U = T, class Enabler = void>
struct is_equality_comparable : std::false_type
{
};

template <class T, class U>
struct is_equality_comparable<T, U, typename std::enable_if<true, decltype(std::declval<const T &>() == std::declval<const U &>(), (void)0)>::type> : std::true_type
{
};

template <class T, class Enabler = void>
struct observable_traits
{
    static constexpr bool is_equality_comparable = is_equality_comparable<T>::value;
};

template <class T>
struct observable_traits<std::optional<T>>
{
    static constexpr bool is_equality_comparable = observable_traits<T>::is_equality_comparable;
};

class reaction
{
public:
    void dispose()
    {
        disposed_ = true;
    }

    bool is_disposed() const
    {
        return disposed_;
    }

private:
    bool disposed_ = false;
};

namespace detail
{

class noncopyable
{
public:
    noncopyable(const noncopyable &other) = delete;
    noncopyable(noncopyable &&other) = delete;
    noncopyable &operator=(const noncopyable &other) = delete;
    noncopyable &operator=(noncopyable &&other) = delete;

protected:
    noncopyable()
    {
    }

    ~noncopyable()
    {
    }
};

template <class Tag>
struct intrusive_list_item
{
public:
    ~intrusive_list_item()
    {
        unlink();
    }

    bool is_linked() const
    {
        assert(!!next == !!prev);
        return !!next;
    }

    void unlink()
    {
        if (is_linked())
        {
            prev->next = next;
            next->prev = prev;
            prev = nullptr;
            next = nullptr;
        }
    }

    intrusive_list_item *next = nullptr;
    intrusive_list_item *prev = nullptr;
};

template <class T, class Tag>
class intrusive_list_iterator
{
public:
    typedef std::bidirectional_iterator_tag iterator_category;
    typedef T value_type;
    typedef std::ptrdiff_t difference_type;
    typedef value_type *pointer;
    typedef value_type &reference;

    typedef Tag tag_type;
    typedef intrusive_list_item<tag_type> item_type;

    intrusive_list_iterator()
    {
    }

    explicit intrusive_list_iterator(item_type *p)
        : p_(p)
    {
    }

    template <class T_>
    explicit intrusive_list_iterator(intrusive_list_iterator<T_, tag_type> other)
        : p_(other.p_)
    {
    }

    reference operator*() const
    {
        return static_cast<reference>(*p_);
    }

    pointer operator->() const
    {
        return static_cast<pointer>(p_);
    }

    intrusive_list_iterator &operator++()
    {
        p_ = p_->next;
        return *this;
    }

    intrusive_list_iterator operator++(int)
    {
        auto old = *this;
        ++*this;
        return old;
    }

    intrusive_list_iterator &operator--()
    {
        p_ = p_->prev;
        return *this;
    }

    intrusive_list_iterator operator--(int)
    {
        auto old = *this;
        --*this;
        return old;
    }

    template <class T_>
    bool operator==(intrusive_list_iterator<T_, tag_type> rhs) const
    {
        return p_ == rhs.p_;
    }

    template <class T_>
    bool operator!=(intrusive_list_iterator<T_, tag_type> rhs) const
    {
        return p_ != rhs.p_;
    }

private:
    item_type *p_ = nullptr;
};

template <class T, class Tag>
class intrusive_list : noncopyable
{
public:
    typedef T value_type;
    typedef Tag tag_type;
    typedef intrusive_list_item<tag_type> item_type;

    typedef intrusive_list_iterator<value_type, tag_type> iterator;
    typedef intrusive_list_iterator<const value_type, tag_type> const_iterator;

    typedef typename iterator::reference reference;
    typedef typename const_iterator::reference const_reference;
    typedef typename iterator::pointer pointer;
    typedef typename const_iterator::pointer const_pointer;

    intrusive_list()
    {
        head_.next = &head_;
        head_.prev = &head_;
    }

    ~intrusive_list()
    {
        while (!empty())
        {
            static_cast<item_type &>(front()).unlink();
        }
    }

    void push_back(value_type &value)
    {
        auto &ref = static_cast<item_type &>(value);

        assert(!ref.is_linked());

        head_.prev->next = &ref;
        ref.prev = head_.prev;

        ref.next = &head_;
        head_.prev = &ref;
    }

    bool empty() const
    {
        return head_.next == &head_;
    }

    iterator begin()
    {
        return iterator(head_.next);
    }

    const_iterator begin() const
    {
        return const_iterator(head_.next);
    }

    iterator end()
    {
        return iterator(&head_);
    }

    const_iterator end() const
    {
        return const_iterator(&head_);
    }

    reference front()
    {
        return *begin();
    }

    const_reference front() const
    {
        return *begin();
    }

    reference back()
    {
        return *std::prev(end());
    }

    const_reference back() const
    {
        return *std::prev(end());
    }

private:
    item_type head_;
};

struct connection_from_observable_tag;
struct connection_from_observer_tag;
struct observable_from_trigger_get_tag;
struct observable_from_trigger_set_tag;
struct observer_from_trigger_tag;

class observable_base;
class observer_base;

class connection : public intrusive_list_item<connection_from_observable_tag>,
                   public intrusive_list_item<connection_from_observer_tag>,
                   noncopyable
{
public:
    connection(observable_base &able, observer_base &er);

    ~connection();

    observable_base &able;
    observer_base &er;
};

class observable_base : public intrusive_list_item<observable_from_trigger_get_tag>,
                        public intrusive_list_item<observable_from_trigger_set_tag>,
                        noncopyable
{
public:
    observable_base()
    {
    }

    ~observable_base()
    {
        disconnect();
    }

    void disconnect()
    {
        while (!connections.empty())
        {
            delete &connections.front();
        }
    }

    intrusive_list<connection, connection_from_observable_tag> connections;
};

class observer_base : public intrusive_list_item<observer_from_trigger_tag>,
                      noncopyable
{
public:
    observer_base()
    {
    }

    virtual ~observer_base()
    {
        disconnect();
    }

    void disconnect()
    {
        while (!connections.empty())
        {
            delete &connections.front();
        }
    }

    virtual void on_notify(reaction &r) = 0;

    intrusive_list<connection, connection_from_observer_tag> connections;
};

inline connection::connection(observable_base &able, observer_base &er)
    : able(able), er(er)
{
    able.connections.push_back(*this);
    er.connections.push_back(*this);
}

inline connection::~connection()
{
    static_cast<intrusive_list_item<connection_from_observable_tag> &>(*this).unlink();
    static_cast<intrusive_list_item<connection_from_observer_tag> &>(*this).unlink();
}

class trigger
{
public:
    trigger()
        : prev_(get())
    {
        get() = this;
    }

    ~trigger()
    {
        get() = prev_;
    }

    void notify(observer_base *new_er = nullptr)
    {
        std::exception_ptr e;

        while (true)
        {
            intrusive_list<observer_base, observer_from_trigger_tag> fired;

            if (new_er)
            {
                fired.push_back(*new_er);
                new_er = nullptr;
            }

            while (!set_observables_.empty())
            {
                auto &able = set_observables_.front();
                auto &ref = static_cast<intrusive_list_item<observable_from_trigger_set_tag> &>(able);

                for (auto &c : able.connections)
                {
                    if (c.er.is_linked())
                    {
                        c.er.unlink();
                    }

                    fired.push_back(c.er);
                }

                ref.unlink();
            }

            if (fired.empty())
            {
                break;
            }

            while (!fired.empty())
            {
                auto &er = fired.front();
                er.unlink();

                er.disconnect();

                reaction r;

                try
                {
                    er.on_notify(r);
                }
                catch (...)
                {
                    if (!e)
                    {
                        e = std::current_exception();
                    }
                }

                if (!r.is_disposed())
                {
                    while (!get_observables_.empty())
                    {
                        auto &able = get_observables_.front();
                        static_cast<intrusive_list_item<observable_from_trigger_get_tag> &>(able).unlink();
                        new connection(able, er);
                    }
                }
            }
        }

        if (e)
        {
            std::rethrow_exception(e);
        }
    }

    static void on_get(observable_base &able)
    {
        if (auto t = get())
        {
            auto &ref = static_cast<intrusive_list_item<observable_from_trigger_get_tag> &>(able);

            if (!ref.is_linked())
            {
                t->get_observables_.push_back(able);
            }
        }
    }

    static void on_set(observable_base &able)
    {
        if (auto t = get())
        {
            auto &ref = static_cast<intrusive_list_item<observable_from_trigger_set_tag> &>(able);

            if (!ref.is_linked())
            {
                t->set_observables_.push_back(able);
            }
        }
        else
        {
            throw std::runtime_error("observable can only be changed inside action");
        }
    }

private:
    static trigger *&get()
    {
        thread_local trigger *t = nullptr;
        return t;
    }

    trigger *prev_;
    intrusive_list<observable_base, observable_from_trigger_get_tag> get_observables_;
    intrusive_list<observable_base, observable_from_trigger_set_tag> set_observables_;
};

template <class F>
class observer : public detail::observer_base
{
public:
    typedef F function_type;

    explicit observer(function_type f)
        : f_(f)
    {
    }

    virtual void on_notify(reaction &r) override
    {
        auto f = detail::bind_if_bindable(f_, r);
        f();
    }

private:
    function_type f_;
};

template <class T>
bool equal(const T &lhs, const T &rhs, typename std::enable_if<observable_traits<T>::is_equality_comparable>::type * = nullptr)
{
    return lhs == rhs;
}

template <class T>
bool equal(const T &lhs, const T &rhs, typename std::enable_if<!observable_traits<T>::is_equality_comparable>::type * = nullptr)
{
    return false;
}

template <class F, class T>
auto bind_if_bindable(F f, T &value, typename std::enable_if<std::is_invocable<F, T>::value>::type * = nullptr)
{
    return std::bind(f, std::ref(value));
}

template <class F, class T>
auto bind_if_bindable(F f, T &value, typename std::enable_if<!std::is_invocable<F, T>::value>::type * = nullptr)
{
    return f;
}

} // namespace detail

template <class T>
class observable : detail::observable_base
{
public:
    typedef T value_type;

    observable()
        : value_()
    {
    }

    observable(const observable &other)
        : value_(other.value_)
    {
    }

    observable(observable &&other)
        : value_(std::move(other.value_))
    {
    }

    observable(const value_type &value)
        : value_(value)
    {
    }

    observable(value_type &&value)
        : value_(std::move(value))
    {
    }

    observable &operator=(const observable &other)
    {
        set(other.value_);
        return *this;
    }

    observable &operator=(observable &&other)
    {
        set(std::move(other.value_));
        return *this;
    }

    observable &operator=(const value_type &value)
    {
        set(value);
        return *this;
    }

    observable &operator=(value_type &&value)
    {
        set(std::move(value));
        return *this;
    }

    const value_type &get() const
    {
        detail::trigger::on_get(const_cast<observable &>(*this));
        return value_;
    }

    operator const value_type &() const
    {
        return get();
    }

    void set(const value_type &value)
    {
        if (!detail::equal(value_, value))
        {
            detail::trigger::on_set(*this);
            value_ = value;
        }
    }

    void set(value_type &&value)
    {
        if (!detail::equal(value_, value))
        {
            detail::trigger::on_set(*this);
            value_ = std::move(value);
        }
    }

private:
    value_type value_;
};

struct disposer
{
public:
    disposer()
    {
    }

    disposer(std::unique_ptr<detail::observer_base> er)
        : er_(std::move(er))
    {
    }

    void dispose()
    {
        er_.reset();
    }

    bool is_disposed() const
    {
        return !er_;
    }

private:
    std::unique_ptr<detail::observer_base> er_;
};

template <class F>
void run_in_action(F f)
{
    detail::trigger t;

    try
    {
        f();
    }
    catch (...)
    {
        t.notify();
        throw;
    }

    t.notify();
}

template <class F>
disposer autorun(F f)
{
    auto er = std::make_unique<detail::observer<F>>(f);

    detail::trigger t;

    t.notify(er.get());

    return disposer(std::move(er));
}

} // namespace sobx

#endif
