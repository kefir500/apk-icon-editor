#include "about.h"
#include "authors.h"
#include "globals.h"
#include <QBoxLayout>
#include <QPushButton>
#include <QDialogButtonBox>

About::About(QWidget *parent) : QDialog(parent)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    QLabel *icon = new QLabel(this);
    icon->setPixmap(QPixmap(":/gfx/logo-about.png"));
    icon->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

    textApp = new QLabel(this);
    textLinks = new QLabel(this);
    textReqs = new QLabel(this);
    textApp->setOpenExternalLinks(true);
    textLinks->setOpenExternalLinks(true);
    textReqs->setOpenExternalLinks(true);

#ifdef Q_OS_WIN
    QFont font("Segoe UI", 9);
    textApp->setFont(font);
    textLinks->setFont(font);
    textReqs->setFont(font);
#endif

    QFrame *line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    QDialogButtonBox *buttons = new QDialogButtonBox(this);
    btnAuthors = new QPushButton(this);
    buttons->addButton(QDialogButtonBox::Ok);
    buttons->addButton(btnAuthors, QDialogButtonBox::AcceptRole);

    QHBoxLayout *layout = new QHBoxLayout(this);
    QVBoxLayout *texts = new QVBoxLayout;
    layout->setSizeConstraint(QLayout::SetFixedSize);
    layout->setSpacing(10);
    layout->addWidget(icon);
    layout->addLayout(texts);
    texts->addWidget(textApp);
    texts->addWidget(textLinks);
    texts->addWidget(line);
    texts->addWidget(textReqs);
    texts->addWidget(buttons);

    connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
    connect(btnAuthors, SIGNAL(clicked()), this, SLOT(accept()));
    connect(btnAuthors, SIGNAL(clicked()), this, SLOT(showAuthors()));

    retranslate();
    setVersions("...", "...", "...");
}

void About::setVersions(QString jre, QString jdk, QString apktool)
{
    const QString CELL = "<td style='padding-right: 24px'>%1</td>";
    const QString APKTOOL = !apktool.isEmpty() ? apktool : "---";
    const QString JRE = !jre.isEmpty() ? jre : QString("<a href='%1'>%2</a>").arg(Url::JRE, QApplication::translate("Apk", "Download"));
    const QString JDK = !jdk.isEmpty() ? jdk : QString("<a href='%1'>%2</a>").arg(Url::JDK, QApplication::translate("Apk", "Download"));

    textReqs->setContentsMargins(0, 0, 0, 0);
    textReqs->setText(
        "<table><tr>" +
            CELL.arg("Qt: %1").arg(QT_VERSION_STR) +
            CELL.arg("Apktool: %1").arg(APKTOOL) +
            CELL.arg("JRE: %1").arg(JRE) +
            CELL.arg("JDK: %1").arg(JDK) +
        "</tr></table>"
    );
}

void About::showAuthors()
{
    Authors authors(this);
    authors.exec();
}

void About::retranslate()
{
    setWindowTitle(tr("About"));

    textApp->setText(
        QString("<h3>%1 v%2</h3>").arg(APP, VER) +
        tr("Built on: %1 - %2").arg(__DATE__, __TIME__) + "<br>" +
        tr("Author: %1").arg("Alexander Gorishnyak") + "<br>" +
        tr("License") + ": <a href='http://www.gnu.org/licenses/gpl-3.0.html'>GNU GPL v3.0</a>"
    );

    const QString LINK = "<tr><td style='padding-right: 4px' align='right'><a href='%1'>%2</a></td><td>%3</td></tr>";
    textLinks->setText(
        "<table>" +
            LINK.arg(Url::WEBSITE, tr("Visit Website"), tr("Visit our official website.")) +
            LINK.arg(Url::CONTACT, tr("Report a Bug"), tr("Found a bug? Let us know so we can fix it!")) +
            LINK.arg(Url::TRANSLATE, tr("Help Translate"), tr("Join our translation team on Transifex.")) +
            LINK.arg(QString("file:///%1/versions.txt").arg(Path::App::dir()), tr("Version History"), tr("List of changes made to the project.")) +
        "</table>"
    );

    btnAuthors->setText(tr("Authors"));
}
