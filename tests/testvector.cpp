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

#include "testvector.h"

#include <vector.h>
#include <datacollection.h>
#include <objectstore.h>

#include "ksttest.h"

static Kst::ObjectStore _store;



void TestVector::cleanupTestCase() {
  _store.clear();
}

void TestVector::testVector() 
{
  Kst::VectorPtr v1 = Kst::kst_cast<Kst::Vector>(_store.createObject<Kst::Vector>());

  QCOMPARE(v1->length(), 1);
  QCOMPARE(v1->value(0), &Kst::NOPOINT);

  Q_ASSERT(v1);
  v1->resize(15);
  QCOMPARE(v1->length(), 15);
  v1->zero();
  for (int i = 0; i < 15; ++i) {
    QCOMPARE(v1->value()[i], 0.0);
  }
  QVERIFY(!v1->isScalarList());
  v1->value()[0] = -42;
  v1->resize(3);
  QCOMPARE(v1->length(), 3);
  QCOMPARE(v1->value()[0], -42.0);

  Kst::VectorPtr v2 = Kst::kst_cast<Kst::Vector>(_store.createObject<Kst::Vector>());
  Q_ASSERT(v2);
  QCOMPARE(v2->length(), 1);
  v2 = Kst::kst_cast<Kst::Vector>(_store.createObject<Kst::Vector>());
  v2->resize(1);
  QCOMPARE(v2->length(), 1);
  v2 = Kst::kst_cast<Kst::Vector>(_store.createObject<Kst::Vector>());
  v2->resize(2);
  QCOMPARE(v2->length(), 2);

  v2->resize(3);
  double *data = v2->value();
  data[0] = 1;
  data[1] = 2;
  data[2] = 3;
  QCOMPARE(v2->interpolate(0, 5), 1.0);
  QCOMPARE(v2->interpolate(1, 5), 1.5);
  QCOMPARE(v2->interpolate(2, 5), 2.0);
  QCOMPARE(v2->interpolate(3, 5), 2.5);
  QCOMPARE(v2->interpolate(4, 5), 3.0);
}

#ifdef KST_USE_QTEST_MAIN
QTEST_MAIN(TestVector)
#endif

// vim: ts=2 sw=2 et
