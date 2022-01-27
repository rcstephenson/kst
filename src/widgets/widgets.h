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

#ifndef WIDGETS_H
#define WIDGETS_H

#include <QObject>
#include <QDesignerCustomWidgetInterface>

#include <QtPlugin>

#include "colorbutton.h"
#include "gradienteditor.h"
#include "filerequester.h"
#include "combobox.h"
#include "datarange.h"
#include "curveselector.h"
#include "vectorselector.h"
#include "matrixselector.h"
#include "scalarselector.h"
#include "stringselector.h"
#include "curveappearance.h"
#include "curveplacement.h"
#include "fftoptions.h"
#include "colorpalette.h"
#include "datasourceselector.h"
#include "labelbuilder.h"
#include "labellineedit.h"

#ifndef QNX // Qt for QNX doesn't include the designer interfaces by default

namespace Kst {
class Widgets : public QObject, public QDesignerCustomWidgetCollectionInterface {
  Q_INTERFACES(QDesignerCustomWidgetCollectionInterface)
  Q_OBJECT
  public:
    explicit Widgets(QObject *parent = 0);
    virtual ~Widgets();
    QList<QDesignerCustomWidgetInterface*> customWidgets() const {
      return _plugins;
    }

  private:
    QList<QDesignerCustomWidgetInterface*> _plugins;
};

class WidgetPlugin : public QObject, public QDesignerCustomWidgetInterface {
  Q_OBJECT

  public:
    explicit WidgetPlugin(QObject *parent = 0);
    virtual ~WidgetPlugin();

    QString group() const;
    QString toolTip() const;
    QString whatsThis() const;
    QString instanceName() const;
    QString includeFile() const;
    QString domXml() const;
    bool isContainer() const;
    bool isInitialized() const;
    QIcon icon() const;
    void initialize(QDesignerFormEditorInterface *);

  private:
    bool _initialized;
};

class ColorButtonPlugin : public WidgetPlugin {
  Q_OBJECT
  Q_INTERFACES(QDesignerCustomWidgetInterface)
  public:
    explicit ColorButtonPlugin(QObject *parent = 0) : WidgetPlugin(parent) {}
    QString name() const {
      return QLatin1String("ColorButton");
    } //do not translate
    QWidget *createWidget(QWidget *parent) {
      return new ColorButton(parent);
    }
};

class DataSourceSelectorPlugin : public WidgetPlugin {
  Q_OBJECT
  Q_INTERFACES(QDesignerCustomWidgetInterface)
  public:
    explicit DataSourceSelectorPlugin(QObject *parent = 0) : WidgetPlugin(parent) {}
    QString name() const {
      return QLatin1String("DataSourceSelector");
    } //do not translate
    QWidget *createWidget(QWidget *parent) {
      return new DataSourceSelector(parent);
    }
};

class GradientEditorPlugin : public WidgetPlugin {
  Q_OBJECT
  Q_INTERFACES(QDesignerCustomWidgetInterface)
  public:
    explicit GradientEditorPlugin(QObject *parent = 0) : WidgetPlugin(parent) {}
    QString name() const {
      return QLatin1String("GradientEditor");
    } //do not translate
    QWidget *createWidget(QWidget *parent) {
      return new GradientEditor(parent);
    }
};

class FileRequesterPlugin : public WidgetPlugin {
  Q_OBJECT
  Q_INTERFACES(QDesignerCustomWidgetInterface)
  public:
    explicit FileRequesterPlugin(QObject *parent = 0) : WidgetPlugin(parent) {}
    QString name() const {
      return QLatin1String("FileRequester");
    } //do not translate
    QWidget *createWidget(QWidget *parent) {
      return new FileRequester(parent);
    }
};

class ComboBoxPlugin : public WidgetPlugin {
  Q_OBJECT
  Q_INTERFACES(QDesignerCustomWidgetInterface)
  public:
    explicit ComboBoxPlugin(QObject *parent = 0) : WidgetPlugin(parent) {}
    QString name() const {
      return QLatin1String("ComboBox");
    } //do not translate
    QWidget *createWidget(QWidget *parent) {
      return new ComboBox(parent);
    }
};

class DataRangePlugin : public WidgetPlugin {
  Q_OBJECT
  Q_INTERFACES(QDesignerCustomWidgetInterface)
  public:
    explicit DataRangePlugin(QObject *parent = 0) : WidgetPlugin(parent) {}
    QString name() const {
      return QLatin1String("DataRange");
    } //do not translate
    QWidget *createWidget(QWidget *parent) {
      return new DataRange(parent);
    }
};

class CurveSelectorPlugin : public WidgetPlugin {
  Q_OBJECT
  Q_INTERFACES(QDesignerCustomWidgetInterface)
  public:
    explicit CurveSelectorPlugin(QObject *parent = 0) : WidgetPlugin(parent) {}
    QString name() const {
      return QLatin1String("CurveSelector");
    } //do not translate
    QWidget *createWidget(QWidget *parent) {
      return new CurveSelector(parent);
    }
};

class VectorSelectorPlugin : public WidgetPlugin {
  Q_OBJECT
  Q_INTERFACES(QDesignerCustomWidgetInterface)
  public:
    explicit VectorSelectorPlugin(QObject *parent = 0) : WidgetPlugin(parent) {}
    QString name() const {
      return QLatin1String("VectorSelector");
    } //do not translate
    QWidget *createWidget(QWidget *parent) {
      return new VectorSelector(parent);
    }
};

class MatrixSelectorPlugin : public WidgetPlugin {
  Q_OBJECT
  Q_INTERFACES(QDesignerCustomWidgetInterface)
  public:
    explicit MatrixSelectorPlugin(QObject *parent = 0) : WidgetPlugin(parent) {}
    QString name() const {
      return QLatin1String("MatrixSelector");
    } //do not translate
    QWidget *createWidget(QWidget *parent) {
      return new MatrixSelector(parent);
    }
};

class ScalarSelectorPlugin : public WidgetPlugin {
  Q_OBJECT
  Q_INTERFACES(QDesignerCustomWidgetInterface)
  public:
    explicit ScalarSelectorPlugin(QObject *parent = 0) : WidgetPlugin(parent) {}
    QString name() const {
      return QLatin1String("ScalarSelector");
    } //do not translate
    QWidget *createWidget(QWidget *parent) {
      return new ScalarSelector(parent);
    }
};

class StringSelectorPlugin : public WidgetPlugin {
  Q_OBJECT
  Q_INTERFACES(QDesignerCustomWidgetInterface)
  public:
    explicit StringSelectorPlugin(QObject *parent = 0) : WidgetPlugin(parent) {}
    QString name() const {
      return QLatin1String("StringSelector");
    } //do not translate
    QWidget *createWidget(QWidget *parent) {
      return new StringSelector(parent);
    }
};

class CurveAppearancePlugin : public WidgetPlugin {
  Q_OBJECT
  Q_INTERFACES(QDesignerCustomWidgetInterface)
  public:
    explicit CurveAppearancePlugin(QObject *parent = 0) : WidgetPlugin(parent) {}
    QString name() const {
      return QLatin1String("CurveAppearance");
    } //do not translate
    QWidget *createWidget(QWidget *parent) {
      return new CurveAppearance(parent);
    }
};

class CurvePlacementPlugin : public WidgetPlugin {
  Q_OBJECT
  Q_INTERFACES(QDesignerCustomWidgetInterface)
  public:
    explicit CurvePlacementPlugin(QObject *parent = 0) : WidgetPlugin(parent) {}
    QString name() const {
      return QLatin1String("CurvePlacement");
    } //do not translate
    QWidget *createWidget(QWidget *parent) {
      return new CurvePlacement(parent);
    }
};

class FFTOptionsPlugin : public WidgetPlugin {
  Q_OBJECT
  Q_INTERFACES(QDesignerCustomWidgetInterface)
  public:
    explicit FFTOptionsPlugin(QObject *parent = 0) : WidgetPlugin(parent) {}
    QString name() const {
      return QLatin1String("FFTOptions");
    } //do not translate
    QWidget *createWidget(QWidget *parent) {
      return new FFTOptions(parent);
    }
};


class ColorPalettePlugin : public WidgetPlugin {
  Q_OBJECT
  Q_INTERFACES(QDesignerCustomWidgetInterface)
  public:
    explicit ColorPalettePlugin(QObject *parent = 0) : WidgetPlugin(parent) {}
    QString name() const {
      return QLatin1String("ColorPalette");
    } //do not translate
    QWidget *createWidget(QWidget *parent) {
      return new ColorPalette(parent);
    }
};

class LabelBuilderPlugin : public WidgetPlugin {
  Q_OBJECT
  Q_INTERFACES(QDesignerCustomWidgetInterface)
  public:
    explicit LabelBuilderPlugin(QObject *parent = 0) : WidgetPlugin(parent) {}
    QString name() const {
      return QLatin1String("LabelBuilder");
    } //do not translate
    QWidget *createWidget(QWidget *parent) {
      return new LabelBuilder(parent);
    }
};

class LabelLineEditPlugin : public WidgetPlugin {
  Q_OBJECT
  Q_INTERFACES(QDesignerCustomWidgetInterface)
  public:
    explicit LabelLineEditPlugin(QObject *parent = 0) : WidgetPlugin(parent) {}
    QString name() const {
      return QLatin1String("LabelLineEdit");
    } //do not translate
    QWidget *createWidget(QWidget *parent) {
      return new LabelLineEdit(parent);
    }
};

}

#endif // __QNX__

#endif

// vim: ts=2 sw=2 et
