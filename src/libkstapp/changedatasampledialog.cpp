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

#include "changedatasampledialog.h"

#include "datacollection.h"
#include "datavector.h"
#include "objectstore.h"
#include "document.h"
#include "mainwindow.h"
#include "application.h"
#include "dialogdefaults.h"
#include "updatemanager.h"
#include "updateserver.h"

#include <QMessageBox>

namespace Kst {

ChangeDataSampleDialog::ChangeDataSampleDialog(QWidget *parent)
  : QDialog(parent) {
   setupUi(this);

  MainWindow::setWidgetFlags(this);

  if (MainWindow *mw = qobject_cast<MainWindow*>(parent)) {
    _store = mw->document()->objectStore();
  } else {
    // FIXME: we need the object store
    qFatal("ERROR: can't construct a ChangeDataSampleDialog without the object store");
  }

  connect(_add, SIGNAL(clicked()), this, SLOT(addButtonClicked()));
  connect(_remove, SIGNAL(clicked()), this, SLOT(removeButtonClicked()));
  connect(_removeAll, SIGNAL(clicked()), this, SLOT(removeAll()));
  connect(_addAll, SIGNAL(clicked()), this, SLOT(addAll()));

  connect(_vectorList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(availableDoubleClicked(QListWidgetItem*)));
  connect(_selectedVectorList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(selectedDoubleClicked(QListWidgetItem*)));

  connect(_vectorList, SIGNAL(itemSelectionChanged()), this, SLOT(updateButtons()));
  connect(_selectedVectorList, SIGNAL(itemSelectionChanged()), this, SLOT(updateButtons()));

  connect(_dataRange, SIGNAL(modified()), this, SLOT(modified()));

  connect(_buttonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(apply()));
  connect(_buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()), this, SLOT(OKClicked()));
  connect(_buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), this, SLOT(reject()));
  connect(UpdateServer::self(), SIGNAL(objectListsChanged()), this, SLOT(updateCurveListDialog()));

  initializeEntries();
  updateButtons();
  updateIndexEntries();

}


ChangeDataSampleDialog::~ChangeDataSampleDialog() {
}


void ChangeDataSampleDialog::show() {
  updateCurveListDialog();
  updateButtons();
  QDialog::show();
}


void ChangeDataSampleDialog::removeButtonClicked() {
  foreach (QListWidgetItem* item, _selectedVectorList->selectedItems()) {
    _vectorList->addItem(_selectedVectorList->takeItem(_selectedVectorList->row(item)));
  }

  _vectorList->clearSelection();
  updateButtons();
  updateIndexEntries();

}


void ChangeDataSampleDialog::selectedDoubleClicked(QListWidgetItem * item) {
  if (item) {
    _vectorList->addItem(_selectedVectorList->takeItem(_selectedVectorList->row(item)));
    _vectorList->clearSelection();
    updateButtons();
    updateIndexEntries();
  }
}


void ChangeDataSampleDialog::addButtonClicked() {
  foreach (QListWidgetItem* item, _vectorList->selectedItems()) {
    _selectedVectorList->addItem(_vectorList->takeItem(_vectorList->row(item)));
  }
  _selectedVectorList->clearSelection();
  updateButtons();
  updateIndexEntries();
}


void ChangeDataSampleDialog::availableDoubleClicked(QListWidgetItem * item) {
  if (item) {
    _selectedVectorList->addItem(_vectorList->takeItem(_vectorList->row(item)));
    _selectedVectorList->clearSelection();
    updateButtons();
    updateIndexEntries();
  }
}


void ChangeDataSampleDialog::updateIndexEntries() {

  // Make a list of all the data sources for all of the vectors which have been selected.
  QList<DataSourcePtr> data_sources;

  _selectedVectorList->blockSignals(true);
  _selectedVectorList->selectAll();
  QList<QListWidgetItem*> selectedItems = _selectedVectorList->selectedItems();
  _selectedVectorList->clearSelection();
  _selectedVectorList->blockSignals(false);

  for (int i = 0; i < selectedItems.size(); ++i) {
    if (DataVectorPtr vector = kst_cast<DataVector>(_store->retrieveObject(selectedItems.at(i)->text()))) {
      if (!data_sources.contains(vector->dataSource())) {
        data_sources.append(vector->dataSource());
      }
    }
  }

  QStringList index_fields;

  // make a list of fields which are provided by all data sources...
  if (data_sources.size()>0) {
    foreach (const QString &field, data_sources[0]->indexFields()) {
      bool in_all = true;
      for (int i=1; i<data_sources.size(); i++) {
        in_all = in_all && (data_sources[i]->indexFields().contains(field));
      }
      if (in_all) {
        index_fields.append(field);
      }
    }
  }

  _dataRange->updateIndexList(index_fields);
}


void ChangeDataSampleDialog::updateButtons() {
  bool valid = _selectedVectorList->count() > 0;
  _buttonBox->button(QDialogButtonBox::Ok)->setEnabled(valid);
  _buttonBox->button(QDialogButtonBox::Apply)->setEnabled(valid);
  _add->setEnabled(_vectorList->selectedItems().count() > 0);
  _addAll->setEnabled(_vectorList->count() > 0);
  _remove->setEnabled(_selectedVectorList->selectedItems().count() > 0);
  _removeAll->setEnabled(_selectedVectorList->count() > 0);
}


void ChangeDataSampleDialog::modified() {
  updateButtons();
}

void ChangeDataSampleDialog::updateCurveListDialog() {

  DataVectorList dataVectors = _store->getObjects<DataVector>();
  _vectorList->blockSignals(true);

  _vectorList->clearSelection();
  _selectedVectorList->clearSelection();

  // make sure all items in _vectorList exist in the store; remove if they don't.
  for (int i_item = 0; i_item < _vectorList->count(); i_item++) {
    bool exists=false;
    for (int i_vector = 0; i_vector<dataVectors.count(); i_vector++) {
      if (dataVectors.at(i_vector)->Name() == _vectorList->item(i_item)->text()) {
        exists = true;
        break;
      }
    }
    if (!exists) {
      QListWidgetItem *item = _vectorList->takeItem(i_item);
      delete item;
      i_item--;
    }
  }

  // make sure all items in _selectedVectorList exist in the store; remove if they don't.
  for (int i_item = 0; i_item<_selectedVectorList->count(); i_item++) {
    bool exists=false;
    for (int i_vector = 0; i_vector<dataVectors.count(); i_vector++) {
      if (dataVectors.at(i_vector)->Name() == _selectedVectorList->item(i_item)->text()) {
        exists = true;
        break;
      }
    }
    if (!exists) {
      QListWidgetItem *item = _selectedVectorList->takeItem(i_item);
      delete item;
      i_item--;
    }
  }

  // insert into _vectorList all items in store not in one of the lists.
  for (int i_vector = 0; i_vector<dataVectors.count(); i_vector++) {
    bool listed = false;
    for (int i_item = 0; i_item<_selectedVectorList->count(); i_item++) {
      if (dataVectors.at(i_vector)->Name() == _selectedVectorList->item(i_item)->text()) {
        _selectedVectorList->item(i_item)->setToolTip(dataVectors.at(i_vector)->descriptionTip());
        listed = true;
        break;
      }
    }
    for (int i_item = 0; i_item<_vectorList->count(); i_item++) {
      if (dataVectors.at(i_vector)->Name() == _vectorList->item(i_item)->text()) {
        _vectorList->item(i_item)->setToolTip(dataVectors.at(i_vector)->descriptionTip());
        listed = true;
        break;
      }
    }
    if (!listed) {
      QListWidgetItem *wi = new QListWidgetItem(dataVectors.at(i_vector)->Name());
      _vectorList->addItem(wi);
      wi->setToolTip(dataVectors.at(i_vector)->descriptionTip());
    }
  }
  _vectorList->blockSignals(false);
}


void ChangeDataSampleDialog::addAll() {
  _vectorList->selectAll();
  addButtonClicked();
}


void ChangeDataSampleDialog::removeAll() {
  _selectedVectorList->selectAll();
  removeButtonClicked();
}


void ChangeDataSampleDialog::initializeEntries() {
  _dataRange->setCountFromEnd(dialogDefaults().value("vector/countFromEnd",false).toBool());
  _dataRange->setStart(dialogDefaults().value("vector/start", 0).toInt());
  _dataRange->setReadToEnd(dialogDefaults().value("vector/readToEnd",true).toBool());
  _dataRange->setRange(dialogDefaults().value("vector/range", 1).toInt());
  _dataRange->setSkip(dialogDefaults().value("vector/skip", 0).toInt());
  _dataRange->setDoSkip(dialogDefaults().value("vector/doSkip", false).toBool());
  _dataRange->setDoFilter(dialogDefaults().value("vector/doAve",false).toBool());
  _dataRange->setStartUnits(dialogDefaults().value("vector/startUnits",tr("frames")).toString());
  _dataRange->setRangeUnits(dialogDefaults().value("vector/rangeUnits",tr("frames")).toString());
}


void ChangeDataSampleDialog::updateDefaults(QListWidgetItem* item) {
  if (!item) {
    return;
  }

  if (DataVectorPtr vector = kst_cast<DataVector>(_store->retrieveObject(item->text()))) {
    vector->readLock();

    _dataRange->setCountFromEnd(vector->countFromEOF());
    _dataRange->setStart(vector->countFromEOF() ? 0 : vector->reqStartFrame());
    _dataRange->setReadToEnd(vector->readToEOF());
    _dataRange->setRange(vector->readToEOF() ? 0 : vector->reqNumFrames());
    _dataRange->setSkip(vector->skip());
    _dataRange->setDoSkip(vector->doSkip());
    _dataRange->setDoFilter(vector->doAve());

    vector->unlock();
  }
}


void ChangeDataSampleDialog::OKClicked() {
  apply();
  accept();
}


void ChangeDataSampleDialog::apply() {
  _selectedVectorList->selectAll();
  QList<QListWidgetItem*> selectedItems = _selectedVectorList->selectedItems();

  QString start_units;
  QString range_units;
  QHash<QString, int> f0_map;
  QHash<QString, int> r_map;
  bool custom_start_index = (_dataRange->_startUnits->currentIndex() != 0) && (!_dataRange->countFromEnd());

  bool custom_range_index = (_dataRange->_rangeUnits->currentIndex() != 0) && (!_dataRange->readToEnd());

  if (!custom_start_index && !custom_range_index) { // FIXME: also for custom index.
    start_units.clear();

    // see if we have enough memory
    //FIXME: doesn't consider data objects that depend on this, and it should
    //FIXME: doesn't work under windows or mac
    double current_memory_used = 0.0;
    double memory_needed = 0.0;
    for (int i = 0; i < selectedItems.size(); ++i) {
      if (DataVectorPtr vector = kst_cast<DataVector>(_store->retrieveObject(selectedItems.at(i)->text()))) {
        current_memory_used += double(vector->length())*sizeof(double);
        long ns=0;
        if (_dataRange->readToEnd()) {
          ns = vector->fileLength() - (int)_dataRange->start();
        } else {
          ns = (int)_dataRange->range();
        }
        if (_dataRange->doSkip()) {
          ns/=_dataRange->skip();
        } else {
          ns *= vector->samplesPerFrame();
        }
        memory_needed += double(ns)*sizeof(double);
      }
    }

    double memory_available = Data::AvailableMemory();
    if (memory_needed-current_memory_used > memory_available) {
      //QApplication::restoreOverrideCursor();
      QMessageBox::warning(this, tr("Insufficient Memory"), tr("You requested to read in %1 MB of data but it seems that you only have approximately %2 MB of usable memory available.  You cannot load this much data."
                                                                   ).arg((memory_needed-current_memory_used)/(1024*1024)).arg(memory_available/(1024*1024)));
      return;
    }
  }

  start_units = _dataRange->_startUnits->currentText();
  range_units = _dataRange->_rangeUnits->currentText();

  for (int i = 0; i < selectedItems.size(); ++i) {
    if (DataVectorPtr vector = kst_cast<DataVector>(_store->retrieveObject(selectedItems.at(i)->text()))) {
      QString filename = vector->filename();
      if (!f0_map.contains(filename)) {
        int f0;
        int r;
        if (custom_start_index) {
          f0 = vector->dataSource()->indexToFrame(_dataRange->start(), start_units);
        } else if (_dataRange->countFromEnd()) {
          f0 = -1;
        } else {
          f0 = _dataRange->start();
        }
        if (custom_range_index) {
          r = _dataRange->range()*vector->dataSource()->framePerIndex(range_units);
        } else if (_dataRange->readToEnd()) {
          r = -1;
        } else {
          r = (int)_dataRange->range();
        }
        f0_map.insert(filename, f0);
        r_map.insert(filename, r);
      }
    }
  }

  for (int i = 0; i < selectedItems.size(); ++i) {
    if (DataVectorPtr vector = kst_cast<DataVector>(_store->retrieveObject(selectedItems.at(i)->text()))) {
      vector->writeLock();
      int from = f0_map.value(vector->filename());
      int range = r_map.value(vector->filename());
      vector->changeFrames( from, 
                            range,
                            _dataRange->skip(),
                            _dataRange->doSkip(),
                            _dataRange->doFilter());
      vector->setStartUnits(start_units);
      vector->setRangeUnits(start_units);
      vector->registerChange();
      vector->unlock();
    }
  }

  UpdateManager::self()->doUpdates(true);
  UpdateServer::self()->requestUpdateSignal();

  dialogDefaults().setValue("vector/range", _dataRange->range());
  dialogDefaults().setValue("vector/start", _dataRange->start());
  dialogDefaults().setValue("vector/countFromEnd", _dataRange->countFromEnd());
  dialogDefaults().setValue("vector/readToEnd", _dataRange->readToEnd());
  dialogDefaults().setValue("vector/skip", _dataRange->skip());
  dialogDefaults().setValue("vector/doSkip", _dataRange->doSkip());
  dialogDefaults().setValue("vector/doAve", _dataRange->doFilter());
  dialogDefaults().setValue("vector/startUnits", _dataRange->_startUnits->currentText());
  dialogDefaults().setValue("vector/rangeUnits", _dataRange->_rangeUnits->currentText());

  updateCurveListDialog();
  kstApp->mainWindow()->document()->setChanged(true);
}


}

// vim: ts=2 sw=2 et
