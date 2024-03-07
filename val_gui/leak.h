#ifndef LEAK_H
#define LEAK_H

#include "error.h"
#include <QList>
#include <Qdebug>
#include <QRegularExpression>

class Leak : Error
{
private:
    QString summary;
    QString content;
    QStringList inFiles;
    QStringList functions;

public:
    Leak(QString _content);

    QString to_string() const {return summary;}
    QString get_content() const {return content;}
    QStringList get_in_files() const {return inFiles;}
    QStringList get_functions() const {return functions;}
};

#endif // LEAK_H
