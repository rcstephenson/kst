/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2008 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef LEGENDITEM_H
#define LEGENDITEM_H

#include "viewitem.h"
#include "graphicsfactory.h"
#include "relation.h"

namespace Label {
  struct Parsed;
}

namespace Kst {

class PlotItem;
class PlotRenderItem;

class LegendItem : public ViewItem {
  Q_OBJECT
  public:
    explicit LegendItem(PlotItem *parent);
    virtual ~LegendItem();

    const QString defaultsGroupName() const {return LegendItem::staticDefaultsGroupName();}
    static QString staticDefaultsGroupName() { return QString("legend");}

    // for view item dialogs
    virtual bool hasStroke() const {return true;}
    virtual bool hasBrush() const {return true;}
    virtual bool hasFont() const {return true;}

    virtual void paint(QPainter *painter);

    void setAutoContents(const bool autoContent);
    bool autoContents() const;

    void setVerticalDisplay(const bool vertical);
    bool verticalDisplay() const;

    QFont font() const;
    void setLegendFont(const QFont &font);

    qreal fontScale() const;
    void setFontScale(const qreal scale);

    QColor legendColor() const;
    void setLegendColor(const QColor &color);

    QString title() const;
    void setTitle(const QString &title);

    virtual void setFont(const QFont &f, const QColor &c);

    PlotItem* plot() { return _plotItem; }

    RelationList relations() { return _relations; }
    void setRelations(RelationList relations) { _relations = relations; }

    virtual void save(QXmlStreamWriter &xml);
    virtual void saveInPlot(QXmlStreamWriter &xml);
    virtual bool configureFromXml(QXmlStreamReader &xml, ObjectStore *store);

    QString descriptionTip() const;

    void saveAsDialogDefaults() const;
    static void saveDialogDefaultsFont(const QFont &F, const QColor &C);

    void applyDefaults();
  public Q_SLOTS:
    virtual void edit();
    virtual void remove();

  protected:
    virtual QString _automaticDescriptiveName() const;
    virtual void _initializeShortName();
  private:
    QSize paintRelation(QString name, RelationPtr relation, QPainter *painter, bool draw);

    PlotItem *_plotItem;
    bool _auto;
    QString _title;
    QFont _font;
    QColor _color;
    qreal _fontScale;
    bool _verticalDisplay;
    RelationList _relations;
};

}

#endif

// vim: ts=2 sw=2 et
