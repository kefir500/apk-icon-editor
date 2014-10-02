/// \file combolist.h
/// \brief This file contains #ComboList widget declaration.

#ifndef COMBOLIST_H
#define COMBOLIST_H

#include <QLabel>
#include <QComboBox>
#include <QListWidget>

/// \brief This widget combines #combo box and #list box widgets.
///
/// The ComboList widget provides \e "group-item" bindings.<br>
/// \e Groups are displayed in the #combo widget.<br>
/// \e Items (dependents of \e groups) are displayed in the #list box widget.<br>
/// Bindings themselves are stored in the #items map.
///
/// \image html combolist.png

class ComboList : public QWidget {
    Q_OBJECT

private:
    /// This nested class describes a #ComboList \e item used in #list box.
    struct Item {
        QString name;   ///< Item title.
        QVariant data;  ///< Item custom data.
        Item(QString _name, QVariant _data) : name(_name), data(_data) { }
    };

    QLabel *label;      ///< This widget displays text in front of #combo box.
    QComboBox *combo;   ///< This widget visualizes primary \e groups.
    QListWidget *list;  ///< This widget visualizes secondary \e items.

    QMultiMap<QString, Item> items; ///< This variable holds "group-item" bindings.

public:
    /// Get the currently selected \e item index.
    /// \return Index of the selected \e item.
    int currentItemIndex() const { return list->currentRow(); }

    /// Get the currently selected \e item text.
    /// \return Text of the selected \e item.
    QString currentItemText() const { return list->currentItem()->text(); }

    /// Get the currently selected \e item custom data.
    /// \return Custom data of the selected \e item.
    QVariant currentItemData() const { return list->currentItem()->data(Qt::UserRole); }

    /// Get the currently selected \e group index.
    /// \return Index of the selected \e group.
    int currentGroupIndex() const { return combo->currentIndex(); }

    /// Get the currently selected \e group text.
    /// \return Text of the selected \e group.
    QString currentGroupText() const { return combo->currentText(); }

    /// Set text of the #label (in front of #combo box).
    /// \param[in] title Text to display.
    void setLabelText(QString title) { label->setText(title); }

    /// Set visiblity of the \e item.
    /// \param[in] id Index of the \e item to show/hide.
    /// \param[in] visible Shows the \e item if \c TRUE, otherwise hides it.
    void setItemVisible(int id, bool visible) { list->item(id)->setHidden(!visible); }

    /// Add new group.
    /// \param[in] name Title of the new \e group.
    void addGroup(QString name);

    /// Add new group.
    /// \param[in] name Title of the new \e group.
    /// \param[in] icon Icon of the new \e group.
    void addGroup(QString name, QIcon icon);

    /// Add new item to the existing \e group.
    /// \param[in] group Name of the existing \e group.
    /// \param[in] name Title of the new \e item.
    /// \param[in] data Custom user data for the new \e item.
    void addItem(QString group, QString name, QVariant data = NULL);

    /// Refresh item #list (according to #items bindings).
    void refresh();

    /// Add the popup menu \c actions to the #list box.
    void addActions(QList<QAction *> actions) { list->addActions(actions); }

    explicit ComboList(QWidget *parent);

public slots:
    /// Set the current group and refresh item #list (according to #items bindings).
    /// \param[in] name Name of the group.
    void setCurrentGroup(QString name);

    /// Set the current item.
    /// \param[in] id ID of the item to acvtivate.
    void setCurrentItem(int id) { list->setCurrentRow(id); emit itemChanged(id); }

signals:
    /// This signal is emitted when item (#list box selection) is changed.
    /// \param id Index of the active item.
    void itemChanged(int id) const;

    /// This signal is emitted when group (#combo box selection) is changed.
    /// \param id Index of the active group.
    void groupChanged(int id) const;
};

#endif // COMBOLIST_H
