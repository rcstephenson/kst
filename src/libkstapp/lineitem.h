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

#ifndef LINEITEM_H
#define LINEITEM_H

#include "viewitem.h"
#include "graphicsfactory.h"

namespace Kst {

class LineItemDialog;

class LineItem : public ViewItem
{
  Q_OBJECT
  public:
    explicit LineItem(View *parent);
    virtual ~LineItem();

    const QString defaultsGroupName() const {return LineItem::staticDefaultsGroupName();}
    static QString staticDefaultsGroupName() { return QString("line");}

    // for view item dialogs
    virtual bool hasStroke() const {return true;}
    virtual bool hasBrush() const {return false;}

    QLineF line() const;
    //void setLine(const QLineF &line);

    virtual void save(QXmlStreamWriter &xml);
    virtual void paint(QPainter *painter);

    virtual QPainterPath grips() const;

    virtual QPointF centerOfRotation() const;

    virtual void updateDataRelativeRect(bool force = false);
    virtual void applyDataLockedDimensions();
    virtual void clearEditDialogPtr() {_lineEditDialog = 0;}

    virtual bool customDimensionsTab() {return true;}

  protected Q_SLOTS:
    virtual void creationPolygonChanged(View::CreationEvent event);

  protected:
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    virtual void hoverMoveEvent(QGraphicsSceneHoverEvent *event);

    virtual QPainterPath leftMidGrip() const;
    virtual QPainterPath rightMidGrip() const;

  private:
    virtual void updateChildGeometry(const QRectF &oldParentRect, const QRectF &newParentRect);
    bool _created;
    LineItemDialog *_lineEditDialog;

  public Q_SLOTS:
    virtual void edit();

  private:

};

class CreateLineCommand : public CreateCommand
{
  public:
    CreateLineCommand() : CreateCommand(QObject::tr("Create Line")) {}
    CreateLineCommand(View *view) : CreateCommand(view, QObject::tr("Create Line")) {}
    virtual ~CreateLineCommand() {}
    virtual void createItem();
};

class LineItemFactory : public GraphicsFactory {
  public:
    LineItemFactory();
    ~LineItemFactory();
    ViewItem* generateGraphics(QXmlStreamReader& stream, ObjectStore *store, View *view, ViewItem *parent = 0);
};

}

#endif

// vim: ts=2 sw=2 et
