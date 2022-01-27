/***************************************************************************
                          generatedvector.cpp - a vector from x0 to x1 with n pts
                             -------------------
    begin                : March, 2005
    copyright            : (C) 2005 by cbn
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
#include "generatedvector.h"
#include "vectorscriptinterface.h"

#include <QDebug>
#include <QXmlStreamWriter>


namespace Kst {

const QString GeneratedVector::staticTypeString = "Generated Vector";
const QString GeneratedVector::staticTypeTag = "generatedvector";

GeneratedVector::GeneratedVector(ObjectStore *store)
    : Vector(store) {
  _saveable = true;
  _saveData = false;
}


const QString& GeneratedVector::typeString() const {
  return staticTypeString;
}


ScriptInterface* GeneratedVector::createScriptInterface() {
  return new VectorGenSI(this);
}


void GeneratedVector::save(QXmlStreamWriter &s) {
  s.writeStartElement("generatedvector");
  s.writeAttribute("min", QString::number(min()));
  s.writeAttribute("max", QString::number(max()));
  s.writeAttribute("count", QString::number(length()));
  saveNameInfo(s, VNUM|XNUM);

  s.writeEndElement();
}


void GeneratedVector::changeRange(double x0, double x1, int n) {
  if (n < 2) {
    n = 2;
  }
  if (n != length()) {
    resize(n, false);
  }
  if (x0 > x1) {
    double tx;
    tx = x0;
    x0 = x1;
    x1 = tx;
  } else if (x0 == x1) {
    x1 = x0 + 0.1;
  }

  for (int i = 0; i < n; i++) {
    _v[i] = x0 + double(i) * (x1 - x0) / double(n - 1);
  }

  _min = x0;
  _max = x1;

  _scalars["min"]->setValue(x0);
  _scalars["max"]->setValue(x1);

  registerChange();
}

void GeneratedVector::setSaveData(bool save) {
  Q_UNUSED(save)
}

QString GeneratedVector::_automaticDescriptiveName() const {
  return QString::number(_v[0])+".."+QString::number(_v[length()-1]);
}

QString GeneratedVector::descriptionTip() const {
  return tr("Generated Vector: %1\n"
      "  %2 values from %3 to %4").arg(Name()).arg(length()).arg(_v[0]).arg(_v[length()-1]);
}

QString GeneratedVector::propertyString() const {
  return tr("%3 points from %1 to %2").arg(_v[0]).arg(_v[length()-1]).arg(length());
}

}

// vim: ts=2 sw=2 et
