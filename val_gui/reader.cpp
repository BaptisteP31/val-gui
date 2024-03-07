#include "reader.h"

reader::reader(QString _path) {
    QFile file(_path);

    if(!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Error reading file " << file.errorString();
        return;
    }

    QTextStream in(&file);
    content = in.readLine();

    while(!in.atEnd()) {
        content += in.readLine();
    }
}

QList<Leak> reader::get_leaks() {
    static QRegularExpression leakExp ("==\\d*==(\\s)\\d*(\\s)bytes(\\s)in*\\d*(\\s)\\d*(\\s)[a-zA-Z]*(\\s)[a-zA-Z]*(\\s)[a-zA-Z]*(\\s)[a-zA-Z]*(\\s)[a-zA-Z]*(\\s)[a-zA-Z]*(\\s)[a-zA-Z]*(\\s)\\d*(\\s)[a-zA-Z]*(\\s)\\d*(==\\d*==)(\\s*)(?:(?!\\s*==6561==\\s*==6561==).)*");
    static QRegularExpressionMatchIterator matchIterator = leakExp.globalMatch(content);

    QList<Leak> matches;
    for (const QRegularExpressionMatch &match : leakExp.globalMatch(content)) {
        if(match.hasMatch())
            matches << match.captured(0);
    }

    return matches;
}

QString reader::get_summary() const {
    static QRegularExpression heapSummaryExp("==\\d*==\\sHEAP\\sSUMMARY:(?s)(.*?)==\\d*==\\s==\\d*==");
    static QRegularExpression leakSummaryExp("==\\d*==\\sLEAK\\sSUMMARY:(?s)(.*?)==\\d*==\\s==\\d*==");

    QString summary;
    for (const QRegularExpressionMatch &match : heapSummaryExp.globalMatch(content)) {
        summary.append(match.captured(0));
    }

    summary.append("\n\n");

    for (const QRegularExpressionMatch &match : leakSummaryExp.globalMatch(content)) {
        summary.append(match.captured(0));
    }


    static QRegularExpression re("==\\d*==");
    return summary.replace(re, "").replace("    ", "\n").trimmed();
}
