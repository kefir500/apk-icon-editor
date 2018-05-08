#include "titlesmodel.h"
#include <QFile>
#include <QTextStream>
#include <QDirIterator>
#include <QRegularExpression>
#include <QCoreApplication>
#include <QDebug>

TitlesModel::~TitlesModel()
{
    qDeleteAll(titles);
}

void TitlesModel::add(const QString &filepath, const QString &key)
{
    if (key.isEmpty()) {
        return;
    }
    QFile xml(filepath);
    if (xml.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream stream(&xml);
        stream.setCodec("UTF-8");
        QDomDocument xmlDocument;
        xmlDocument.setContent(stream.readAll());

        // Iterate through child elements:

        QDomNodeList xmlNodes = xmlDocument.firstChildElement("resources").childNodes();
        for (int i = 0; i < xmlNodes.count(); ++i) {
            QDomElement xmlNode = xmlNodes.at(i).toElement();
            if (Q_LIKELY(!xmlNode.isNull())) {

                // Find application label nodes:

                if (xmlNode.nodeName() == "string" && xmlNode.attribute("name") == key) {
                    beginInsertRows(QModelIndex(), rowCount(), rowCount());
                        titles.append(new String(filepath, xmlNode));
                    endInsertRows();
                }
            } else {
                qWarning() << "CRITICAL: Element \"resources\" contains non-element child nodes";
            }
        }
    }
}

bool TitlesModel::save() const
{
    foreach (String *title, titles) {
        if (title->wasModified()) {
            title->save();
        }
    }
    return true;
}

bool TitlesModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole) {
        const int row = index.row();
        String *title = titles.at(row);
        title->setValue(value.toString());
        emit dataChanged(index, index);
        return true;
    }
    return false;
}

QVariant TitlesModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid()) {
        String *title = titles.at(index.row());
        QFileInfo fi(title->getFilePath());
        const QString qualifiers = fi.path().split('/').last();
        if (role == Qt::DisplayRole || role == Qt::EditRole) {
            switch (index.column()) {
            case Value:
                return title->getValue();
            case Language: {
                QRegularExpression regex("-([a-z]{2}(-r[A-Z]{2})?)(?:-|$)");
                QString locale = regex.match(qualifiers).captured(1).replace("-r", "-");
                QString native = QLocale(locale).nativeLanguageName();
                if (!native.isEmpty()) {
                    native[0] = native[0].toUpper();
                }
                return native;
            }
            case Path:
                QFileInfo fi(title->getFilePath());
                return QString("%1/%2").arg(qualifiers, fi.fileName());
            }
        }
    }
    return QVariant();
}

QVariant TitlesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
            case Value:    return QCoreApplication::translate("MainWindow", "Application Name");
            case Language: return QCoreApplication::translate("MainWindow", "Language");
            case Path:     return QCoreApplication::translate("TitlesModel", "Resource");
        }
    }
    return QVariant();
}

QModelIndex TitlesModel::index(int row, int column, const QModelIndex &parent) const
{
    if (Q_UNLIKELY(parent.isValid())) {
        return QModelIndex();
    }
    return createIndex(row, column);
}

int TitlesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return titles.count();
}

int TitlesModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return ColumnCount;
}

Qt::ItemFlags TitlesModel::flags(const QModelIndex &index) const
{
    if (index.column() == 0) {
        return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
    } else {
        return QAbstractItemModel::flags(index);
    }
}
