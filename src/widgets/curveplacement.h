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

#ifndef CURVEPLACEMENT_H
#define CURVEPLACEMENT_H

#include <QWidget>
#include "ui_curveplacement.h"

#include "kstwidgets_export.h"

namespace Kst {

class PlotItemInterface;

class KSTWIDGETS_EXPORT CurvePlacement : public QWidget, public Ui::CurvePlacement {
  Q_OBJECT
  public:
    enum Place { NewPlot, ExistingPlot, NoPlot, NewPlotNewTab };
    enum Layout { Auto, Custom, Protect };
    explicit CurvePlacement(QWidget *parent = 0);
    virtual ~CurvePlacement();

    Place place() const;
    void setPlace(Place place);

    bool scaleFonts() const;

    PlotItemInterface *existingPlot() const;
    void setExistingPlots(const QList<PlotItemInterface*> &existingPlots);
    void setCurrentPlot(const PlotItemInterface *currentPlot);
    void updatePlotListCombo();

    Layout layout() const;
    void setLayout(Layout layout);

    int gridColumns() const;
    void setGridColumns(int gridColumns);

    virtual bool event(QEvent * event);
  public Q_SLOTS:
    void updateButtons();
  private:
    QList<PlotItemInterface*> _plots;
};

}

#endif

// vim: ts=2 sw=2 et
