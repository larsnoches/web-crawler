#ifndef IOEXCEPTION_H
#define IOEXCEPTION_H

#include <string>
#include <stdexcept>

class IOException : public std::runtime_error
{
  public:
    IOException(const std::string& st) : std::runtime_error(st)
    {
      //
    }
};

#endif // IOEXCEPTION_H
