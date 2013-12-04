#ifndef __SINGLETON_H__
#define __SINGLETON_H__

template <class T>
class Singleton
{
private:
    Singleton(const Singleton<T> &);
    Singleton& operator=(const Singleton<T> &);

    static T* m_instance;

protected:
    Singleton() {
        m_instance = static_cast<T*>(this);
    }

public:
    virtual ~Singleton() {
        m_instance = 0;
    }

    static T* instance() {
        if(!m_instance) {
            m_instance = static_cast<T*>(new T);
        }
        return m_instance;
    }
};

template<class T>
T* Singleton<T>::m_instance = 0;

#endif
