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

#ifndef HISTOGRAMFACTORY_H
#define HISTOGRAMFACTORY_H

#include "objectfactory.h"

namespace Kst {

class HistogramFactory : public ObjectFactory {
  public:
    HistogramFactory();
    ~HistogramFactory();
    DataObjectPtr generateObject(ObjectStore *store, QXmlStreamReader& stream);
};

}

#endif

// vim: ts=2 sw=2 et
