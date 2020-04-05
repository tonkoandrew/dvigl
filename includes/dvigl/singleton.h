#pragma once

template <typename T> class Singleton
{
public:
    Singleton()
    {
        if (instance == 0)
        {
            instance = (T*)this;
        }
    }
    virtual ~Singleton() { instance = 0; }
    static T* ptr() { return instance; }
    void Release()
    {
        if (this)
        {
            delete this;
            instance = 0;
        }
        return;
    }

private:
    static T* instance;
};

template <typename T> T* Singleton<T>::instance = 0;
