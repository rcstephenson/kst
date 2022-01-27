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

#ifndef DATABUTTONACTION_H
#define DATABUTTONACTION_H

#include <QWidgetAction>

namespace Kst {

class Document;

class DataButtonAction : public QWidgetAction
{
  Q_OBJECT
  public:
    explicit DataButtonAction(const QString &text, QWidget *parent = 0);

  Q_SIGNALS:
    void triggered(QString &name);

  public Q_SLOTS:
    void triggerWithName();

  protected:
    QWidget* createWidget(QWidget* parent = 0);

  private:
    QString m_text;
};

}

#endif

// vim: ts=2 sw=2 et
