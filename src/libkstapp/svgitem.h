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

#ifndef SVGITEM_H
#define SVGITEM_H

#include "viewitem.h"
#include "graphicsfactory.h"

class QSvgRenderer;

namespace Kst {

class SvgItem : public ViewItem
{
  Q_OBJECT
  public:
    explicit SvgItem(View *parent, const QString &file = QString());
    ~SvgItem();

    const QString defaultsGroupName() const {return QString("picture");}

    // for view item dialogs
    virtual bool hasStroke() const {return false;}
    virtual bool hasBrush() const {return false;}

    virtual void save(QXmlStreamWriter &xml);
    virtual void paint(QPainter *painter);

    void setSvgData(const QByteArray &svgData);
    virtual void creationPolygonChanged(View::CreationEvent event);

  private:
    QSvgRenderer *_svg;
    QByteArray _svgData;
};


class CreateSvgCommand : public CreateCommand
{
  Q_OBJECT
  public:
    CreateSvgCommand() : CreateCommand(QObject::tr("Create Svg")) {}
    CreateSvgCommand(View *view) : CreateCommand(view, QObject::tr("Create Svg")) {}
    ~CreateSvgCommand() {}
    void createItem();
};


class SvgItemFactory : public GraphicsFactory {
  public:
    SvgItemFactory();
    ~SvgItemFactory();
    ViewItem* generateGraphics(QXmlStreamReader& stream, ObjectStore *store, View *view, ViewItem *parent = 0);
};

}

#endif

// vim: ts=2 sw=2 et
