#include "reader.h"

reader::reader(QString _path) {
    QFile file(_path);

    if(!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Error reading file " << file.errorString();
        return;
    }

    QTextStream in(&file);
    content = in.readLine();
    new_line_content = content;

    while(!in.atEnd()) {
        const QString line = in.readLine();
        content += line;
        new_line_content += line + "\n";
    }
}

QList<Leak> reader::get_leaks() {
    static QRegularExpression leakExp ("==\\d*==(\\s)\\d*(\\s)[aA-zZ]*\\s*[aA-zZ]*\\s*\\d*\\s*[aA-zZ]*(\\(\\d*\\s*[aA-zZ]*,?\\s*\\d*\\s*[aA-zZ]*\\))?\\s*byte\\s*[aA-zZ]*\\s*\\d*\\s*[aA-zZ]*\\s*[aA-zZ]*\\s*[aA-zZ]*\\s*[aA-zZ]*\\s*\\d*\\s*[aA-zZ]*\\s*\\d*\\s*([aA-zZ]*\\s*[aA-zZ]*\\s*\\d*\\s*[aA-zZ]*\\s*\\d*\\s)?.+?(?=(==\\d*==\\s*==\\d*==)|$)");
    static QRegularExpressionMatchIterator matchIterator = leakExp.globalMatch(content);

    QList<Leak> matches;
    for (const QRegularExpressionMatch &match : leakExp.globalMatch(content)) {
        matches << match.captured(0);
    }

    return matches;
}

QString reader::get_summary() const {
    static QRegularExpression heapSummaryExp("==\\d*==\\sHEAP\\sSUMMARY:(?s)(.*?)==\\d*==\\s==\\d*==");
    static QRegularExpression leakSummaryExp("==\\d*==\\sLEAK\\sSUMMARY:(?s)(.*?)==\\d*==\\s==\\d*==");
    static QRegularExpression errorSummaryExp("==\\d*==\\sERROR\\sSUMMARY:\\s*\\d*\\s*[aA-zZ]*\\s*[aA-zZ]*\\s*\\d*\\s*[aA-zZ]*\\s*\\([aA-zZ]*:\\s*\\d*\\s*[aA-zZ]*\\s*\\d*\\)");

    QString summary;
    for (const QRegularExpressionMatch &match : heapSummaryExp.globalMatch(content)) {
        summary.append(match.captured(0));
    }

    summary.append("\n\n");

    for (const QRegularExpressionMatch &match : leakSummaryExp.globalMatch(content)) {
        summary.append(match.captured(0));
    }

    summary.append("\n\n");

    for (const QRegularExpressionMatch &match : errorSummaryExp.globalMatch(content)) {
        summary.append(match.captured(0));
    }

    static QRegularExpression re("==\\d*==");
    return summary.replace(re, "").replace("    ", "\n").trimmed();
}

QString reader::get_command() const {
    static QRegularExpression commandExp("Command: (\\S+)(?!==\\d*==)");
    static QRegularExpression re("==\\d*==");
    return "Val-gui - " + commandExp.match(content).captured(0).replace(re,"").replace("Command:","").trimmed();
}
