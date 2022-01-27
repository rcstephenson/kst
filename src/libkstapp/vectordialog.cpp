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
#include "vectordialog.h"

#include "application.h"
#include "dialogpage.h"
#include "datasourcedialog.h"
#include "editmultiplewidget.h"

#include "datavector.h"
#include "generatedvector.h"

#include "datacollection.h"
#include "document.h"
#include "objectstore.h"
#include "datasourcepluginmanager.h"
#include "dialogdefaults.h"
#include "debug.h"

#include <QDir>
#include <QThreadPool>

namespace Kst {

VectorTab::VectorTab(ObjectStore *store, QWidget *parent)
  : DataTab(parent), validating(false), _mode(DataVector), _store(store), _initField(QString()), _requestID(0), _valid(false) {

  setupUi(this);
  setTabTitle(tr("Vector"));

  connect(_generatedVectorGroup, SIGNAL(clicked(bool)), this, SLOT(generateClicked()));
  connect(_dataVectorGroup, SIGNAL(clicked(bool)), this, SLOT(readFromSourceClicked()));
  connect(_fileName, SIGNAL(changed(QString)), this, SLOT(fileNameChanged(QString)));
  connect(_configure, SIGNAL(clicked()), this, SLOT(showConfigWidget()));
  connect(_field, SIGNAL(editTextChanged(QString)), this, SIGNAL(fieldChanged()));

  connect(_dataRange, SIGNAL(modified()), this, SIGNAL(modified()));
  connect(_numberOfSamples, SIGNAL(valueChanged(int)), this, SIGNAL(modified()));
  connect(_from, SIGNAL(textChanged(QString)), this, SIGNAL(modified()));
  connect(_to, SIGNAL(textChanged(QString)), this, SIGNAL(modified()));

  // embed data range in the data source box
  _dataRange->groupBox2->setFlat(true);
  _dataRange->groupBox2->setTitle("");
  int top_margin;
  _dataRange->groupBox2->layout()->getContentsMargins(NULL,&top_margin,NULL,NULL);
  _dataRange->groupBox2->layout()->setContentsMargins(0,top_margin,0,0); 

  _connect->setVisible(false);

  _updateBox->addItem(tr("Time Interval", "update periodically"));
  _updateBox->addItem(tr("Change Detection", "update when a change is detected"));
  _updateBox->addItem(tr("No Update", "do not update the file"));
  updateUpdateBox();
  connect(_updateBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateTypeActivated(int)));
  connect(_updateBox, SIGNAL(currentIndexChanged(int)), this, SIGNAL(modified()));
}


VectorTab::~VectorTab() {
}

void VectorTab::updateUpdateBox()
{
  if (_dataSource) {
    _updateBox->setEnabled(_mode == DataVector);
    switch (_dataSource->updateType()) {
      case DataSource::Timer: _updateBox->setCurrentIndex(0); break;
      case DataSource::File:  _updateBox->setCurrentIndex(1); break;
      case DataSource::None:  _updateBox->setCurrentIndex(2); break;
      default: break;
    };
  } else {
    _updateBox->setEnabled(false);
  }
}

void VectorTab::updateTypeActivated(int idx)
{
  if (!_dataSource) {
    _updateBox->setEnabled(false);
    return;
  }
  switch (idx) {
    _updateBox->setEnabled(true);
    case 0: _dataSource->startUpdating(DataSource::Timer); break;
    case 1: _dataSource->startUpdating(DataSource::File);  break;
    case 2: _dataSource->startUpdating(DataSource::None);  break;
    default: break;
  };
}

DataSourcePtr VectorTab::dataSource() const {
  return _dataSource;
}


void VectorTab::setDataSource(DataSourcePtr dataSource) {
  _dataSource = dataSource;
}


void VectorTab::setVectorMode(VectorMode mode) {
  _mode = mode;
  if (mode == DataVector) {
    _dataVectorGroup->setChecked(true);
    _dataRange->setEnabled(true);
    _generatedVectorGroup->setChecked(false);
    _field->setEnabled(_valid);
    _configure->setEnabled(_valid);
    updateUpdateBox();
  } else {
    _generatedVectorGroup->setChecked(true);
    _dataVectorGroup->setChecked(false);
    _dataRange->setEnabled(false);
  }
}


QString VectorTab::file() const {
  return _fileName->file();
}


void VectorTab::setFile(const QString &file) {
  _fileName->setFile(file);
}


QString VectorTab::field() const {
  return _field->currentText();
}


void VectorTab::setField(const QString &field) {
  _initField = field; // for delayed index setting
  _field->setCurrentIndex(_field->findText(field));
}


void VectorTab::setFieldList(const QStringList &fieldList) {
  _field->clear();
  _field->addItems(fieldList);
}


DataRange *VectorTab::dataRange() const {
  return _dataRange;
}


qreal VectorTab::from() const {
  return _from->text().toDouble();
}


bool VectorTab::fromDirty() const {
  return (!_from->text().isEmpty());
}


void VectorTab::setFrom(qreal from) {
  _from->setText(QString::number(from));
}


qreal VectorTab::to() const {
  return _to->text().toDouble();
}


bool VectorTab::toDirty() const {
  return (!_to->text().isEmpty());
}


void VectorTab::setTo(qreal to) {
  _to->setText(QString::number(to));
}


int VectorTab::numberOfSamples() const {
  return _numberOfSamples->value();
}


bool VectorTab::numberOfSamplesDirty() const {
  return (!_numberOfSamples->text().isEmpty());
}


void VectorTab::setNumberOfSamples(int numberOfSamples) {
  _numberOfSamples->setValue(numberOfSamples);
}

void VectorTab::generateClicked() {
  if (_generatedVectorGroup->isChecked())
    setVectorMode(GeneratedVector);
  else
    setVectorMode(DataVector);

  emit sourceChanged();
}

void VectorTab::readFromSourceClicked() {
  if (_dataVectorGroup->isChecked())
    setVectorMode(DataVector);
  else
    setVectorMode(GeneratedVector);

  emit sourceChanged();
}

void VectorTab::hideGeneratedOptions() {
  _generatedVectorGroup->setVisible(false);
  _dataVectorGroup->setCheckable(false);
  _dataVectorGroup->setTitle("");
  _dataVectorGroup->setFlat(true);
  int top_margin;
  _dataVectorGroup->layout()->getContentsMargins(NULL,&top_margin,NULL,NULL);
  _dataVectorGroup->layout()->setContentsMargins(0,top_margin,0,0);
}


void VectorTab::hideDataOptions() {
  _dataVectorGroup->setVisible(false);
  _dataRange->setVisible(false);
  _generatedVectorGroup->setCheckable(false);
  _generatedVectorGroup->setTitle("");
  _generatedVectorGroup->setFlat(true);
  int top_margin;
  _generatedVectorGroup->layout()->getContentsMargins(NULL,&top_margin,NULL,NULL);
  _generatedVectorGroup->layout()->setContentsMargins(0,top_margin,0,0);
}


void VectorTab::clearTabValues() {
  _numberOfSamples->clear();
  _from->clear();
  _to->clear();
  _dataRange->clearValues();
}


void VectorTab::enableSingleEditOptions(bool enabled) {
  _dataVectorGroup->setEnabled(enabled);
}


void VectorTab::sourceValid(QString filename, int requestID) {
  if (_requestID != requestID) {
    return;
  }
  _valid = true;
  _dataSource = DataSourcePluginManager::findOrLoadSource(_store, filename);
  _field->setEnabled(true);

  _dataSource->readLock();

  _field->addItems(_dataSource->vector().list());
  if (!_initField.isEmpty()) {
    setField(_initField);
  }
  _field->setEditable(!_dataSource->vector().isListComplete());
  _configure->setEnabled(_dataSource->hasConfigWidget() && (_mode == DataVector));

  updateUpdateBox();
  updateIndexList(_dataSource);

  _dataSource->unlock();


  validating = false;

  _store->cleanUpDataSourceList();
  _field->setEnabled(_mode == DataVector);

  emit sourceChanged();
}


void VectorTab::fileNameChanged(const QString &file) {
  _field->clear();
  _field->setEnabled(false);
  _configure->setEnabled(false);
  _valid = false;

  clearIndexList();
  emit sourceChanged();

  _requestID += 1;
  ValidateDataSourceThread *validateDSThread = new ValidateDataSourceThread(file, _requestID);
  connect(validateDSThread, SIGNAL(dataSourceValid(QString,int)), this, SLOT(sourceValid(QString,int)));
  validating = true;
  QThreadPool::globalInstance()->start(validateDSThread);
}


void VectorTab::showConfigWidget() {
  QPointer<DataSourceDialog> dialog = new DataSourceDialog(dataDialog()->editMode(), _dataSource, this);
  if ( dialog->exec() == QDialog::Accepted ) {
    fileNameChanged(_dataSource->fileName());
  }
  delete dialog;
}

VectorDialog::VectorDialog(ObjectPtr dataObject, QWidget *parent)
  : DataDialog(dataObject, parent, false) {

  if (editMode() == Edit)
    setWindowTitle(tr("Edit Vector"));
  else
    setWindowTitle(tr("New Vector"));

  Q_ASSERT(_document);
  _vectorTab = new VectorTab(_document->objectStore(), this);
  addDataTab(_vectorTab);

  if (editMode() == Edit) {
    configureTab(dataObject);
  } else {
    configureTab(0);
  }

  connect(_vectorTab, SIGNAL(sourceChanged()), this, SLOT(updateButtons()));
  connect(_vectorTab, SIGNAL(fieldChanged()), this, SLOT(updateButtons()));

  connect(this, SIGNAL(editMultipleMode()), this, SLOT(editMultipleMode()));
  connect(this, SIGNAL(editSingleMode()), this, SLOT(editSingleMode()));

  connect(_vectorTab, SIGNAL(modified()), this, SLOT(modified()));
  connect(_vectorTab, SIGNAL(destroyed()), kstApp->mainWindow(), SLOT(cleanUpDataSourceList()));
  updateButtons();
}


VectorDialog::~VectorDialog() {
}


void VectorDialog::editMultipleMode() {
  _vectorTab->enableSingleEditOptions(false);
  _vectorTab->clearTabValues();
}


void VectorDialog::editSingleMode() {
  _vectorTab->enableSingleEditOptions(true);
   configureTab(dataObject());
}


void VectorDialog::updateButtons() {

  bool valid = _vectorTab->vectorMode() == VectorTab::GeneratedVector || !_vectorTab->field().isEmpty();
  // FIXME: add stricter validity testing.
  _buttonBox->button(QDialogButtonBox::Ok)->setEnabled(valid);
  _buttonBox->button(QDialogButtonBox::Apply)->setEnabled(valid);
}


void VectorDialog::configureTab(ObjectPtr vector) {
  if (!vector) {
    _vectorTab->dataRange()->loadWidgetDefaults();
    _vectorTab->setFile(dialogDefaults().value("vector/datasource",_vectorTab->file()).toString());
    _vectorTab->setFrom(dialogDefaults().value("genVector/min",-10).toInt());
    _vectorTab->setTo(dialogDefaults().value("genVector/max",10).toInt());
    _vectorTab->setNumberOfSamples(dialogDefaults().value("genVector/length",1000).toInt());
    _vectorTab->setVectorMode((VectorTab::VectorMode)dialogDefaults().value("genVector/vectorType",VectorTab::GeneratedVector).toInt()); // FIXME: should be sticky
  } else if (DataVectorPtr dataVector = kst_cast<DataVector>(vector)) {
    _vectorTab->setVectorMode(VectorTab::DataVector);
    _vectorTab->setFile(dataVector->dataSource()->fileName());
    _vectorTab->setDataSource(dataVector->dataSource());
    _vectorTab->updateIndexList(dataVector->dataSource());
    _vectorTab->setField(dataVector->field());

    _vectorTab->dataRange()->setRangeUnits(dataVector->rangeUnits());
    if ( _vectorTab->dataRange()->rangeUnitsIndex()>0) {
      double frame_per_index = dataVector->dataSource()->framePerIndex(dataVector->startUnits());
      if (frame_per_index == 0) {
        frame_per_index = 1.0;
      }
      _vectorTab->dataRange()->setRange(dataVector->numFrames()/frame_per_index);

    } else {
      _vectorTab->dataRange()->setRange(dataVector->numFrames());
    }

    _vectorTab->dataRange()->setStartUnits(dataVector->startUnits());
    if (_vectorTab->dataRange()->startUnitsIndex()>0) {
      _vectorTab->dataRange()->setStart(dataVector->dataSource()->frameToIndex(dataVector->startFrame(),dataVector->startUnits()));
    } else {
      _vectorTab->dataRange()->setStart(dataVector->startFrame());
    }

    _vectorTab->dataRange()->setCountFromEnd(dataVector->countFromEOF());
    _vectorTab->dataRange()->setReadToEnd(dataVector->readToEOF());
    _vectorTab->dataRange()->setSkip(dataVector->skip());
    _vectorTab->dataRange()->setDoSkip(dataVector->doSkip());
    _vectorTab->dataRange()->setDoFilter(dataVector->doAve());
    _vectorTab->hideGeneratedOptions();
    if (_editMultipleWidget) {
      DataVectorList objects = _document->objectStore()->getObjects<DataVector>();
      _editMultipleWidget->clearObjects();
      foreach(DataVectorPtr object, objects) {
        _editMultipleWidget->addObject(object->Name(), object->descriptionTip());
      }
    }
  } else if (GeneratedVectorPtr generatedVector = kst_cast<GeneratedVector>(vector)) {
    _vectorTab->setVectorMode(VectorTab::GeneratedVector);
    _vectorTab->setFrom(generatedVector->min());
    _vectorTab->setTo(generatedVector->max());
    _vectorTab->setNumberOfSamples(generatedVector->length());
    _vectorTab->hideDataOptions();
    if (_editMultipleWidget) {
      DataVectorList objects = _document->objectStore()->getObjects<DataVector>();
      _editMultipleWidget->clearObjects();
      foreach(DataVectorPtr object, objects) {
        _editMultipleWidget->addObject(object->Name(), object->descriptionTip());
      }
    }
  }
}


ObjectPtr VectorDialog::createNewDataObject() {
  switch(_vectorTab->vectorMode()) {
  case VectorTab::DataVector:
    dialogDefaults().setValue("genVector/vectorType", VectorTab::DataVector);
    return createNewDataVector();
  case VectorTab::GeneratedVector:
    dialogDefaults().setValue("genVector/vectorType", VectorTab::GeneratedVector);
    return createNewGeneratedVector();
  default:
    return 0;
  }
}


void VectorTab::updateIndexList(DataSourcePtr dataSource) {
  dataRange()->updateIndexList(dataSource->indexFields());
}


void VectorTab::clearIndexList() {
  dataRange()->clearIndexList();
}


ObjectPtr VectorDialog::createNewDataVector() {
  DataSourcePtr dataSource = _vectorTab->dataSource();

  //FIXME better validation than this please...
  if (!dataSource)
    return 0;

  // Hook the progress signal when loading a new datasource to get feedback in case it takes long
  connect(dataSource, SIGNAL(progress(int,QString)), kstApp->mainWindow(), SLOT(updateProgress(int,QString)));
  dataSource->vector().prepareRead(1);

  const QString field = _vectorTab->field();
  const DataRange *dataRange = _vectorTab->dataRange();

  Q_ASSERT(_document && _document->objectStore());

  DataVectorPtr vector = _document->objectStore()->createObject<DataVector>();

  double startOffset = dataRange->start();
  double rangeCount = dataRange->range();

  if ((dataRange->_startUnits->currentIndex() != 0) && (!dataRange->countFromEnd())) {
    startOffset = _vectorTab->dataSource()->indexToFrame(dataRange->start(), dataRange->startUnits());
  }

  if ((dataRange->_rangeUnits->currentIndex() != 0) && (!dataRange->readToEnd())) {
    rangeCount = dataRange->range()*_vectorTab->dataSource()->framePerIndex(dataRange->rangeUnits());
  }

  vector->writeLock();
  vector->change(dataSource, field,
      dataRange->countFromEnd() ? -1 : int(startOffset),
      dataRange->readToEnd() ? -1 : int(rangeCount),
      dataRange->skip(),
      dataRange->doSkip(),
      dataRange->doFilter());

  vector->setRangeUnits(dataRange->rangeUnits());
  vector->setStartUnits(dataRange->startUnits());

  if (DataDialog::tagStringAuto()) {
     vector->setDescriptiveName(QString());
  } else {
     vector->setDescriptiveName(DataDialog::tagString());
  }

  setDataVectorDefaults(vector);
  _vectorTab->dataRange()->setWidgetDefaults();

  vector->registerChange();
  vector->unlock();

  _dataObjectName = vector->Name();

  //UpdateServer::self()->requestUpdateSignal();
  return vector;
}


ObjectPtr VectorDialog::createNewGeneratedVector() {
  const qreal from = _vectorTab->from();
  const qreal to = _vectorTab->to();
  const int numberOfSamples = _vectorTab->numberOfSamples();

  Q_ASSERT(_document && _document->objectStore());
  GeneratedVectorPtr vector = _document->objectStore()->createObject<GeneratedVector>();
  vector->changeRange(from, to, numberOfSamples);

  setGenVectorDefaults(vector);

  if (DataDialog::tagStringAuto()) {
     vector->setDescriptiveName(QString());
  } else {
     vector->setDescriptiveName(DataDialog::tagString());
  }

  _dataObjectName = vector->Name();

  _vectorTab->setDataSource(0L);
  _document->objectStore()->cleanUpDataSourceList();

  //UpdateServer::self()->requestUpdateSignal();
  return vector;
}


ObjectPtr VectorDialog::editExistingDataObject() const {
  if (DataVectorPtr dataVector = kst_cast<DataVector>(dataObject())) {
    if (editMode() == EditMultiple) {
      const DataRange *dataRange = _vectorTab->dataRange();
      QStringList objects = _editMultipleWidget->selectedObjects();
      foreach (const QString &objectName, objects) {
        DataVectorPtr vector = kst_cast<DataVector>(_document->objectStore()->retrieveObject(objectName));
        if (vector) {
          int start = dataRange->startDirty() ? dataRange->start() : vector->startFrame();
          int range = dataRange->rangeDirty() ?  dataRange->range() : vector->numFrames();
          int skip = dataRange->skipDirty() ?  dataRange->skip() : vector->skip();

          if (dataRange->countFromEndDirty()) {
              start = dataRange->countFromEnd() ? -1 : dataRange->start();
              range = dataRange->readToEnd() ? -1 : dataRange->range();
          }

          bool doSkip = dataRange->doSkipDirty() ?  dataRange->doSkip() : vector->doSkip();
          bool doAve = dataRange->doFilterDirty() ?  dataRange->doFilter() : vector->doAve();
          vector->writeLock();
          vector->changeFrames(start, range, skip, doSkip, doAve);
          vector->registerChange();
          vector->unlock();
        }
      }
    } else {
      const DataSourcePtr dataSource = _vectorTab->dataSource();

      //FIXME better validation than this please...
      if (!dataSource)
        return 0;

      const QString field = _vectorTab->field();
      const DataRange *dataRange = _vectorTab->dataRange();

      double startOffset = dataRange->start();
      double rangeCount = dataRange->range();

      if ((dataRange->_startUnits->currentIndex() != 0) && (!dataRange->countFromEnd())) {
        startOffset = _vectorTab->dataSource()->indexToFrame(dataRange->start(), dataRange->startUnits());
      }

      if ((dataRange->_rangeUnits->currentIndex() != 0) && (!dataRange->readToEnd())) {
        rangeCount = dataRange->range()*_vectorTab->dataSource()->framePerIndex(dataRange->rangeUnits()) + 0.0001;
      }


      dataVector->writeLock();
      dataVector->change(dataSource, field,
        dataRange->countFromEnd() ? -1 : int(startOffset),
        dataRange->readToEnd() ? -1 : int(rangeCount),
        dataRange->skip(),
        dataRange->doSkip(),
        dataRange->doFilter());

      if (DataDialog::tagStringAuto()) {
        dataVector->setDescriptiveName(QString());
      } else {
        dataVector->setDescriptiveName(DataDialog::tagString());
      }

      dataVector->registerChange();
      dataVector->unlock();

      setDataVectorDefaults(dataVector);
      _vectorTab->dataRange()->setWidgetDefaults();
    }
  } else if (GeneratedVectorPtr generatedVector = kst_cast<GeneratedVector>(dataObject())) {
    if (editMode() == EditMultiple) {
      QStringList objects = _editMultipleWidget->selectedObjects();
      foreach (const QString &objectName, objects) {
        GeneratedVectorPtr vector = kst_cast<GeneratedVector>(_document->objectStore()->retrieveObject(objectName));
        if (vector) {
          double min = _vectorTab->fromDirty() ? _vectorTab->from() : vector->min();
          double max = _vectorTab->toDirty() ?  _vectorTab->to() : vector->max();
          double length = _vectorTab->numberOfSamplesDirty() ?  _vectorTab->numberOfSamples() : vector->length();
          vector->writeLock();
          vector->changeRange(min, max, length);
          vector->registerChange();
          vector->unlock();
        }
      }
    } else {
      const qreal from = _vectorTab->from();
      const qreal to = _vectorTab->to();
      const int numberOfSamples = _vectorTab->numberOfSamples();
      generatedVector->writeLock();
      generatedVector->changeRange(from, to, numberOfSamples);
      if (DataDialog::tagStringAuto()) {
        generatedVector->setDescriptiveName(QString());
      } else {
        generatedVector->setDescriptiveName(DataDialog::tagString());
      }
      generatedVector->registerChange();
      generatedVector->unlock();
      setGenVectorDefaults(generatedVector);
    }
  }

  return dataObject();
}

void VectorDialog::waitForValidation() {
  while (_vectorTab->validating && (_vectorTab->vectorMode() == VectorTab::DataVector)) {
    Sleep::ms(10);
    QApplication::processEvents();
  }
}

}

// vim: ts=2 sw=2 et
