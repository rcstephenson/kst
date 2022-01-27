/***************************************************************************
                          histogram.cpp: Histogram for KST
                             -------------------
    begin                : July 2002
    copyright            : (C) 2002 by C. Barth Netterfield
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


#include "histogram.h"

#include <assert.h>
#include <math.h>
#include <stdlib.h>

#include <QTextDocument>
#include <QXmlStreamWriter>


#include "dialoglauncher.h"
#include "datacollection.h"
#include "objectstore.h"

namespace Kst {

const QString Histogram::staticTypeString = "Histogram";
const QString Histogram::staticTypeTag = "histogram";

static const QLatin1String& RAWVECTOR  = QLatin1String("I");
static const QLatin1String& BINS = QLatin1String("B");
static const QLatin1String& HIST = QLatin1String("H");

Histogram::Histogram(ObjectStore *store)
  : DataObject(store) {
  setRealTimeAutoBin(false);
  _typeString = staticTypeString;
  _type = "Histogram";
  _initializeShortName();

  // _Bins, _bVector and _hVector need to be valid, 
  // so initialize them as size 2 (where 2 is a small valid number)
  _Bins = new unsigned long[2];
  _NumberOfBins = 0;

  VectorPtr v = store->createObject<Vector>();
  v->setProvider(this);
  v->setSlaveName("bin");
  v->resize(2);
  _bVector = _outputVectors.insert(BINS, v).value();

  v = store->createObject<Vector>();
  v->setProvider(this);
  v->setSlaveName("num");
  v->resize(2);
  _hVector = _outputVectors.insert(HIST, v).value();
}

void Histogram::_initializeShortName() {
  _shortName = 'H'+QString::number(_hnum);
  if (_hnum>max_hnum)
    max_hnum = _hnum;
  _hnum++;
}


void Histogram::change(VectorPtr in_V,
                          double xmin_in,
                          double xmax_in,
                          int in_n_bins,
                          NormalizationType in_norm_mode,
                          bool realTimeAutoBin) {

  _NormalizationMode = in_norm_mode;
  _realTimeAutoBin = realTimeAutoBin;
  _NumberOfBins = 0;

  _inputVectors[RAWVECTOR] = in_V;

  if (xmax_in>xmin_in) {
    _MaxX = xmax_in;
    _MinX = xmin_in;
  } else {
    _MinX = xmax_in;
    _MaxX = xmin_in;
  }
  if (_MaxX == _MinX) {
    _MaxX += 1.0;
    _MinX -= 1.0;
  }

  _NumberOfBins = in_n_bins;
  if (_NumberOfBins < 2) {
    _NumberOfBins = 2;
  }

  delete[] _Bins;
  _Bins = new unsigned long[_NumberOfBins];
  _NS = 3 * _NumberOfBins + 1;

  _bVector->resize(_NumberOfBins);
  _hVector->resize(_NumberOfBins);
}


Histogram::~Histogram() {
  _bVector = 0L;
  _hVector = 0L;

  delete[] _Bins;
  _Bins = 0L;
}


void Histogram::internalUpdate() {

  writeLockInputsAndOutputs();

  int i_bin, i_pt, ns;
  double y = 0.0;
  double MaxY = 0.0;
  // do auto-binning if necessary
  if (_realTimeAutoBin) {
    int temp_NumberOfBins;
    double temp_xMin, temp_xMax;
    Histogram::AutoBin(_inputVectors[RAWVECTOR], &temp_NumberOfBins, &temp_xMax, &temp_xMin);
    internalSetNumberOfBins(temp_NumberOfBins);
    internalSetXRange(temp_xMin, temp_xMax);
  }

  _NS = 3 * _NumberOfBins + 1;
  _W = (_MaxX - _MinX)/double(_NumberOfBins);

  memset(_Bins, 0, _NumberOfBins*sizeof(*_Bins));

  ns = _inputVectors[RAWVECTOR]->length();
  for (i_pt = 0; i_pt < ns ; ++i_pt) {
    y = _inputVectors[RAWVECTOR]->interpolate(i_pt, ns);
    i_bin = (int)floor((y-_MinX)/_W);
    if (i_bin >= 0 && i_bin < _NumberOfBins) {
      _Bins[i_bin]++;
    } else {
      // the top boundary of the top bin is included in the top bin.
      // for all other bins, the top boundary is included in the next bin
      if (y == _MaxX) {
        _Bins[_NumberOfBins-1]++;
      }
    }
  }

  for (i_bin=0; i_bin<_NumberOfBins; ++i_bin) {
    y = _Bins[i_bin];
    if (y > MaxY) {
      MaxY = y;
    }
  }

  LabelInfo label_info;

  switch (_NormalizationMode) {
    case Number:
      _Normalization = 1.0;
      label_info.quantity = tr("Number");
      break;
    case Percent:
      if (ns > 0) {
        _Normalization = 100.0/(double)ns;
      } else {
        _Normalization = 1.0;
      }
      label_info.quantity = tr("Percent");
      break;
    case Fraction:
      if (ns > 0) {
        _Normalization = 1.0/(double)ns;
      } else {
        _Normalization = 1.0;
      }
      label_info.quantity = tr("Fraction");
      break;
    case MaximumOne:
      if (MaxY > 0) {
        _Normalization = 1.0/MaxY;
      } else {
        _Normalization = 1.0;
      }
      label_info.quantity = tr("Normalized Frequency");
      break;
    default:
      _Normalization = 1.0;
      label_info.quantity = tr("Number");
      break;
  }

  _bVector->setLabelInfo(_inputVectors[RAWVECTOR]->labelInfo());

  label_info.quantity.clear();
  label_info.units.clear();
  label_info.name = tr( "Histogram of %1").arg(_bVector->labelInfo().name);
  label_info.file = _bVector->labelInfo().file;

  _hVector->setTitleInfo(label_info);

  double *bins = _bVector->value();
  double *hist = _hVector->value();

  for ( i_bin = 0; i_bin<_NumberOfBins; ++i_bin ) {
    bins[i_bin] = ( double( i_bin ) + 0.5 )*_W + _MinX;
    hist[i_bin] = _Bins[i_bin]*_Normalization;
  }

  unlockInputsAndOutputs();

}


int Histogram::numberOfBins() const {
  return _NumberOfBins;
}


void Histogram::setXRange(double xmin_in, double xmax_in) {
  internalSetXRange(xmin_in, xmax_in);
}


void Histogram::internalSetXRange(double xmin_in, double xmax_in) {
  if (xmax_in > xmin_in) {
    _MaxX = xmax_in;
    _MinX = xmin_in;
  } else if (xmax_in < xmin_in) {
    _MinX = xmax_in;
    _MaxX = xmin_in;
  } else {
    _MinX = xmax_in - 1.0;
    _MaxX = xmax_in + 1.0;
  }
  _W = (_MaxX - _MinX)/double(_NumberOfBins);
}


void Histogram::internalSetNumberOfBins(int in_n_bins) {
  if (in_n_bins < 2) {
    in_n_bins = 2;
  }
  if (_NumberOfBins != in_n_bins) {
    _NumberOfBins = in_n_bins;

    delete[] _Bins;
    _Bins = new unsigned long[_NumberOfBins];
    memset(_Bins, 0, _NumberOfBins*sizeof(*_Bins));

    _bVector->resize(_NumberOfBins);
    _hVector->resize(_NumberOfBins);
  }
  _W = (_MaxX - _MinX)/double(_NumberOfBins);
  _NS = 3 * _NumberOfBins + 1;
}


void Histogram::setNumberOfBins(int in_n_bins) {
  _realTimeAutoBin = false;
  internalSetNumberOfBins(in_n_bins);
}

void Histogram::setVector(VectorPtr new_v) {
  if (new_v) {
    _inputVectors[RAWVECTOR] = new_v;
  }
}


VectorPtr Histogram::vector() const {
  return _inputVectors[RAWVECTOR];
}


void Histogram::save(QXmlStreamWriter &xml) {
  xml.writeStartElement(staticTypeTag);
  xml.writeAttribute("vector", _inputVectors[RAWVECTOR]->Name());
  xml.writeAttribute("numberofbins", QString::number(_NumberOfBins));
  xml.writeAttribute("realtimeautobin", QVariant(_realTimeAutoBin).toString());
  xml.writeAttribute("min", QString::number(_MinX));
  xml.writeAttribute("max", QString::number(_MaxX));
  xml.writeAttribute("normalizationmode", QString::number(_NormalizationMode));
  saveNameInfo(xml, VNUM|HNUM|XNUM);

  xml.writeEndElement();
}


QString Histogram::propertyString() const {
  return tr("Histogram: %1").arg(_inputVectors[RAWVECTOR]->Name());
}


void Histogram::showNewDialog() {
  DialogLauncher::self()->showHistogramDialog();
}


void Histogram::showEditDialog() {
  DialogLauncher::self()->showHistogramDialog(this);
}


void Histogram::AutoBin(VectorPtr V, int *n, double *max, double *min) {
  double m;

  *max = V->max();
  *min = V->min();
  *n = V->length();

  if (*max < *min) {
    m = *max;
    *max = *min;
    *min = m;
  }

  if (*max == *min) {
    *max += 1.0;
    *min -= 1.0;
  }

  // we can do a better job auto-ranging using the tick rules from plot...
  // this has not been done yet, you will notice...
  *n /= 50;
  if (*n < 6) {
    *n = 6;
  }
  if (*n > 60) {
    *n = 60;
  }

  m = (*max - *min)/(100.0*double(*n));
  *max += m;
  *min -= m;
}


bool Histogram::slaveVectorsUsed() const {
  return true;
}


void Histogram::setRealTimeAutoBin(bool autoBin) {
  _realTimeAutoBin = autoBin;
}


bool Histogram::realTimeAutoBin() const {
  return _realTimeAutoBin;
}


double Histogram::vMax() const {
  return _inputVectors[RAWVECTOR]->max();
}


double Histogram::vMin() const {
  return _inputVectors[RAWVECTOR]->min();
}


int Histogram::vNumSamples() const {
  return _inputVectors[RAWVECTOR]->length();
}


DataObjectPtr Histogram::makeDuplicate() const {

  HistogramPtr histogram = store()->createObject<Histogram>();

  histogram->setVector(_inputVectors[RAWVECTOR]);
  histogram->setXRange(_MinX, _MaxX);
  histogram->setNumberOfBins(_NumberOfBins);
  histogram->setNormalizationType(_NormalizationMode);
  histogram->setRealTimeAutoBin(_realTimeAutoBin);

  if (descriptiveNameIsManual()) {
    histogram->setDescriptiveName(descriptiveName());
  }

  histogram->writeLock();
  histogram->registerChange();
  histogram->unlock();

  return DataObjectPtr(histogram);
}

QString Histogram::_automaticDescriptiveName() const {
  return (_inputVectors[RAWVECTOR]->descriptiveName());
}

QString Histogram::descriptionTip() const {
  QString tip;

  tip = tr("Histogram: %1").arg(Name());
  if (realTimeAutoBin()) {
    tip+= tr("\n  Auto-bin");
  } else {
    tip += tr("\n  %1 bins from %2 to %3").arg(numberOfBins()).arg(xMin()).arg(xMax());
  }
  tip += tr("\nInput: %1").arg(_inputVectors[RAWVECTOR]->descriptionTip());

  return tip;
}

}
// vim: ts=2 sw=2 et
