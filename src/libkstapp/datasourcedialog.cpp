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

#include "datasourcedialog.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QDialogButtonBox>

namespace Kst {

DataSourceDialog::DataSourceDialog(DataDialog::EditMode mode, DataSourcePtr dataSource, QWidget *parent)
  : QDialog(parent), _dataSource(dataSource) {

  setWindowTitle(QString("Configure %1").arg(_dataSource->fileType()));

  QVBoxLayout *layout = new QVBoxLayout(this);

  _dataSource->readLock();
  _configWidget = _dataSource->configWidget();
  connect(this, SIGNAL(ok()), _configWidget, SLOT(save()));
  connect(this, SIGNAL(cancel()), _configWidget, SLOT(cancel()));

  if (mode == DataDialog::Edit) {
    // Why would we want a source to not be reuseable?
    // Note: this is the only place where reuse is ever disabled...
    //connect(this, SIGNAL(ok()), this, SLOT(disableReuse()));
  }

  _dataSource->unlock();

  _configWidget->setDialogParent(this);
  layout->addWidget(_configWidget);

  _buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  layout->addWidget(_buttonBox);

  connect(_buttonBox, SIGNAL(clicked(QAbstractButton*)),
          this, SLOT(buttonClicked(QAbstractButton*)));

  setLayout(layout);

  setMaximumSize(QSize(1280, 1024));
  resize(minimumSizeHint());
}


DataSourceDialog::~DataSourceDialog() {
}


void DataSourceDialog::disableReuse() {
  _dataSource->disableReuse();
}

void DataSourceDialog::buttonClicked(QAbstractButton *button) {
  QDialogButtonBox::StandardButton std = _buttonBox->standardButton(button);
  switch(std) {
  case QDialogButtonBox::Ok:
    if (_configWidget->isOkAcceptabe()) {
      emit ok();
      accept();
    }
    break;
  case QDialogButtonBox::Cancel:
    emit cancel();
    reject();
    break;
  default:
    break;
  }
}

}

// vim: ts=2 sw=2 et
