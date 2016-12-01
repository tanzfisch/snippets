#include <functional>
#include <string>
#include <map>
#include <vector>
#include <iostream>
using namespace std;

// A iSignal object may call multiple slots with the
// same signature. You can connect functions to the iSignal
// which will be called when the emit() method on the
// iSignal object is invoked. Any argument passed to emit()
// will be passed to the given functions.

template <typename... Args>
class iSignal
{

public:

    iSignal()
    {
    }

    // copy creates new iSignal
    iSignal(iSignal const& other)
    {
    }

    // connects a member function of a given object to this iSignal
    template <typename F, typename... A>
    void connect_member(F&& f, A&& ... a) const
    {
        _slots.push_back(bind(f, a...));
    }

    // connects a function to the iSignal. The returned
    // value can be used to disconnect the function again
    void connect(function<void(Args...)> const& slot) const
    {
        _slots.push_back(slot);
    }

    // disconnects a previously connected function
    void disconnect(function<void(Args...)> const& slot) const
    {
        auto iter = find(_slots.begin(), _slots.end(), slot);
        if (iter != _slots.end())
        {
            _slots.erase(iter);
        }
    }

    // disconnects all previously connected functions
    void disconnect_all() const
    {
        _slots.clear();
    }

    void operator() (Args... p)
    {
        for (auto it : _slots)
        {
            it(p...);
        }
    }

    // assignment creates new iSignal
    iSignal& operator=(iSignal const& other)
    {
        disconnect_all();
    }

private:

    mutable vector<function<void(Args...)>> _slots;

};

class Message
{
public:
    void message()
    {
        cout << "message" << endl;
    }

    void blub(string arg1, int arg2)
    {
        cout << arg1 << " blub " << arg2 << endl;
    }
};

void blub(string arg1, int arg2)
{
    cout << arg1 << " blub " << arg2 << endl;
}

int main()
{
    // create new iSignal
    iSignal<string, int> signal;

    // attach a slot
    signal.connect([](string arg1, int arg2)
    {
        cout << arg1 << " " << arg2 << endl;
    });

    signal.connect(blub);


    //signal.disconnect(blub);

    //signal.connect_member(&Message::message, blub);           /// hier ist es nicht ok

    signal("The answer:", 42);

    iSignal<> signal2;
    Message message;
    signal2.connect_member(&Message::message, message);       /// hier ist es ok 

    signal2();

    return 0;
}
