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

#ifndef SCRIPTSERVER_H
#define SCRIPTSERVER_H

#ifndef CALL_MEMBER_FN
#define CALL_MEMBER_FN(object,ptrToMember)  ((object).*(ptrToMember))
#endif

#include "objectstore.h"
#include "scriptinterface.h"
#include <QLocalServer>
#include <QMap>

namespace Kst {

class ViewItem;

class ScriptServer;

typedef QByteArray (ScriptServer::*ScriptMemberFn)(QByteArray& command, QLocalSocket* s,ObjectStore*_store);

class ScriptServer : public QObject
{
    Q_OBJECT
    QLocalServer* _server;
    ObjectStore* _store;
    ScriptInterface* _interface;
    bool _curMacComEcho;
    QList<ViewItem*> vi;    // cache
    QMap<QByteArray,ScriptMemberFn> _fnMap;
public:
    explicit ScriptServer(ObjectStore*obj);
    ~ScriptServer();
    //QByteArray checkPrimatives(QByteArray&command,QLocalSocket* s);
    void setStore(ObjectStore *obj) { _store = obj; vi.clear();}
public slots:
    void procConnection();
    void readSomething();
    QByteArray exec(QByteArray command,QLocalSocket* s);

protected:
    QByteArray noSuchFn(QByteArray& , QLocalSocket*,ObjectStore*) {return ""; }

    // ObjectStore
    QByteArray getVectorList(QByteArray& command, QLocalSocket* s,ObjectStore*_store);
    QByteArray newDataVector(QByteArray& command, QLocalSocket* s,ObjectStore*_store);
    QByteArray newGeneratedVector(QByteArray& command, QLocalSocket* s,ObjectStore*_store);

    QByteArray getEditableVectorList(QByteArray& command, QLocalSocket* s,ObjectStore*_store);
    QByteArray newEditableVectorAndGetHandle(QByteArray& command, QLocalSocket* s,ObjectStore*_store);

    QByteArray getMatrixList(QByteArray& command, QLocalSocket* s,ObjectStore*_store);
    QByteArray newDataMatrix(QByteArray& command, QLocalSocket* s,ObjectStore*_store);

    QByteArray getEditableMatrixList(QByteArray& command, QLocalSocket* s,ObjectStore*_store);
    QByteArray newEditableMatrixAndGetHandle(QByteArray& command, QLocalSocket* s,ObjectStore*_store);

    QByteArray getScalarList(QByteArray& command, QLocalSocket* s,ObjectStore*_store);
    QByteArray newGeneratedScalar(QByteArray& command, QLocalSocket* s,ObjectStore*_store);
    QByteArray newDataScalar(QByteArray& command, QLocalSocket* s,ObjectStore*_store);
    QByteArray newVScalar(QByteArray& command, QLocalSocket* s,ObjectStore*_store);

    QByteArray getStringList(QByteArray& command, QLocalSocket* s,ObjectStore*_store);
    QByteArray newGeneratedString(QByteArray& command, QLocalSocket* s,ObjectStore*_store);
    QByteArray newDataString(QByteArray& command, QLocalSocket* s,ObjectStore*_store);

    QByteArray getCurveList(QByteArray& command, QLocalSocket* s,ObjectStore*_store);
    QByteArray newCurve(QByteArray& command, QLocalSocket* s,ObjectStore*_store);

    QByteArray getEquationList(QByteArray& command, QLocalSocket* s,ObjectStore*_store);
    QByteArray newEquation(QByteArray& command, QLocalSocket* s,ObjectStore*_store);

    QByteArray getHistogramList(QByteArray& command, QLocalSocket* s,ObjectStore*_store);
    //QByteArray newHistogram(QByteArray& command, QLocalSocket* s,ObjectStore*_store);

    QByteArray getPSDList(QByteArray& command, QLocalSocket* s,ObjectStore*_store);
    QByteArray newPSD(QByteArray& command, QLocalSocket* s,ObjectStore*_store);

    QByteArray getPluginList(QByteArray& command, QLocalSocket* s,ObjectStore*_store);
    QByteArray newPlugin(QByteArray& command, QLocalSocket* s,ObjectStore*_store);

    QByteArray getImageList(QByteArray& command, QLocalSocket* s,ObjectStore*_store);
    QByteArray newImage(QByteArray& command, QLocalSocket* s,ObjectStore*_store);

    QByteArray getCSDList(QByteArray& command, QLocalSocket* s,ObjectStore*_store);
    //QByteArray newCSD(QByteArray& command, QLocalSocket* s,ObjectStore*_store);

    QByteArray getBasicPluginList(QByteArray& command, QLocalSocket* s,ObjectStore*_store);
    QByteArray getBasicPluginTypeList(QByteArray& command, QLocalSocket* s,ObjectStore*_store);
    //QByteArray newBasicPlugin(QByteArray& command, QLocalSocket* s,ObjectStore*_store);

    QByteArray getArrowList(QByteArray& command, QLocalSocket* s,ObjectStore*_store);
    QByteArray newArrow(QByteArray& command, QLocalSocket* s,ObjectStore*_store);

    QByteArray getBoxList(QByteArray& command, QLocalSocket* s,ObjectStore*_store);
    QByteArray newBox(QByteArray& command, QLocalSocket* s,ObjectStore*_store);

    QByteArray getButtonList(QByteArray& command, QLocalSocket* s,ObjectStore*_store);
    QByteArray newButton(QByteArray& command, QLocalSocket* s,ObjectStore*_store);

    QByteArray getLineEditList(QByteArray& command, QLocalSocket* s,ObjectStore*_store);
    QByteArray newLineEdit(QByteArray& command, QLocalSocket* s,ObjectStore*_store);

    QByteArray getCircleList(QByteArray& command, QLocalSocket* s,ObjectStore*_store);
    QByteArray newCircle(QByteArray& command, QLocalSocket* s,ObjectStore*_store);

    QByteArray getEllipseList(QByteArray& command, QLocalSocket* s,ObjectStore*_store);
    QByteArray newEllipse(QByteArray& command, QLocalSocket* s,ObjectStore*_store);

    QByteArray getLabelList(QByteArray& command, QLocalSocket* s,ObjectStore*_store);
    QByteArray newLabel(QByteArray& command, QLocalSocket* s,ObjectStore*_store);

    QByteArray getLineList(QByteArray& command, QLocalSocket* s,ObjectStore*_store);
    QByteArray newLine(QByteArray& command, QLocalSocket* s,ObjectStore*_store);

    QByteArray getPictureList(QByteArray& command, QLocalSocket* s,ObjectStore*_store);
    QByteArray newPicture(QByteArray& command, QLocalSocket* s,ObjectStore*_store);

    QByteArray getPlotList(QByteArray& command, QLocalSocket* s,ObjectStore*_store);
    QByteArray newPlot(QByteArray& command, QLocalSocket* s,ObjectStore*_store);

    //QByteArray getSharedAxisBoxList(QByteArray& command, QLocalSocket* s,ObjectStore*_store);
    //QByteArray newSharedAxisBox(QByteArray& command, QLocalSocket* s,ObjectStore*_store);

#ifndef KST_NO_SVG
    QByteArray getSvgItemList(QByteArray& command, QLocalSocket* s,ObjectStore*_store);
    QByteArray newSvgItem(QByteArray& command, QLocalSocket* s,ObjectStore*_store);
#endif

    // Access to interfaces
    QByteArray beginEdit(QByteArray& command, QLocalSocket* s,ObjectStore*_store);
    QByteArray endEdit(QByteArray& command, QLocalSocket* s,ObjectStore*_store);

    // Quit:
    QByteArray done(QByteArray& command, QLocalSocket* s,ObjectStore*_store);

    // Clear:
    QByteArray clear(QByteArray& command, QLocalSocket* s,ObjectStore*_store);

    // Destruction is much easier than construction.
    QByteArray eliminate(QByteArray& command, QLocalSocket* s,ObjectStore*_store);

    // General
    QByteArray tabCount(QByteArray& command, QLocalSocket* s,ObjectStore*_store);
    QByteArray newTab(QByteArray& command, QLocalSocket* s,ObjectStore*_store);
    QByteArray setTab(QByteArray& command, QLocalSocket* s,ObjectStore*_store);

    QByteArray screenBack(QByteArray& command, QLocalSocket* s,ObjectStore*_store);
    QByteArray screenForward(QByteArray& command, QLocalSocket* s,ObjectStore*_store);
    QByteArray countFromEnd(QByteArray& command, QLocalSocket* s,ObjectStore*_store);
    QByteArray readToEnd(QByteArray& command, QLocalSocket* s,ObjectStore*_store);
    QByteArray setPaused(QByteArray& command, QLocalSocket* s,ObjectStore*_store);
    QByteArray unsetPaused(QByteArray& command, QLocalSocket* s,ObjectStore*_store);

    QByteArray fileOpen(QByteArray& command, QLocalSocket* s,ObjectStore*_store);
    QByteArray fileSave(QByteArray& command, QLocalSocket* s,ObjectStore*_store);


    // Hacks
    QByteArray editableVectorSetBinaryArray(QByteArray& command, QLocalSocket* s,ObjectStore*_store);
    //EditableVector::setBinaryArray(

    QByteArray editableMatrixSetBinaryArray(QByteArray& command, QLocalSocket* s,ObjectStore*_store);
    //EditableMatrix::setBinaryArray(

    QByteArray editableVectorSet(QByteArray& command, QLocalSocket* s,ObjectStore*_store);
    //EditableVector::set(

    QByteArray vectorGetBinaryArray(QByteArray& command, QLocalSocket* s,ObjectStore*_store);
    //Vector::getBinaryArray(

    QByteArray matrixGetBinaryArray(QByteArray& command, QLocalSocket*s,ObjectStore*_store);
    //Matrix::getBinaryArray(

    QByteArray stringValue(QByteArray& command, QLocalSocket* s,ObjectStore*_store);
    //String::value(

    QByteArray stringSetValue(QByteArray& command, QLocalSocket* s,ObjectStore*_store);
    //String::setValue(

    QByteArray scalarValue(QByteArray& command, QLocalSocket* s,ObjectStore*_store);
    //Scalar::value(

    QByteArray scalarSetValue(QByteArray& command, QLocalSocket* s,ObjectStore*_store);
    //Scalar::setValue(

};


}

#endif // SCRIPTSERVER_H
