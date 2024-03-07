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
    QString new_line_content;

public:
    reader(QString _path);
    QList<Leak> get_leaks();
    QString get_summary() const;
    QString get_command() const;
    QString get_content() const {return content;}
    QString get_new_line_content() const {return new_line_content;}
};

#endif // READER_H
