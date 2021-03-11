#pragma once

#include "Callback.hpp"

//
// Description: Use to implement Observer pattern.
// Objects that want to be notified can use the `+=` operator to subscribe and
// the `-=` operator to unsubscribe
//
template <typename... Args> class Action
{
  private:
    using ActionCallback = Callback<Args...>;

    /*
     * List of callbacks to notify each subscribers
     */
    std::vector<ActionCallback *> _subscribers;

  public:
    Action()
    {
    }

    /*
     * Notify all subscribers
     */
    void operator()(Args... args)
    {
        for (auto &s : _subscribers)
        {
            (*s)(std::forward<Args>(args)...);
        }
    }

    /*
     * Subscribe to this action
     */
    void operator+=(ActionCallback &callback)
    {
        _subscribers.push_back(&callback);
    }

    /*
     * Unsubscribe from this action
     */
    void operator-=(ActionCallback &callback)
    {
        auto cb = std::find(_subscribers.begin(), _subscribers.end(), &callback);
        if (cb != _subscribers.end())
        {
            _subscribers.erase(cb);
        }
    }
};
