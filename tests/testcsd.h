/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TESTCSD_H
#define TESTCSD_H

#include <QObject>
#include <QDomDocument>

class TestCSD : public QObject
{
  Q_OBJECT

  private:
    QDomDocument makeDOMElement(const QString& tag, const QString& val);
  private Q_SLOTS:
    void cleanupTestCase();

    void testCSD();
};

#endif

// vim: ts=2 sw=2 et
