#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <map>

class Dispatcher
{
  public:
    Dispatcher();
    virtual ~Dispatcher() noexcept;
    void operator()();
    unsigned int getCounter() const noexcept;
    void resetCounter() noexcept;

  private:
    unsigned int m_instrCount;
    //std::map<> m_FuncMap;
};

#endif // DISPATCHER_H
