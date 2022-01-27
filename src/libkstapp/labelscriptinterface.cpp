/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2011 Joshua Netterfield                               *
 *                   joshua.netterfield@gmail.com                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "labelscriptinterface.h"

#include <QStringBuilder>

namespace Kst {

struct LabelTabSI {
    LabelItem* item;
    QByteArrayList commands() {
        QByteArrayList ba;
        ba<<"setLabel("<<"setLabelColor("<<"setLabelColor("<<"checkLabelItalic()"<<"uncheckLabelItalic()"<<
             "checkLabelBold()"<<"checkLabelBold()"<<"setFontSize("<<"setFontFamily(";
        return ba;
    }
    QString doCommand(QString x) {
        if(x.startsWith("setLabel(")) {
            x.remove("setLabel(").chop(1);
            item->setLabelText(x);
            return "Done";
        } else if(x.startsWith("setLabelColor(")) {
            item->setLabelColor(QColor(x.remove("setLabelColor(").remove(')')));
        } else if(x.contains("Italic")) {
            QFont f=item->labelFont();
            f.setItalic(!x.contains("un"));
            item->setLabelFont(f);
            return "Done";
        } else if(x.contains("Bold")) {
            QFont f=item->labelFont();
            f.setBold(!x.contains("un"));
            item->setLabelFont(f);
            return "Done";
        } else if(x.contains("setFont")) {
            QFont f=item->labelFont();
            if(x.contains("Size")) {
                item->setLabelScale(x.remove("setFontSize(").remove(')').toInt());
            } else if(x.contains("Family")) {
                f.setFamily(x.remove("setFontFamily(").remove(')'));
            } else {
                return "";
            }
            item->setLabelFont(f);
            return "Done";
        }
        return "";
    }
};

LabelSI::LabelSI(LabelItem *it) : layout(new LayoutTabSI), dim(new DimensionTabSI), lab(new LabelTabSI) {
    layout->vi=it;
    dim->item=it;
    lab->item=it;
}

QString LabelSI::doCommand(QString x) {

  QString v=doNamedObjectCommand(x, dim->item);

  if (v.isEmpty()) {
    v=layout->doCommand(x);
  }
  if (v.isEmpty()) {
    v=dim->doCommand(x);
  }
  if (v.isEmpty()) {
    v=lab->doCommand(x);
  }
  return v.isEmpty()?"No command":v;
}

bool LabelSI::isValid() {
    return dim->item;
}

ScriptInterface* LabelSI::newLabel() {
    LabelItem* bi=new LabelItem(kstApp->mainWindow()->tabWidget()->currentView(),"");
    kstApp->mainWindow()->tabWidget()->currentView()->scene()->addItem(bi);
    return new LabelSI(bi);
}


}
