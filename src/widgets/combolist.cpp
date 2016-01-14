#include "combolist.h"
#include <QBoxLayout>

ComboList::ComboList(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout;
    QHBoxLayout *titleLayout = new QHBoxLayout;
    setLayout(layout);

    label = new QLabel(this);
    combo = new QComboBox(this);
    combo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    combo->setStyleSheet("QComboBox {padding: 4px}");
    qobject_cast<QListView*>(combo->view())->setSpacing(1);
    list = new QListWidget(this);

    titleLayout->addWidget(label);
    titleLayout->addWidget(combo);
    layout->addLayout(titleLayout);
    layout->addWidget(list);
    layout->setMargin(0);

    list->setContextMenuPolicy(Qt::ActionsContextMenu);

    connect(list, SIGNAL(currentRowChanged(int)), this, SIGNAL(itemChanged(int)));
    connect(combo, SIGNAL(currentTextChanged(QString)), this, SLOT(setCurrentGroup(QString)));
}

void ComboList::addGroup(QString name)
{
    combo->addItem(name);
    refresh();
}

void ComboList::addGroup(QString name, QIcon icon)
{
    combo->addItem(icon, name);
    refresh();
}

void ComboList::addItem(QString group, QString name, QVariant data)
{
    items.insert(group, Item(name, data));
    refresh();
}

void ComboList::refresh()
{
    setCurrentGroup(currentGroupText());
}

void ComboList::setCurrentGroup(QString name)
{
    int tempId;

    if (combo->findText(name) != -1) {
        tempId = list->currentRow();
    }
    else {
        name = combo->itemText(0);
        tempId = 0;
    }

    combo->setCurrentText(name);
    list->clear();

    QMultiMap<QString, Item>::iterator i = items.find(name);
    while (i != items.end() && i.key() == name) {
        list->insertItem(0, i.value().name);
        list->item(0)->setData(Qt::UserRole, i.value().data);
        ++i;
    }

    // Restore the last selection:
    list->setCurrentRow(tempId);

    emit groupChanged(combo->currentIndex());
    emit itemChanged(tempId);
}
