/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
 *   copyright : (C) 2005 by University of British Columbia                *
 *                   dscott@phas.ubc.ca                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/** A class for handling spectrograms for kst
 */

#include "csd.h"

#include <assert.h>
#include <math.h>

#include <QXmlStreamWriter>
#include <QLatin1String>



#include "dialoglauncher.h"
#include "datacollection.h"
#include "debug.h"
#include "psdcalculator.h"
#include "objectstore.h"

extern "C" void rdft(int n, int isgn, double *a);

namespace Kst {

const QString CSD::staticTypeString = "Spectrogram";
const QString CSD::staticTypeTag = "csd";

static const QLatin1String CSD_INVECTOR = QLatin1String("I");
static const QLatin1String& OUTMATRIX = QLatin1String("M");

#define KSTCSDMAXLEN 27
CSD::CSD(ObjectStore *store)
  : DataObject(store) {
  _typeString = staticTypeString;
  _type = "Spectrogram";

  _initializeShortName();

  Q_ASSERT(store);
  MatrixPtr outMatrix = store->createObject<Matrix>();
  outMatrix->setProvider(this);
  outMatrix->setSlaveName("SG");
  outMatrix->change(2, 2);
  _outMatrix = _outputMatrices.insert(OUTMATRIX, outMatrix).value();
}

void CSD::_initializeShortName() {
  _shortName = 'G'+QString::number(_csdnum);
  if (_csdnum>max_csdnum)
    max_csdnum = _csdnum;
  _csdnum++;
}

void CSD::change(VectorPtr in_V, double in_freq, bool in_average,
    bool in_removeMean, bool in_apodize, ApodizeFunction in_apodizeFxn,
    int in_windowSize, int in_length, double in_gaussianSigma,
    PSDType in_outputType, const QString& in_vectorUnits,
    const QString& in_rateUnits) {

  _inputVectors[CSD_INVECTOR] = in_V;
  QString vecName = in_V ? in_V->Name() : QString();
  _frequency = in_freq;
  _average = in_average;
  _apodize = in_apodize;
  _windowSize = in_windowSize;
  _apodizeFxn = in_apodizeFxn;
  _gaussianSigma = in_gaussianSigma;
  _removeMean = in_removeMean;
  _averageLength = in_length;
  _vectorUnits = in_vectorUnits;
  _rateUnits = in_rateUnits;
  _outputType = in_outputType;

  if (_frequency <= 0.0) {
    _frequency = 1.0;
  }

  updateMatrixLabels();
}


CSD::~CSD() {
  _outMatrix = 0L;
}

void CSD::internalUpdate() {

  VectorPtr inVector = _inputVectors[CSD_INVECTOR];

  writeLockInputsAndOutputs();

  double *tempOutput, *input;
  int tempOutputLen = PSDCalculator::calculateOutputVectorLength(_windowSize, _average, _averageLength);
  _length = tempOutputLen;
  tempOutput = new double[tempOutputLen];

  input = inVector->value();

  int xSize = 0;
  for (int i=0; i < inVector->length(); i+= _windowSize) {
    //ensure there is enough data left.
    if (i + _windowSize >= inVector->length()) {
        break; //If there isn't enough left for a complete window.
    }

    _psdCalculator.calculatePowerSpectrum(input + i, _windowSize, tempOutput, tempOutputLen, _removeMean,  false, _average, _averageLength, _apodize, _apodizeFxn, _gaussianSigma, _outputType, _frequency);

    // resize output matrix
    _outMatrix->resize(xSize+1, tempOutputLen);

    if (_outMatrix->sampleCount() == (xSize+1)*tempOutputLen) { // all is well.
      // copy elements to output matrix
      for (int j=0; j < tempOutputLen; j++) {
        _outMatrix->setValueRaw(xSize, j, tempOutput[j]);
      }
    } else {
      Debug::self()->log(tr("Could not allocate sufficient memory for Spectrogram."), Debug::Error);
      break;
    }

    xSize++;
  }

  delete[] tempOutput;

  double frequencyStep = .5*_frequency/(double)(tempOutputLen-1);

  _outMatrix->change(xSize, tempOutputLen, 0, 0, _windowSize/_frequency, frequencyStep);

  unlockInputsAndOutputs();

  return;
}


void CSD::save(QXmlStreamWriter &s) {
  s.writeStartElement(staticTypeTag);
  s.writeAttribute("vector", _inputVectors[CSD_INVECTOR]->Name());
  s.writeAttribute("samplerate", QString::number(_frequency));
  s.writeAttribute("gaussiansigma", QString::number(_gaussianSigma));
  s.writeAttribute("average", QVariant(_average).toString());
  s.writeAttribute("fftlength", QString::number(int(ceil(log(double(_length*2)) / log(2.0)))));
  s.writeAttribute("removemean", QVariant(_removeMean).toString());
  s.writeAttribute("apodize", QVariant(_apodize).toString());
  s.writeAttribute("apodizefunction", QString::number(_apodizeFxn));
  s.writeAttribute("windowsize", QString::number(_windowSize));
  s.writeAttribute("vectorunits", _vectorUnits);
  s.writeAttribute("rateunits", _rateUnits);
  s.writeAttribute("outputtype", QString::number(_outputType));
  saveNameInfo(s,VNUM|XNUM|MNUM|CSDNUM);

  s.writeEndElement();
}


void CSD::setVector(VectorPtr new_v) {
  VectorPtr v = _inputVectors[CSD_INVECTOR];
  if (v) {
    if (v == new_v) {
      return;
    }
    v->unlock();
  }

  _inputVectors.remove(CSD_INVECTOR);
  new_v->writeLock();
  _inputVectors[CSD_INVECTOR] = new_v;
}


VectorPtr CSD::vector() const {
  return _inputVectors[CSD_INVECTOR];
}


bool CSD::slaveVectorsUsed() const {
  return true;
}


QString CSD::propertyString() const {
  return tr("Spectrogram: %1").arg(_inputVectors[CSD_INVECTOR]->Name());
}


void CSD::showNewDialog() {
  DialogLauncher::self()->showCSDDialog();
}


void CSD::showEditDialog() {
  DialogLauncher::self()->showCSDDialog(this);
}


bool CSD::apodize() const {
  return _apodize;
}


PSDType CSD::output() const {
  return _outputType;
}


void CSD::setOutput(PSDType in_outputType)  {
  _outputType = in_outputType;

  updateMatrixLabels();
}


void CSD::setApodize(bool in_apodize)  {
  _apodize = in_apodize;
}


bool CSD::removeMean() const {
  return _removeMean;
}


void CSD::setRemoveMean(bool in_removeMean) {
  _removeMean = in_removeMean;
}


bool CSD::average() const {
  return _average;
}


void CSD::setAverage(bool in_average) {
  _average = in_average;
}


double CSD::frequency() const {
  return _frequency;
}


void CSD::setFrequency(double in_frequency) {
  if (in_frequency > 0.0) {
    _frequency = in_frequency;
  } else {
    _frequency = 1.0;
  }
}

ApodizeFunction CSD::apodizeFxn() const {
  return _apodizeFxn;
}

void CSD::setApodizeFxn(ApodizeFunction in_fxn) {
  _apodizeFxn = in_fxn;
}

int CSD::length() const {
  return _averageLength;
}

void CSD::setLength(int in_length) {
  _averageLength = in_length;
}


int CSD::windowSize() const {
  return _windowSize;
}


void CSD::setWindowSize(int in_size) {
  _windowSize = in_size;
}

double CSD::gaussianSigma() const {
  return _gaussianSigma;
}

void CSD::setGaussianSigma(double in_sigma) {
  _gaussianSigma = in_sigma;
}


MatrixPtr CSD::outputMatrix() const {
  return _outMatrix;
}


const QString& CSD::vectorUnits() const {
  return _vectorUnits;
}


void CSD::setVectorUnits(const QString& units) {
  _vectorUnits = units;
}


const QString& CSD::rateUnits() const {
  return _rateUnits;
}


void CSD::setRateUnits(const QString& units) {
  _rateUnits = units;
}


DataObjectPtr CSD::makeDuplicate() const{

  CSDPtr csd = store()->createObject<CSD>();
  csd->change(_inputVectors[CSD_INVECTOR],
              _frequency,
              _average,
              _removeMean,
              _apodize,
              _apodizeFxn,
              _windowSize,
              _averageLength,
              _gaussianSigma,
              _outputType,
              _vectorUnits,
              _rateUnits);
  if (descriptiveNameIsManual()) {
    csd->setDescriptiveName(descriptiveName());
  }
  csd->writeLock();
  csd->registerChange();
  csd->unlock();

  return DataObjectPtr(csd);
}

void CSD::updateMatrixLabels(void) {

  LabelInfo label_info;

  switch (_outputType) {
  default:
  case 0: // amplitude spectral density (default) [V/Hz^1/2]
    label_info.quantity = tr("Amplitude Spectral Density");
    label_info.units = QString("%1/%2^{1/2}").arg(_vectorUnits).arg(_rateUnits);
    break;
  case 1: // power spectral density [V^2/Hz]
    label_info.quantity = tr("Power Spectral Density");
    label_info.units = QString("%1^2/%2").arg(_vectorUnits).arg(_rateUnits);
    break;
  case 2: // amplitude spectrum [V]
    label_info.quantity = tr("Amplitude Spectrum");
    label_info.units = QString("%1").arg(_vectorUnits);
    break;
  case 3: // power spectrum [V^2]
    label_info.quantity = tr("Power Spectrum");
    label_info.units = QString("%1^2").arg(_vectorUnits);
    break;
  }
  label_info.name = _inputVectors[CSD_INVECTOR]->descriptiveName();
  _outMatrix->setTitleInfo(label_info);

  label_info.name.clear();
  label_info.units = _rateUnits;
  label_info.quantity = tr("Frequency");
  _outMatrix->setYLabelInfo(label_info);

  label_info.quantity = tr("Time");
  label_info.units = QString('s');
  _outMatrix->setXLabelInfo(label_info);

}

QString CSD::_automaticDescriptiveName() const {
  return vector()->descriptiveName();
}

QString CSD::descriptionTip() const {
  QString tip;

  tip = tr("Spectrogram: %1\n  FFT Length: 2^%2").arg(Name()).arg(length());

  if (average() || apodize() || removeMean()) {
    tip += "\n  ";
    if (average()) tip += tr("Average; ");
    if (apodize()) tip += tr("Apodize; ");
    if (removeMean()) tip += tr("Remove Mean;");
  }
  tip += tr("\nInput: %1").arg(_inputVectors[CSD_INVECTOR]->descriptionTip());
  return tip;
 
}
}

// vim: ts=2 sw=2 et
