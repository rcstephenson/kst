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

#ifndef KST_DATA_SOURCE_PLUGIN_MANAGER_H
#define KST_DATA_SOURCE_PLUGIN_MANAGER_H


#include "dataplugin.h"
#include "sharedptr.h"
#include "datasource.h"

#include <QSettings>
#include <QMap>


namespace Kst {

class ObjectStore;



class KSTCORE_EXPORT DataSourcePluginManager
{
  public:

    static const QMap<QString, QString> urlMap();

    static void init();
    static void cleanupForExit();
    static void initPlugins();

    /** Returns a list of plugins found on the system. */
    static QStringList pluginList();

    static QString pluginFileName(const QString& pluginName);

    static SharedPtr<DataSource> loadSource(ObjectStore *store, const QString& filename, const QString& type = QString());
    static SharedPtr<DataSource> loadSource(ObjectStore *store, QDomElement& e);
    static SharedPtr<DataSource> findOrLoadSource(ObjectStore *store, const QString& filename);
    static bool validSource(const QString& filename);

    static bool sourceHasConfigWidget(const QString& filename, const QString& type = QString());
    static DataSourceConfigWidget *configWidgetForSource(const QString& filename, const QString& type = QString());

    static bool pluginHasConfigWidget(const QString& plugin);
    static DataSourceConfigWidget *configWidgetForPlugin(const QString& plugin);

    //static QStringList matrixListForSource(const QString& filename, const QString& type = QString(), QString *outType = 0L, bool *complete = 0L);
    static QStringList scalarListForSource(const QString& filename, const QString& type = QString(), QString *outType = 0L, bool *complete = 0L);
    static QStringList stringListForSource(const QString& filename, const QString& type = QString(), QString *outType = 0L, bool *complete = 0L);


  private:
    static QSettings& settingsObject();
    static QMap<QString,QString> url_map;
    static QString obtainFile(const QString& source);

    struct PluginSortContainer {
      SharedPtr<DataSourcePluginInterface> plugin;
      int match;
      int operator<(const PluginSortContainer& x) const;
      int operator==(const PluginSortContainer& x) const;
    };
    static QList<PluginSortContainer> bestPluginsForSource(const QString& filename, const QString& type);
    static DataSourcePtr findPluginFor(ObjectStore *store, const QString& filename, const QString& type, const QDomElement& e = QDomElement());
};

}


#endif
