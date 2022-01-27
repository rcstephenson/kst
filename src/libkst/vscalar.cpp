/***************************************************************************
                          vscalar.cpp  -  a scalar from a data source
                             -------------------
    begin                : September, 2008
    copyright            : (C) 2008 by cbn
    email                : netterfield@astro.utoronto.ca
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "vscalar.h"

#include <QDebug>
#include <QTextDocument>
#include <QXmlStreamWriter>


#include "debug.h"
#include "objectstore.h"
#include "updatemanager.h"
#include "vscalar.h"
#include "scalarscriptinterface.h"

namespace Kst {

const QString VScalar::staticTypeString = "Vector Field Scalar";
const QString VScalar::staticTypeTag = "vscalar";

/** Create a VScalar: a scalar from a single sample of a vector field */
VScalar::VScalar(ObjectStore *store)
: Scalar(store) {

  _file = 0L;
  _field.clear();

  setOrphan(true);
}


VScalar::~VScalar() {
  _file = 0;
}


QString VScalar::_automaticDescriptiveName() const {
  return field();
}


const QString& VScalar::typeString() const {
  return staticTypeString;
}

ScriptInterface* VScalar::createScriptInterface() {
  return new ScalarVectorSI(this);
}

void VScalar::change(DataSourcePtr in_file, const QString &in_field, int in_f0) {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  _field = in_field;
  _file = in_file;
  _f0 = in_f0;
}

void VScalar::changeFile(DataSourcePtr in_file) {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  if (!in_file) {
    Debug::self()->log(tr("Data file for scalar %1 was not opened.").arg(Name()), Debug::Warning);
  }
}


/** return the name of the file */
QString VScalar::filename() const {
  QString rc;
  if (_file) {
    _file->readLock();
    rc = _file->fileName();
    _file->unlock();
  }
  return rc;
}


/** return the field */
const QString& VScalar::field() const {
  return _field;
}

/** return the sample number */
int VScalar::F0() const {
  return _f0;
}

/** Save data scalar information */
void VScalar::save(QXmlStreamWriter &s) {
  if (_file) {
    s.writeStartElement("vscalar");

    _file->readLock();
    s.writeAttribute("provider", _file->Name());
    DataPrimitive::saveFilename(_file->fileName(), s);
    _file->unlock();

    s.writeAttribute("field", _field);
    s.writeAttribute("f0", QString::number(_f0));

    saveNameInfo(s, XNUM);
    s.writeEndElement();
  }
}


/** Update a data Scalar */
void VScalar::internalUpdate() {
  if (_file) {
    int f0;
    if (_f0<0) { 
      f0 = _file->vector().dataInfo(_field).frameCount-1;
    } else {
      f0 = _f0;
    }
    _file->writeLock();
    DataVector::ReadInfo p = {&_value, f0, -1, -1};
    _file->vector().read(_field, p);
    _file->unlock();
  }
}

qint64 VScalar::minInputSerial() const {
  if (_file) {
    return (_file->serial());
  }
  return LLONG_MAX;
}

qint64 VScalar::maxInputSerialOfLastChange() const {
  if (_file) {
    return (_file->serialOfLastChange());
  }
  return NoInputs;
}

PrimitivePtr VScalar::_makeDuplicate() const {
  Q_ASSERT(store());
  VScalarPtr scalar = store()->createObject<VScalar>();

  scalar->writeLock();
  scalar->change(_file, _field, _f0);
  if (descriptiveNameIsManual()) {
    scalar->setDescriptiveName(descriptiveName());
  }

  scalar->registerChange();
  scalar->unlock();

  return kst_cast<Primitive>(scalar);
}


DataSourcePtr VScalar::dataSource() const {
  return _file;
}


QString VScalar::descriptionTip() const {
  QString IDstring;

  IDstring = tr(
      "Data Scalar: %1 = %4\n"
      "  %2\n"
      "  Field: %3\n"
      "  Frame: %5"
                 ).arg(Name()).arg(dataSource()->fileName()).arg(field()).arg(value()).arg(F0());
  return IDstring;
}


/** return true if it has a valid file and field, or false otherwise */
bool VScalar::isValid() const {
  if (_file) {
    _file->readLock();
    bool rc = _file->vector().isValid(_field);
    _file->unlock();
    return rc;
  }
  return false;
}


bool VScalar::_checkValidity(const DataSourcePtr ds) const {
  if (ds) {
    ds->readLock();
    bool rc = ds->vector().isValid(_field);
    ds->unlock();
    return rc;
  }
  return false;
}


QString VScalar::propertyString() const {
  return tr("%2 frame %3 of %1 = %4", "%2 is field name.  %3 is frame/index.  %1 is the file name.  %4 is the value").arg(dataSource()->fileName()).arg(field()).arg(F0()).arg(value());
}

}
// vim: ts=2 sw=2 et
