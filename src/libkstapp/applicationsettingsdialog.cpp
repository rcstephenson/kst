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

#include "applicationsettingsdialog.h"

#include "applicationsettings.h"
#include "gridtab.h"
#include "generaltab.h"
#include "dialogpage.h"
#include "defaultlabelpropertiestab.h"
#include "layouttab.h"

#include <QDebug>

namespace Kst {

ApplicationSettingsDialog::ApplicationSettingsDialog(QWidget *parent)
    : Dialog(parent) {

  setWindowTitle(tr("Kst Settings"));
  setAttribute(Qt::WA_DeleteOnClose, false);

  _generalTab = new GeneralTab(this);
  _gridTab = new GridTab(this);
  _defaultLabelPropertiesTab = new DefaultLabelPropertiesTab(this);
  _layoutTab = new LayoutTab(this);

  connect(_generalTab, SIGNAL(apply()), this, SLOT(generalChanged()));
  connect(_gridTab, SIGNAL(apply()), this, SLOT(gridChanged()));
  connect(_defaultLabelPropertiesTab, SIGNAL(apply()), this, SLOT(defaultLabelPropertiesChanged()));
  connect(_layoutTab, SIGNAL(apply()), this, SLOT(layoutChanged()));

  DialogPage *general = new DialogPage(this);
  general->setPageTitle(tr("General"));
  general->addDialogTab(_generalTab);
  addDialogPage(general);

  DialogPage *defaultLabelProperties = new DialogPage(this);
  defaultLabelProperties->setPageTitle(tr("Scaling"));
  defaultLabelProperties->addDialogTab(_defaultLabelPropertiesTab);
  addDialogPage(defaultLabelProperties);

  DialogPage *grid = new DialogPage(this);
  grid->setPageTitle(tr("Grid"));
  grid->addDialogTab(_gridTab);
  addDialogPage(grid);

  DialogPage *layout = new DialogPage(this);
  layout->setPageTitle(tr("Layout Properties"));
  layout->addDialogTab(_layoutTab);
  addDialogPage(layout);

  setupGeneral();
  setupGrid();
  setupDefaultLabelProperties();
  setupLayout();

  selectDialogPage(general);
}


ApplicationSettingsDialog::~ApplicationSettingsDialog() {
}


void ApplicationSettingsDialog::setupGeneral() {
  _generalTab->setUseRaster(ApplicationSettings::self()->useRaster());
  _generalTab->setTransparentDrag(ApplicationSettings::self()->transparentDrag());
  _generalTab->setMinimumUpdatePeriod(ApplicationSettings::self()->minimumUpdatePeriod());
  _generalTab->setAntialiasPlot(ApplicationSettings::self()->antialiasPlots());
}


void ApplicationSettingsDialog::setupGrid() {
  _gridTab->setShowGrid(ApplicationSettings::self()->showGrid());
  _gridTab->setSnapToGrid(ApplicationSettings::self()->snapToGrid());
  _gridTab->setGridHorizontalSpacing(ApplicationSettings::self()->gridHorizontalSpacing());
  _gridTab->setGridVerticalSpacing(ApplicationSettings::self()->gridVerticalSpacing());
}


void ApplicationSettingsDialog::setupDefaultLabelProperties() {
  _defaultLabelPropertiesTab->setReferenceViewWidth(ApplicationSettings::self()->referenceViewWidthCM());
  _defaultLabelPropertiesTab->setReferenceViewHeight(ApplicationSettings::self()->referenceViewHeightCM());
  _defaultLabelPropertiesTab->setMinimumFontSize(ApplicationSettings::self()->minimumFontSize());
}


void ApplicationSettingsDialog::setupLayout() {
  _layoutTab->setHorizontalMargin(ApplicationSettings::self()->layoutMargins().width());
  _layoutTab->setVerticalMargin(ApplicationSettings::self()->layoutMargins().height());
  _layoutTab->setHorizontalSpacing(ApplicationSettings::self()->layoutSpacing().width());
  _layoutTab->setVerticalSpacing(ApplicationSettings::self()->layoutSpacing().height());
}


void ApplicationSettingsDialog::generalChanged() {
  //Need to block the signals so that the modified signal only goes out once...
  ApplicationSettings::self()->blockSignals(true);
  ApplicationSettings::self()->setTransparentDrag(_generalTab->transparentDrag());
  ApplicationSettings::self()->setUseRaster(_generalTab->useRaster());
  ApplicationSettings::self()->setMinimumUpdatePeriod(_generalTab->minimumUpdatePeriod());
  ApplicationSettings::self()->setAntialiasPlots(_generalTab->antialiasPlot());
  ApplicationSettings::self()->blockSignals(false);

  emit ApplicationSettings::self()->modified();
}


void ApplicationSettingsDialog::gridChanged() {
  //Need to block the signals so that the modified signal only goes out once...
  ApplicationSettings::self()->blockSignals(true);
  ApplicationSettings::self()->setShowGrid(_gridTab->showGrid());
  ApplicationSettings::self()->setSnapToGrid(_gridTab->snapToGrid());
  ApplicationSettings::self()->setGridHorizontalSpacing(_gridTab->gridHorizontalSpacing());
  ApplicationSettings::self()->setGridVerticalSpacing(_gridTab->gridVerticalSpacing());
  ApplicationSettings::self()->blockSignals(false);

  emit ApplicationSettings::self()->modified();
}


void ApplicationSettingsDialog::defaultLabelPropertiesChanged() {
  //Need to block the signals so that the modified signal only goes out once...
  ApplicationSettings::self()->blockSignals(true);

  ApplicationSettings::self()->blockSignals(false);
  ApplicationSettings::self()->setReferenceViewWidthCM(_defaultLabelPropertiesTab->referenceViewWidth());
  ApplicationSettings::self()->setReferenceViewHeightCM(_defaultLabelPropertiesTab->referenceViewHeight());
  ApplicationSettings::self()->setMinimumFontSize(_defaultLabelPropertiesTab->minimumFontSize());

  emit ApplicationSettings::self()->modified();
}


void ApplicationSettingsDialog::layoutChanged() {
  //Need to block the signals so that the modified signal only goes out once...
  ApplicationSettings::self()->blockSignals(true);
  ApplicationSettings::self()->setLayoutMargins(QSizeF(_layoutTab->horizontalMargin(), _layoutTab->verticalMargin()));
  ApplicationSettings::self()->setLayoutSpacing(QSizeF(_layoutTab->horizontalSpacing(), _layoutTab->verticalSpacing()));
  ApplicationSettings::self()->blockSignals(false);

  emit ApplicationSettings::self()->modified();
}


}

// vim: ts=2 sw=2 et
