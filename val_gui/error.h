#ifndef ERROR_H
#define ERROR_H

#include <QString>

class Error
{
public:
    Error();

    virtual QString to_string() const {return "";};
};

#endif // ERROR_H
