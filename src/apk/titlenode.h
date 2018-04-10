#ifndef STRING_H
#define STRING_H

#include <QDomDocument>

class String {

public:
    String(const QString &filename, const QDomNode &node);

    void setValue(const QString &value);
    QString getValue() const;
    QString getFilePath() const;
    bool wasModified() const;
    void save() const;

private:
    QString filename;
    QDomDocument document;
    QDomNode node;
    bool modified;
};

#endif // STRING_H
