#include "leak.h"

Leak::Leak(QString _content)
    : content(_content)
{
    QStringList list = _content.trimmed().split("    ");
    list.removeAll(QString(""));
    static QRegularExpression re("==\\d*==");

    if(!list.empty()) {
        summary = list.first();
        content.remove(summary);
        summary = summary.remove(re).trimmed();
    }

    static QRegularExpression fileNameRegex("\\([aA-zZ]*.cpp:\\d*\\)");
    static QRegularExpression functionRegex("(?:\\w+\\s+)*?(?:(\\w+)::)?(\\w+)\\s*\\(([^:()]*?)\\)");

    for (const QRegularExpressionMatch &match : fileNameRegex.globalMatch(content)) {
        if(match.hasMatch()) {
            inFiles << match.captured(0).replace("(","").replace(")","").trimmed();
        }
    }

    for (const QRegularExpressionMatch &match : functionRegex.globalMatch(content)) {
        if(match.hasMatch()) {
            functions << match.captured(0).trimmed();
        }
    }

    qDebug() << functions;

    content = content.trimmed().replace(re,"").replace("    ", "\n");
}
