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

#include "colorbutton.h"

#include <QPainter>
#include <QColorDialog>
#include <QDebug>

namespace Kst {

ColorButton::ColorButton(QWidget *parent)
  : QToolButton(parent), _color(Qt::black) {
  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  connect (this, SIGNAL(clicked()), this, SLOT(chooseColor()));
}


ColorButton::ColorButton(const QColor &color, QWidget *parent)
  : QToolButton(parent), _color(color) {
  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  connect (this, SIGNAL(clicked()), this, SLOT(chooseColor()));
}


ColorButton::~ColorButton() {
}


QColor ColorButton::color() const {
  return _color;
}


void ColorButton::clearSelection() {
  setColor(QColor(Qt::transparent));
}


bool ColorButton::colorDirty() const {
  return _color != QColor(Qt::transparent);
}


void ColorButton::setColor(const QColor &color) {
  _color = color;
  update();
  emit changed(color);
}


void ColorButton::paintEvent(QPaintEvent *event)
{
  QToolButton::paintEvent(event);
  if (!isEnabled())
      return;

  QPainter painter(this);
  QBrush brush(_color);
  int m = (rect().height())/6;
#ifndef QT5 // TODO 
  qDrawShadePanel(&painter, rect().x() + m, rect().y() + m, rect().width() - 2*m, rect().height() - 2*m,
                  palette(), /*sunken*/ isDown(), /*lineWidth*/ 1, /*fill*/ &brush);
#endif
}


void ColorButton::chooseColor() {

  bool ok;
  if (_color == Qt::transparent) {
    _color = Qt::black;
  }
  QColor color = QColorDialog::getColor(_color, parentWidget(), "Choose Color", QColorDialog::ShowAlphaChannel);

  if (color.isValid()) {
    setColor(color);
  }
}

}

// vim: ts=2 sw=2 et
