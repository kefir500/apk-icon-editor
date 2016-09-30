#include "authors.h"
#include "globals.h"
#include <QBoxLayout>
#include <QFile>
#include <QTextStream>
#include <QTextCodec>

Authors::Authors(QWidget *parent) : QDialog(parent)
{
    resize(Gui::Screen::scaled(700, 260));
    setWindowTitle(tr("Authors"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    list = new QListWidget(this);
    text = new QTextBrowser(this);
    text->setOpenExternalLinks(true);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(list);
    layout->addWidget(text);
    layout->setStretch(0, 1);
    layout->setStretch(1, 2);

    connect(list, SIGNAL(currentTextChanged(QString)), this, SLOT(setCategory(QString)));

    parse();
    list->setCurrentRow(0);
}

void Authors::setCategory(QString category)
{
    text->setText(authors[category]);
}

void Authors::parse()
{
    Q_UNUSED(QT_TR_NOOP("Authors"));
    Q_UNUSED(QT_TR_NOOP("Translators"));
    Q_UNUSED(QT_TR_NOOP("Contributors"));
    Q_UNUSED(QT_TR_NOOP("Donators"));
    Q_UNUSED(QT_TR_NOOP("Special Thanks"));
    Q_UNUSED(QT_TR_NOOP("Testers"));

    QString category;
    QFile inputFile(Path::App::shared() + "authors.txt");
    if (inputFile.open(QIODevice::ReadOnly)) {
        QTextStream in(&inputFile);
        in.setCodec(QTextCodec::codecForName("UTF-8"));
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (line.endsWith(':')) {
                line.chop(1);
                category = tr(qPrintable(line));
                list->addItem(category);
            }
            else {
                if (!line.isEmpty()) {
                    QRegExp rx("\\(www.(.+)\\)");
                    rx.setMinimal(true);
                    line = line.replace(rx, "(<a href=\"http://www.\\1\">www.\\1</a>)");
                    authors[category].append(line + "<br>");
                }
            }
        }
        inputFile.close();
    }
}
