/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "dialog.h"

#include <QDebug>

#include "dialogpage.h"

#include <QHash>
#include <QPushButton>

namespace Kst {

Dialog::Dialog(QWidget *parent)
  : QDialog(parent), _allowApply(false) {

  setupUi(this);

  _saveAsDefault->hide();
  _applyToExisting->hide();

  extensionWidget()->hide();

  connect(_listWidget, SIGNAL(itemClicked(QListWidgetItem*)),
          this, SLOT(selectPageForItem(QListWidgetItem*)));

  connect(_buttonBox, SIGNAL(clicked(QAbstractButton*)),
          this, SLOT(buttonClicked(QAbstractButton*)));

  setAttribute(Qt::WA_DeleteOnClose);

  resize(minimumSizeHint());

  _saveAsDefault->setProperty("si","Save as default");
  _applyToExisting->setProperty("si","Apply to existing objects");
#if defined(__QNX__) || defined(__ANDROID__)
  // Mobile environments don't have window managers, and so dialogs
  // are not a native concept. We may consider adding a "Back"
  // button to dialogs on mobile platform...
  //
  // In the meantime, dialogs should be fullscreen. A dialog without
  // borders looks really bad, and getting the size right is difficult.
  setWindowState(Qt::WindowFullScreen);
#endif
}


Dialog::~Dialog() {
}


void Dialog::addDialogPage(DialogPage *page, bool addMinSize) {
  connect(page, SIGNAL(modified()), this, SLOT(modified()));
  connect(this, SIGNAL(apply()), page, SIGNAL(apply()));
  QListWidgetItem *item = new QListWidgetItem(page->pageIcon(), page->pageTitle(), _listWidget);
  _listWidget->addItem(item);
  _stackedWidget->addWidget(page);
  _itemHash.insert(item, page);
  int itemWidth = _listWidget->visualItemRect(item).width() + 4;
  if (addMinSize && _listWidget->width() < itemWidth) {
    _listWidget->setMinimumSize(itemWidth, _listWidget->size().height());
  }
}


void Dialog::removeDialogPage(DialogPage *page) {
  disconnect(page, SIGNAL(modified()), this, SLOT(modified()));
  disconnect(this, SIGNAL(apply()), page, SIGNAL(apply()));
  QList<QListWidgetItem*> items = _listWidget->findItems(page->pageTitle(), Qt::MatchExactly);
  foreach (QListWidgetItem* item, items) {
    _itemHash.remove(item);
    _listWidget->takeItem(_listWidget->row(item));
    _stackedWidget->removeWidget(page);
  }
  resetListWidget();
}


void Dialog::resetListWidget() {
  int extraSize = _listWidget->width() - 100;
  _listWidget->setMaximumWidth(100);
  _listWidget->setMinimumWidth(100);
  if (extraSize > 0 ) {
     QList <int> splitterSize;
     splitterSize << 100;
     splitterSize << _splitter->sizes()[1] + extraSize;
     _splitter->setSizes(splitterSize);
  }
}


DialogPage* Dialog::getDialogPage(const QString& pageName) {
  QList<QListWidgetItem*> items = _listWidget->findItems(pageName, Qt::MatchExactly);
  if (!items.empty()) {
    return _itemHash.value(items[0]);
  }
  return 0;
}


void Dialog::selectDialogPage(DialogPage *page) {
  QList<QListWidgetItem*> items = _listWidget->findItems(page->pageTitle(), Qt::MatchExactly);
  foreach (QListWidgetItem* item, items) {
    _listWidget->setCurrentItem(item);
    selectPageForItem(item);
  }
}


void Dialog::setVisible(bool visible) {

  _listWidget->setVisible(_itemHash.count() > 1);
  _buttonBox->button(QDialogButtonBox::Apply)->setEnabled(false);

  QDialog::setVisible(visible);
}


QWidget *Dialog::leftCustomWidget() const {
  return _leftCustom;
}


QWidget *Dialog::rightCustomWidget() const {
  return _rightCustom;
}


QWidget *Dialog::topCustomWidget() const {
  return _topCustom;
}


QWidget *Dialog::bottomCustomWidget() const {
  return _bottomCustom;
}


QWidget *Dialog::extensionWidget() const {
  return _extensionWidget;
}


QDialogButtonBox *Dialog::buttonBox() const {
 return _buttonBox;
}


void Dialog::selectPageForItem(QListWidgetItem *item) {
  if (_itemHash.contains(item))
    _stackedWidget->setCurrentWidget(_itemHash.value(item));
}


void Dialog::buttonClicked(QAbstractButton *button) {
  QDialogButtonBox::StandardButton std = _buttonBox->standardButton(button);
  switch(std) {
  case QDialogButtonBox::Ok:
    emit apply();
    emit ok();
    break;
  case QDialogButtonBox::Apply:
    if (!_allowApply) {
      _buttonBox->button(QDialogButtonBox::Apply)->setEnabled(false);
    }
    emit apply();
    break;
  case QDialogButtonBox::Cancel:
    emit cancel();
    break;
  default:
    break;
  }
}


void Dialog::setAlwaysAllowApply(const bool allow) {
  _allowApply = allow;
  if (!allow) {
    _buttonBox->button(QDialogButtonBox::Apply)->setEnabled(false);
  }
}


void Dialog::modified() {
  _buttonBox->button(QDialogButtonBox::Apply)->setEnabled(true);
}

}

// vim: ts=2 sw=2 et
