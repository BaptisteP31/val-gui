#ifndef READER_H
#define READER_H

#include <QTextStream>
#include <QMessageBox>
#include <QFile>
#include <QDebug>
#include <QRegularExpression>
#include <QList>

#include "leak.h"

class reader
{
private:
    QString content;

public:
    reader(QString _path);
    QList<Leak> get_leaks();
    QString get_summary() const;
    QString get_command() const;
};

#endif // READER_H
