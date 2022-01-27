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

#ifndef SCALARFACTORY_H
#define SCALARFACTORY_H

#include "primitivefactory.h"

namespace Kst {

class ScalarFactory : public PrimitiveFactory {
  public:
    ScalarFactory();
    ~ScalarFactory();
    PrimitivePtr generatePrimitive(ObjectStore *store, QXmlStreamReader& stream);
};

class DataScalarFactory : public PrimitiveFactory {
  public:
    DataScalarFactory();
    ~DataScalarFactory();
    PrimitivePtr generatePrimitive(ObjectStore *store, QXmlStreamReader& stream);
};

class VScalarFactory : public PrimitiveFactory {
  public:
    VScalarFactory();
    ~VScalarFactory();
    PrimitivePtr generatePrimitive(ObjectStore *store, QXmlStreamReader& stream);
};
}

#endif

// vim: ts=2 sw=2 et
