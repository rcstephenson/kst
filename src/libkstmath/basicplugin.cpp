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

#include "basicplugin.h"

#include <stdlib.h>

#include <QXmlStreamWriter>

#ifndef Q_OS_WIN32
#include <unistd.h>
#endif



#include "debug.h"
#include "dialoglauncher.h"
#include "datacollection.h"
#include "objectstore.h"
#include "dataobjectplugin.h"
#include "dataobjectscriptinterface.h"

namespace Kst {

const QString BasicPlugin::staticTypeString = "Plugin";
const QString BasicPlugin::staticTypeTag = "plugin";

BasicPlugin::BasicPlugin(ObjectStore *store)
: DataObject(store) {
  _typeString = "Plugin";
  _type = "Plugin";

  _initializeShortName();
}


BasicPlugin::~BasicPlugin() {
}

void BasicPlugin::_initializeShortName() {
  _shortName = 'P'+QString::number(_pnum);
  if (_pnum>max_pnum)
    max_pnum = _pnum;
  _pnum++;

}


ScriptInterface* BasicPlugin::createScriptInterface() {
  return new PluginSI(this);
}


void BasicPlugin::setPluginName(const QString &pluginName) {
  _pluginName = pluginName;
}


DataObjectPtr BasicPlugin::makeDuplicate() const {
#if 0
  BasicPluginPtr plugin = kst_cast<BasicPlugin>(DataObject::createPlugin(propertyString()));

  // use same inputs
  for (VectorMap::ConstIterator iter = _inputVectors.begin(); iter != _inputVectors.end(); ++iter) {
    plugin->inputVectors().insert(iter.key(), iter.value());
  }
  for (ScalarMap::ConstIterator iter = _inputScalars.begin(); iter != _inputScalars.end(); ++iter) {
    plugin->inputScalars().insert(iter.key(), iter.value());
  }
  for (StringMap::ConstIterator iter = _inputStrings.begin(); iter != _inputStrings.end(); ++iter) {
    plugin->inputStrings().insert(iter.key(), iter.value());
  }

  // create new outputs
  for (VectorMap::ConstIterator iter = outputVectors().begin(); iter != outputVectors().end(); ++iter) {
    KstWriteLocker blockVectorUpdates(&vectorList.lock());
    VectorPtr v = new Vector(ObjectTag(iter.value()->tag().tag() + "'", iter.value()->tag().context()), 0, plugin.data()); // FIXME: unique tag generation
    plugin->outputVectors().insert(iter.key(), v);
  }
  for (ScalarMap::ConstIterator iter = outputScalars().begin(); iter != outputScalars().end(); ++iter) {
    ScalarPtr s = new Scalar(ObjectTag(iter.value()->tag().tag() + "'", iter.value()->tag().context()), plugin.data()); // FIXME: unique tag generation
    plugin->outputScalars().insert(iter.key(), s);
  }
  for (StringMap::ConstIterator iter = outputStrings().begin(); iter != outputStrings().end(); ++iter) {
    StringPtr s = new String(ObjectTag(iter.value()->tag().tag() + "'", iter.value()->tag().context()), plugin.data()); // FIXME: unique tag generation
    plugin->outputStrings().insert(iter.key(), s);
  }

  // set the same plugin
  plugin->setTagName(ObjectTag(tag().tag() + "'", tag().context())); // FIXME: unique tag generation method
  map.insert(this, DataObjectPtr(plugin));
  return DataObjectPtr(plugin);
#endif
  // FIXME: implement this
  return 0L;
}

void BasicPlugin::showNewDialog() {
  DialogLauncher::self()->showBasicPluginDialog(_pluginName);
}


void BasicPlugin::showEditDialog() {
  DialogLauncher::self()->showBasicPluginDialog(_pluginName, this);
}


VectorPtr BasicPlugin::inputVector(const QString& vector) const {
  VectorMap::ConstIterator i = _inputVectors.constFind(vector);
  if (i != _inputVectors.constEnd())
    return *i;
  else
    return 0;
}


ScalarPtr BasicPlugin::inputScalar(const QString& scalar) const {
  ScalarMap::ConstIterator i = _inputScalars.constFind(scalar);
  if (i != _inputScalars.constEnd())
    return *i;
  else
    return 0;
}


StringPtr BasicPlugin::inputString(const QString& string) const {
  StringMap::ConstIterator i = _inputStrings.constFind(string);
  if (i != _inputStrings.constEnd())
    return *i;
  else
    return 0;
}


void BasicPlugin::setOutputVector(const QString &type, const QString &name) {
  QString txt = !name.isEmpty() ? name : type;
  Q_ASSERT(store());
  VectorPtr v = store()->createObject<Vector>();
  v->setProvider(this);
  v->setSlaveName(txt);
  _outputVectors.insert(type, v);
}


void BasicPlugin::setOutputScalar(const QString &type, const QString &name) {
  QString txt = !name.isEmpty() ? name : type;
  Q_ASSERT(store());
  ScalarPtr s = store()->createObject<Scalar>();
  s->setProvider(this);
  s->setSlaveName(txt);
  _outputScalars.insert(type, s);
}


void BasicPlugin::setOutputString(const QString &type, const QString &name) {
  QString txt = !name.isEmpty() ? name : type;
  Q_ASSERT(store());
  StringPtr s = store()->createObject<String>();
  s->setProvider(this);
  s->setSlaveName(txt);
  s->setOrphan(false);
  _outputStrings.insert(type, s);
}


void BasicPlugin::internalUpdate() {

  //Make sure we have all the necessary inputs
  if (!inputsExist())
    return;

  writeLockInputsAndOutputs();

  //Call the plugins algorithm to operate on the inputs
  //and produce the outputs
  if ( !algorithm() ) {
    Debug::self()->log(tr("There is an error in the %1 algorithm.").arg(propertyString()), Debug::Error);
    unlockInputsAndOutputs();
    return;
  }

  //Perform update on the outputs
  updateOutput();

  createScalars();

  unlockInputsAndOutputs();

  return;
}


// If a plugin provides a Parameters Vector, then scalars will be created, as well as a label.
void BasicPlugin::createScalars() {
  // Assumes that this is called with a write lock in place on this object
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  if (hasParameterVector()) {
    VectorPtr vectorParam = _outputVectors["Parameters Vector"];
    if (vectorParam) {
      QString paramName;
      int i = 0;
      int length = vectorParam->length();

      Q_ASSERT(store());
      for (paramName = parameterName(i);
          !paramName.isEmpty() && i < length;
           paramName = parameterName(++i)) {
        double scalarValue = vectorParam->value(i);
        if (!_outputScalars.contains(paramName)) {
          ScalarPtr s = store()->createObject<Scalar>();
          s->setProvider(this);
          s->setSlaveName(paramName);
          s->setValue(scalarValue);
          s->writeLock();
          _outputScalars.insert(paramName, s);
        } else {
          _outputScalars[paramName]->setValue(scalarValue);
        }
      }
    }
  }
}


QString BasicPlugin::parameterName(int /*index*/) const {
    return QString();
}


QString BasicPlugin::label(int precision) const {
  Q_UNUSED(precision)
  QString label;
  QString paramName;

  VectorPtr yVector = outputVectors().value(outputVectorList().first());

  label = yVector->labelInfo().name;

  if (hasParameterVector()) {
    VectorPtr vectorParam = _outputVectors["Parameters Vector"];
    int length = vectorParam->length();
    int i=0;
    for (paramName = parameterName(i);
         !paramName.isEmpty() && i < length;
         paramName = parameterName(++i)) {
        if (_outputScalars.contains(paramName)) {
          label += QString("\n%1: [%2]").arg(paramName).arg(_outputScalars[paramName]->Name());
        }
    }
  }

  return label;
}


template<class T, class V>
static void writeVectors(T& vectors, const QString& element, QXmlStreamWriter& stream, QString (V::* name)() const) {
  for (QStringList::iterator it = vectors.ordered.begin(); it != vectors.ordered.end(); ++it) {
    typename T::iterator i = vectors.find(*it);
    stream.writeStartElement(element);
    stream.writeAttribute("type", i.key());
    stream.writeAttribute("tag", (i.value()->*name)());
    stream.writeEndElement();
  }
}


void BasicPlugin::save(QXmlStreamWriter &stream) {
  stream.writeStartElement(staticTypeTag);
  stream.writeAttribute("type", _pluginName);
  saveNameInfo(stream, VNUM|PNUM|XNUM);

  writeVectors(_inputVectors, "inputvector", stream, &NamedObject::Name);
  writeVectors(_inputScalars, "inputscalar", stream, &NamedObject::Name);
  writeVectors(_inputStrings, "inputstring", stream, &NamedObject::Name);
  writeVectors(_outputVectors, "outputvector", stream, &Primitive::slaveName);
  writeVectors(_outputScalars, "outputscalar", stream, &Primitive::slaveName);
  writeVectors(_outputStrings, "outputstring", stream, &Primitive::slaveName);

  stream.writeEndElement();
}


void BasicPlugin::saveProperties(QXmlStreamWriter &s) {
  Q_UNUSED(s);
}


//TODO Could use some templates perhaps...
bool BasicPlugin::inputsExist() const {
  //First, check the inputVectors...
  QStringList iv = inputVectorList();
  QStringList::ConstIterator ivI = iv.constBegin();
  for (; ivI != iv.constEnd(); ++ivI) {
    if (!inputVector(*ivI))
      return false;
  }

  //Now, check the inputScalars...
  QStringList is = inputScalarList();
  QStringList::ConstIterator isI = is.constBegin();
  for (; isI != is.constEnd(); ++isI) {
    if (!inputScalar(*isI))
      return false;
  }

  //Finally, check the inputStrings...
  QStringList istr = inputStringList();
  QStringList::ConstIterator istrI = istr.constBegin();
  for (; istrI != istr.constEnd(); ++istrI) {
    if (!inputString(*istrI))
      return false;
  }
  return true;
}


void BasicPlugin::updateOutput() const {
  //output vectors...
  //FIXME: _outputVectors should be used, not this string list!
  QStringList ov = outputVectorList();
  QStringList::ConstIterator ovI = ov.constBegin();
  for (; ovI != ov.constEnd(); ++ovI) {
    if (VectorPtr o = outputVector(*ovI)) {
      Q_ASSERT(o->myLockStatus() == KstRWLock::WRITELOCKED);
      vectorRealloced(o, o->value(), o->length()); // why here?
      o->setNewAndShift(o->length(), o->numShift()); // why here?
    }
  }
}

QString BasicPlugin::descriptionTip() const {
  return tr("Plugin: %1").arg(Name());
}

}
// vim: ts=2 sw=2 et
