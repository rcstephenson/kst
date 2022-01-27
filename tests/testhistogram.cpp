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

#include "testhistogram.h"

#include <QtTest>

#include <generatedvector.h>
#include <datacollection.h>
#include <objectstore.h>


#include <histogram.h>


static Kst::ObjectStore _store;

void TestHistogram::cleanupTestCase() {
  _store.clear();
}

void TestHistogram::testHistogram() {
  Kst::GeneratedVectorPtr gvp = Kst::kst_cast<Kst::GeneratedVector>(_store.createObject<Kst::GeneratedVector>());
  Q_ASSERT(gvp);
  QCOMPARE(gvp->length(), 1);

  gvp->changeRange(0, 10, 100);
  QCOMPARE(gvp->max(), 0.0);
  QCOMPARE(gvp->length(), 100);

  gvp->internalUpdate();
  QCOMPARE(gvp->max(), 10.0);
  QCOMPARE(gvp->length(), 100);

  Kst::VectorPtr vp(gvp);
  QCOMPARE(vp->max(), 10.0);

  Kst::HistogramPtr h1 = Kst::kst_cast<Kst::Histogram>(_store.createObject<Kst::Histogram>());
  h1->change(vp, 0, 10, 10, Kst::Histogram::Number);

  QVERIFY(!h1->realTimeAutoBin()); // should be false by default
  QCOMPARE(h1->numberOfBins(), 10);

  h1->writeLock();
  h1->internalUpdate();
  h1->unlock();

  QCOMPARE(h1->vMin(), 0.0);
  QCOMPARE(h1->vMax(), 10.0);
  QCOMPARE(h1->vNumSamples(), 100);
  int count = 0;
  for (int i=0; i<10; i++) {
    count += int(h1->vY()->value(i));
  }
  h1->setRealTimeAutoBin(true);
  QVERIFY(h1->realTimeAutoBin());
  //dumpPoints(h1, 10);
  QCOMPARE(count, 100); // should account for the whole vector
  h1->setNumberOfBins(11);
  QVERIFY(!h1->realTimeAutoBin());
  QCOMPARE(h1->numberOfBins(), 11);
  QCOMPARE(h1->vMin(), 0.0);
  QCOMPARE(h1->vMax(), 10.0);
  QCOMPARE(h1->vNumSamples(), 100);

  h1->writeLock();
  h1->internalUpdate();
  h1->unlock();

  count = 0;
  for (int i=0; i<11; i++) {
    count += int(h1->vY()->value(i));
  }
  //dumpPoints(h1, 11);
  QCOMPARE(count, 100); // should still account for the whole vector
  h1->setNumberOfBins(9);
  QCOMPARE(h1->numberOfBins(), 9);
  QCOMPARE(h1->vMin(), 0.0);
  QCOMPARE(h1->vMax(), 10.0);
  QCOMPARE(h1->vNumSamples(), 100);

  h1->writeLock();
  h1->internalUpdate();
  h1->unlock();

  count = 0;
  for (int i=0; i<9; i++) {
    count += int(h1->vY()->value(i));
  }
  //dumpPoints(h1, 9);
  QCOMPARE(count, 100); // should still account for the whole vector
  // min > max

  h1 = Kst::kst_cast<Kst::Histogram>(_store.createObject<Kst::Histogram>());
  h1->change(vp, 10, 0, 10, Kst::Histogram::Number);


  QCOMPARE(h1->numberOfBins(), 10);
  QCOMPARE(h1->xMin(), 0.0);
  QCOMPARE(h1->xMax(), 10.0);
  QCOMPARE(h1->vMin(), 0.0);
  QCOMPARE(h1->vMax(), 10.0);
  QCOMPARE(h1->vNumSamples(), 100);

  // min == max
  h1 = Kst::kst_cast<Kst::Histogram>(_store.createObject<Kst::Histogram>());
  h1->change(vp, 10, 10, 2, Kst::Histogram::Number);

  QCOMPARE(h1->numberOfBins(), 2);
  QCOMPARE(h1->xMin(), 9.0);
  QCOMPARE(h1->xMax(), 11.0);
  QCOMPARE(h1->vMin(), 0.0);
  QCOMPARE(h1->vMax(), 10.0);
  QCOMPARE(h1->vNumSamples(), 100);

  // max < min
  h1 = Kst::kst_cast<Kst::Histogram>(_store.createObject<Kst::Histogram>());
  h1->change(vp, 11, 9, 1, Kst::Histogram::Number);

  QCOMPARE(h1->numberOfBins(), 2);
  QCOMPARE(h1->xMax(), 11.0);
  QCOMPARE(h1->xMin(), 9.0);
  QCOMPARE(h1->vMin(), 0.0);
  QCOMPARE(h1->vMax(), 10.0);
  QCOMPARE(h1->vNumSamples(), 100);
  // set to max == min
  h1->setXRange(10, 10);
  QCOMPARE(h1->xMin(), 9.0);
  QCOMPARE(h1->xMax(), 11.0);
  // set to max > min
  h1->setXRange(1,2);
  QCOMPARE(h1->xMax() - h1->xMin(), 1.0);
  h1->setXRange(8, 10);
  QCOMPARE(h1->xMin(), 8.0);
  QCOMPARE(h1->xMax(), 10.0);
}

#ifdef KST_USE_QTEST_MAIN
QTEST_MAIN(TestHistogram)
#endif

// vim: ts=2 sw=2 et
