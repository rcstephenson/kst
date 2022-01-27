/***************************************************************************
                              dialoglauncher.cpp
                             -------------------
    begin                : Nov. 24, 2004
    copyright            : (C) 2004 The University of Toronto
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

#include "dialoglauncher.h"

#include <QApplication>

namespace Kst {

DialogLauncher *DialogLauncher::_self = 0L;
void DialogLauncher::cleanup() {
    delete _self;
    _self = 0;
}


DialogLauncher *DialogLauncher::self() {
  Q_ASSERT(_self);
  return _self;
}


void DialogLauncher::replaceSelf(DialogLauncher *newInstance) {
  cleanup();
  _self = newInstance;
}


DialogLauncher::DialogLauncher() {
  qAddPostRoutine(DialogLauncher::cleanup);
}


DialogLauncher::~DialogLauncher() {
}


void DialogLauncher::showVectorDialog(QString &vectorname, ObjectPtr objectPtr, bool modal) {
  Q_UNUSED(objectPtr);
  Q_UNUSED(vectorname);
  Q_UNUSED(modal);
}


void DialogLauncher::showMatrixDialog(QString &matrixName, ObjectPtr objectPtr, bool modal) {
  Q_UNUSED(objectPtr);
  Q_UNUSED(matrixName);
  Q_UNUSED(modal);
}


void DialogLauncher::showScalarDialog(QString &scalarname, ObjectPtr objectPtr, bool modal) {
  Q_UNUSED(objectPtr);
  Q_UNUSED(scalarname);
  Q_UNUSED(modal);
}


void DialogLauncher::showStringDialog(QString &scalarname, ObjectPtr objectPtr, bool modal) {
  Q_UNUSED(objectPtr);
  Q_UNUSED(scalarname);
  Q_UNUSED(modal);
}


void DialogLauncher::showCurveDialog(ObjectPtr objectPtr, VectorPtr vector) {
  Q_UNUSED(objectPtr);
  Q_UNUSED(vector);
}

void DialogLauncher::showMultiCurveDialog(QList<ObjectPtr> curves) {
  Q_UNUSED(curves);
}

void DialogLauncher::showImageDialog(ObjectPtr objectPtr, MatrixPtr matrix) {
  Q_UNUSED(objectPtr);
  Q_UNUSED(matrix);
}

void DialogLauncher::showMultiImageDialog(QList<ObjectPtr> images) {
  Q_UNUSED(images);
}


void DialogLauncher::showEquationDialog(ObjectPtr objectPtr) {
  Q_UNUSED(objectPtr);
}


void DialogLauncher::showEventMonitorDialog(ObjectPtr objectPtr) {
  Q_UNUSED(objectPtr);
}


void DialogLauncher::showHistogramDialog(ObjectPtr objectPtr, VectorPtr vector) {
  Q_UNUSED(objectPtr);
  Q_UNUSED(vector);
}


void DialogLauncher::showPowerSpectrumDialog(ObjectPtr objectPtr, VectorPtr vector) {
  Q_UNUSED(objectPtr);
  Q_UNUSED(vector);
}


void DialogLauncher::showCSDDialog(ObjectPtr objectPtr, VectorPtr vector) {
  Q_UNUSED(objectPtr);
  Q_UNUSED(vector);
}


void DialogLauncher::showBasicPluginDialog(QString pluginName, ObjectPtr objectPtr, VectorPtr vectorX, VectorPtr vectorY, PlotItemInterface *plotItem) {
  Q_UNUSED(pluginName);
  Q_UNUSED(objectPtr);
  Q_UNUSED(vectorX);
  Q_UNUSED(vectorY);
  Q_UNUSED(plotItem);
}


void DialogLauncher::showObjectDialog(ObjectPtr objectPtr) {
  Q_UNUSED(objectPtr);
}

void DialogLauncher::showMultiObjectDialog(QList<ObjectPtr> objects) {
  Q_UNUSED(objects);
}
}

// vim: ts=2 sw=2 et
