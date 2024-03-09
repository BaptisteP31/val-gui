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
    static QRegularExpression functionRegex("(?:\\w+\\s+)*?(?:(\\w+)::)?(\\w+)\\s*\\(([^:()]*?)\\)(\\s\\([aA-zZ]*.[aA-zZ]*:\\d*\\))?");

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

    content = content.trimmed().replace(re,"").replace("    ", "\n");
}

int Leak::get_lost_bytes() const {
    static QRegularExpression bytesExp("^\\d*");
    return bytesExp.match(summary).captured(0).toInt();
}

QStandardItem* Leak::get_leaks_as_items(QList<Leak> leaks) {
    QStandardItem *leakItem = new QStandardItem("Leaks");

    // populate the treeview
    for (const auto& leak : leaks) {
        // Leak summaries
        QStandardItem *item = new QStandardItem(leak.to_string());

        // File list
        QStandardItem *inFiles = new QStandardItem("Files");
        for (const auto& filename : leak.get_in_files()) {
            QStandardItem *file = new QStandardItem(filename);
            inFiles->appendRow(file);
        }

        // List functions
        QStandardItem *inFunctions = new QStandardItem("Functions");
        for (const auto& function : leak.get_functions()) {
            QStandardItem *func = new QStandardItem(function);
            inFunctions->setForeground(Qt::blue);
            inFunctions->appendRow(func);

            // operator new means no delete
            if(function.startsWith("operator new")) {
                QStandardItem *func = new QStandardItem("Hint: delete statement might be missing");
                inFunctions->setForeground(Qt::blue);
                inFunctions->appendRow(func);
            }
        }

        // Adds item in hierachical order
        item->appendRow(inFiles);
        item->appendRow(inFunctions);
        item->setForeground(Qt::darkRed);
        leakItem->appendRow(item);
    }

    return leakItem;
}
