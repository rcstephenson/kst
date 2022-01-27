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

#ifndef GRADIENTEDITOR_H
#define GRADIENTEDITOR_H

#include <QHash>
#include <QWidget>
#include <QGradient>

#include "kstwidgets_export.h"

namespace Kst {

class KSTWIDGETS_EXPORT GradientEditor : public QWidget {
  Q_OBJECT
public:
  explicit GradientEditor(QWidget *parent = 0);
  virtual ~GradientEditor();

  virtual QSize sizeHint() const;

  QGradient gradient() const;

  void setDefaultGradientStops(QGradientStops stops);

  bool dirty() const { return _dirty; }

public Q_SLOTS:
  void setGradient(const QGradient &gradient);
  void resetGradient();

Q_SIGNALS:
  void changed(const QGradient &gradient);

protected:
  virtual void mousePressEvent(QMouseEvent *event);
  virtual void mouseReleaseEvent(QMouseEvent *event);
  virtual void mouseMoveEvent(QMouseEvent *event);
  virtual void paintEvent(QPaintEvent *event);

private:
  bool isStopAt(const QPoint &point) const;
  QPainterPath marker(int x) const;

  QGradientStops gradientStops() const;
  void setGradientStops(const QGradientStops &stops);
  void clearGradientStops();

  struct Stop {
    int pos;
    QColor color;
    QPainterPath path;
  };

private:
  QGradient *_gradient;
  QHash<int, Stop> _stopHash;
  int _movingStop;
  QGradientStops _defaultGradientStops;
  bool _dirty;
};

}

#endif

// vim: ts=2 sw=2 et
