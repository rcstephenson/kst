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

#include "databuttonaction.h"
#include "databutton.h"

namespace Kst {

DataButtonAction::DataButtonAction(const QString &text, QWidget *parent)
  : QWidgetAction(parent),
    m_text(text) {
}

QWidget * DataButtonAction::createWidget ( QWidget * parent ) {
  DataButton *button = new DataButton(m_text, parent);
  connect(button, SIGNAL(clicked()), this, SLOT(trigger()));
  connect(button, SIGNAL(clicked()), this, SLOT(triggerWithName()));
  return button;
}

void DataButtonAction::triggerWithName() {
  emit triggered(m_text);
}

}

// vim: ts=2 sw=2 et
