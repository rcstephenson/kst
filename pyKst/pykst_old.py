import sip
sip.setapi('QString', 1)
import sys
import math
import os
import ctypes
from time import sleep
from PyQt4 import QtCore, QtNetwork
from numpy import *
from pykstpp import * 

def b2str(val):
  if isinstance(val, bool):
    return "true" if val else "false"
  else:
    return str(val)

class Client:
  """ This class is an interface to a running kst session. Every convenience class inside pykst accepts an instance of Client which it
  uses to interact with a kst session. In addition, it holds functions which effect the entire kst session.
      
  If serverName is specified, creates a connection to either a running kst session with serverName, or if none exists, a new one.
  If serverName is not specified, creates a connection to either the kst session with the name "kstScript", or if none exists, a new one. """
  
  def __init__(self,serverName="kstScript"):
    self.ls=QtNetwork.QLocalSocket()
    self.ls.connectToServer(serverName)
    self.ls.waitForConnected(300)
    self.serverName=serverName
    if self.ls.state()==QtNetwork.QLocalSocket.UnconnectedState:
      os.system("kst2 --serverName="+str(serverName)+"&")
      while self.ls.state()==QtNetwork.QLocalSocket.UnconnectedState:
        self.ls.connectToServer(serverName)
        self.ls.waitForConnected(300)
        self.serverName=serverName
    
  def send(self,command):
    """ Sends a command to kst and returns a response. You should never use this directly, as there is no guarantee that the internal command
        list kst uses won't change. Instead use the convenience classes included with pykst. """
    ba=QtCore.QByteArray(b2str(command))
    self.ls.write(ba)
    self.ls.flush()
    self.ls.waitForReadyRead(300000)
    x=self.ls.readAll()
    return x
    
  def send_si(self, handle, command):
    self.send(b2str("beginEdit("+handle.toAscii()+")"))
    x = self.send(command)
    self.send(b2str("endEdit()"))
    return x

  def getArray(self,command):
    """ Sends a request for a numPy.array. You should never use this directly, as there is no guarantee that the internal command list kst
        uses won't change. Instead use the convenience classes included with pykst. """
    ret=array([0.0])
    get_arr(ret,self.serverName,command)
    return ret

  def getArray2D(self,command):
    """ Sends a request for a numPy.array. You should never use this directly, as there is no guarantee that the internal command list kst
        uses won't change. Instead use the convenience classes included with pykst. """
    ret=array([0.0])
    get_matrix(ret,self.serverName,command)
    return ret

  def clear(self):
    """ Equivalent to file->close from the menubar inside kst.  Clears all objects from kst."""
    self.send("clear()")
  def screenBack(self):
    """ Equivalent to "Range>Back One Screen" from the menubar inside kst. """
    self.send("screenBack()")
  def screenForward(self):
    """ Equivalent to "Range>Forward One Screen" from the menubar inside kst. """
    self.send("screenForward()")
  def countFromEnd(self):
    """ Equivalent to "Range>Count From End" from the menubar inside kst. """
    self.send("countFromEnd()")
  def readToEnd(self):
    """ Equivalent to "Range>Read To End" from the menubar inside kst. """
    self.send("readToEnd()")
  def setPaused(self):
    """ Equivalent to checking "Range>Pause" from the menubar inside kst if "Range>Pause" is unchecked, otherwise no action. """
    self.send("setPaused()")
  def unsetPaused(self):
    """ Equivalent to unchecking "Range>Pause" from the menubar inside kst if "Range>Pause" is checked, otherwise no action. """
    self.send("unsetPaused()")
  def tabCount(self):
    """ Get the number of tabs open in the current document. """
    return self.send("tabCount()")
  def newTab(self):
    """ Create a new tab in the current document and switch to it. """
    return self.send("newTab()")
  def setTab(self,tab):
    """ Set the index of the current tab. It must be greater or equal to 0 and less than tabCount(). """
    self.send("setTab("+b2str(tab)+")")
  def plot(self,*args):
    """ Create a new plot in the current tab in kst with arguments.
    
    Arguments may be 1D numPy arrays, 2D numPy arrays, pykst vectors or pykst matricies. In the case of the first two, they would be imported into kst.

    Formating based loosly on matplotlib.pyplot.plot strings like "r+" is also provided.  
    
    Some examples::
    
      client.plot(x, y)         # plot x and y using default line style and color
      client.plot(x, y, 'bo')   # plot x and y using blue circle markers
      client.plot(y)            # plot y using x as index array 0..N-1
      client.plot(y, 'r+')      # ditto, but with red plusses
    

    See http://matplotlib.sourceforge.net/api/pyplot_api.html#matplotlib.pyplot.plot

    A reference to the created plot is returned. """

    s=-1
    plot=0
    makecurve=False
    for arg in args:
      if s==-1 and makecurve:
        if isinstance(arg,str):
          if '--' in arg:
            curvelinetype=1
            arg=arg.replace('--','',1)
          elif '-..' in arg:
            curvelinetype=4
            arg=arg.replace('-..','',1)
          elif '-.' in arg:
            curvelinetype=3
            arg=arg.replace('-.','',1)
          elif ':' in arg:
            curvelinetype=2
            arg=arg.replace(':','',1)
          elif '-' in arg:
            curvelinetype=0
            arg=arg.replace('-','',1)

          if '.' in arg or ',' in arg or '<' in arg or '>' in arg or '3' in arg or '4' in arg or 'p' in arg or 'h' in arg or 'H' in arg or '|' in arg or '_' in arg:
            print("Warning: you are trying to use a marker type which is present in matplotlib.pyplot but not in pykst")
            usepoints=True
            pointtype=12
          elif 'o' in arg:
            usepoints=True
            pointtype=2
          elif 'v' in arg or '1' in arg:
            usepoints=True
            pointtype=4
          elif '^' in arg or '2' in arg:
            usepoints=True
            pointtype=5
          elif 's' in arg:
            usepoints=True
            pointtype=1
          elif '*' in arg:
            usepoints=True
            pointtype=8
          elif '+' in arg:
            usepoints=True
            pointtype=7
          elif 'x' in arg:
            usepoints=True
            pointtype=0
          elif 'D' in arg or 'd' in arg:
            usepoints=True
            pointtype=12
        
          if 'b' in arg:
            color="blue"
          elif 'g' in arg:
            color="green"
          elif 'r' in arg:
            color="red"
          elif 'c' in arg:
            color="cyan"
          elif 'm' in arg:
            color="magenta"
          elif 'y' in arg:
            color="yellow"
          elif 'k' in arg:
            color="black"
          elif 'w' in arg:
            color="white"
          
          s=-2
        NewCurve(self,x,y,0,0,0,0,False,False,color,curvelinetype,curveweight,
                 uselines,usepoints,pointtype,pointdensity,usehead,headtype,color,usebargraph,bargraphfill,
                 ignoreinauto,donotplaceinanyplot,placeinexistingplot,placeinnewplot)
        plot=ExistingPlot.getList(self)[-1]   #i.e., last
        makecurve=False
        x=0
        y=0
        if s==-2:
          s=-1
          continue
      s+=1
      if s==0:
        while True:
          if isinstance(arg,Vector):
            x=arg
            break
          elif isinstance(arg,Matrix):
            ColorImage(self,arg,0,0,0,0,0,True,0,False,0 if not isinstance(plot,Plot) else plot,True if not isinstance(plot,Plot) else False)
            plot=ExistingPlot(self,ExistingPlot.getList(self)[-1])   #i.e., last
            s=-1
            break
          elif isinstance(arg,ndarray):
            x=EditableVector(self) if arg.ndim==1 else EditableMatrix(self)
            x.setFromList(arg)
            arg=x
          else:
            print("Unknown arg. type...")
            break
      if s==1:
        if isinstance(arg,Vector):
          y=arg
        elif isinstance(arg,ndarray):
          y=EditableVector(self)
          y.setFromList(arg)
        else:
          continue
        color="black"
        curvelinetype=0
        curveweight=2
        uselines=True
        usepoints=False
        pointtype=0
        pointdensity=0
        usehead=False
        headtype=0
        usebargraph=False
        bargraphfill="black"
        ignoreinauto=False
        donotplaceinanyplot=False
        placeinexistingplot=0 if not isinstance(plot,Plot) else plot
        placeinnewplot=True if not isinstance(plot,Plot) else False
        #no args after this
        makecurve=True
        s=-1
    if makecurve:
      NewCurve(self,x,y,0,0,0,0,False,False,color,curvelinetype,curveweight,uselines,usepoints,pointtype,pointdensity,usehead,headtype,color,usebargraph,bargraphfill,ignoreinauto,donotplaceinanyplot,placeinexistingplot,placeinnewplot)                                                                                                                                                                        
      plot=ExistingPlot(self,ExistingPlot.getList(self)[-1])   #i.e., last
    return plot
    

class NamedObject:
  """ This is a class which some convenience classes within pykst use. You should not use it directly. """
  def __init__(self,client):
    self.client=client

  def remove(self):
    """ This removes the object from Kst. You should be careful when using this, because any handles you have to this object will
        still exist and be invalid. """
    self.client.send("eliminate("+self.handle.toAscii()+")")
    
  def getHandle(self):
    return self.handle
    
  def setName(self,name):
    """ Sets a descriptive name for this object. Descriptive names may be non-unique, and as such should not be used as handles. """
    self.client.send("#setName("+self.handle.toAscii()+","+name+")")





class Scalar(NamedObject):
  """ This is a class which some convenience classes within pykst use. You should not use it directly. """
  def __init__(self,client) :
    NamedObject.__init__(self,client)
    
  def value(self):
    """ Returns the value of the scalar as a float. """
    return QtCore.QString(self.client.send("Scalar::value("+self.handle+")")).toFloat()[0]
    
  def setValue(self,val):
    """ This function should only be used with GeneratedScalars and ExistingScalars. """
    self.client.send("Scalar::value("+self.handle+","+b2str(val)+")")
    
class DataSourceScalar(Scalar):
  """ This class represents a scalar you would create via "Create>Scalar>Read from Data Source" from the menubar inside kst.
  
  TODO: implement the configure widget within this widget
  
  The parameters of this function mirror the parameters within "Create>Scalar>Read from Data Source". 
  
  To create a scalar from '/foo.bar' with field foobar and descriptive name "Bob" and print its value::
    
    import pykst as kst
    client = kst.Client()
    dataSrcScalar = kst.DataSourceScalar(client,"/foo.bar","foobar","Bob")
    print(dataSrcScalar.value())"""
      
  def __init__(self,client,filename,field="INDEX",name=""):
    Scalar.__init__(self,client)
    self.handle=QtCore.QString(self.client.send("#newScalarFromDataSource("+b2str(filename)+","+b2str(field)+","+b2str(name)+")"))
    self.handle.remove(0,self.handle.indexOf("ing ")+4)
    
class DataVectorScalar(Scalar):
  """ This class represents a scalar you would create via "Create>Scalar>Read from Data Vector" from the menubar inside kst.
      
  TODO: implement the configure widget within this widget, implement edit functions
  
  The parameters of this function mirror the parameters within "Create>Scalar>Read from Data Vector". """
  def __init__(self,client,filename,field="INDEX",frame=-1,name=""):
    Scalar.__init__(self,client)
    self.handle=QtCore.QString(self.client.send("#newScalarFromDataVector("+b2str(filename)+","+b2str(field)+","+b2str(frame)+","+b2str(name)+")"))
    self.handle.remove(0,self.handle.indexOf("ing ")+4)
    
class GeneratedScalar(Scalar):
  """ This class represents a scalar you would create via "Create>Scalar>Generate" from the menubar inside kst.
  
  TODO: implement edit functions
  
  The parameters of this function mirror the parameters within "Create>Scalar>Generate". """
  def __init__(self,client,val,name=""):
    Scalar.__init__(self,client)
    self.handle=QtCore.QString(self.client.send("#newScalarGenerated("+b2str(val)+","+b2str(name)+")"))
    self.handle.remove(0,self.handle.indexOf("ing ")+4)
    
class ExistingScalar(Scalar):
  """ This class allows access to a scalar created inside kst or through a script given a descriptive or short name.
  
  "handle" is a descriptive or short name of a scalar created inside kst or through a script.
  
  To print out the name and value of all scalars::
    
    import pykst as kst
    client = kst.Client()
    scalars = kst.ExistingScalar.getList(client)
    for s in sclars:
      print(s.getHandle()+":"+s.value()) """
  def __init__(self,client,handle):
    Scalar.__init__(self,client)
    self.handle=handle
    
  @classmethod
  def getList(cls,client):
    """ Returns a python list of all scalars. """
    x=QtCore.QString(client.send("getScalarList()"))
    ret=[]
    while x.contains('['):
      y=QtCore.QString(x)
      y.remove(0,1)
      y.remove(y.indexOf(']'),9999999)
      x.remove(0,x.indexOf(']')+1)
      ret.append(ExistingScalar(client,y))
    return ret





class Vector(NamedObject):
  """ This is a class which some convenience classes within pykst use. You should not use it directly.
  
  "handle" is a descriptive or short name of a scalar created inside kst or through a script. """
  def __init__(self,client) :
    NamedObject.__init__(self,client)
    
  def length(self) :
    """ Returns the count of elements inside the vector. """
    return QtCore.QString(self.client.send("Vector::length("+self.handle+")")).toInt()[0]
  def interpolate(self,in_i,ns_i):
    """  Returns element i of this vector interpolated to have ns_i total samples. """
    return QtCore.QString(self.client.send("Vector::interpolate("+self.handle+","+b2str(in_i)+","+b2str(ns_i)+")")).toDouble()[0]
  def interpolateNoHoles(self,in_i,ns_i):
    """  Returns element i of this vector interpolated to have ns_i total samples without any holes. """
    return QtCore.QString(self.client.send("Vector::interpolateNoHoles("+self.handle+","+b2str(in_i)+","+b2str(ns_i)+")")).toDouble()[0]
  def __getitem__(self,index):
    """  Returns element i of this vector. """
    return QtCore.QString(self.client.send("Vector::value("+self.handle+","+b2str(index)+")")).toDouble()[0]
  def value(self,index):
    """  Returns element i of this vector. """
    return QtCore.QString(self.client.send("Vector::value("+self.handle+","+b2str(index)+")")).toDouble()[0]
  def min_(self) :
    """  Returns the value of the element with the minimum value. """
    return QtCore.QString(self.client.send("Vector::min("+self.handle+")")).toDouble()[0]
  def max_(self) :
    """  Returns the value of the element with the maximum value. """
    return QtCore.QString(self.client.send("Vector::max("+self.handle+")")).toDouble()[0]
  def ns_max(self) :
    """  Returns the value of the element with the maximum value while being insensitive to spikes. """
    return QtCore.QString(self.client.send("Vector::ns_max("+self.handle+")")).toDouble()[0]
  def ns_min(self) :
    """  Returns the value of the element with the minimum value while being insensitive to spikes. """
    return QtCore.QString(self.client.send("Vector::ns_min("+self.handle+")")).toDouble()[0]
  def mean(self) :
    """  Returns the mean of this vector. """
    return QtCore.QString(self.client.send("Vector::mean("+self.handle+")")).toDouble()[0]
  def minPos(self) :
    """  Returns the value of the element with the minimum positive value. """
    return QtCore.QString(self.client.send("Vector::minPos("+self.handle+")")).toDouble()[0]
  def numNew(self) :
    """  Returns the number of new samples in the vector since last newSync. See also newSync()"""
    return QtCore.QString(self.client.send("Vector::numNew("+self.handle+")")).toInt()[0]
  def numShift(self) :
    """  Returns the number of samples shifted in the vector since last newSync. See also newSync()"""
    return QtCore.QString(self.client.send("Vector::numShift("+self.handle+")")).toInt()[0]
  def isRising(self) :
    """  Returns true if a data vector is getting new data. """
    return True if QtCore.QString(self.client.send("Vector::isRising("+self.handle+")"))==True else False
  def newSync(self) :
    """  See numNew() and numShift() """
    QtCore.QString(self.client.send("Vector::newSync("+self.handle+")"))
  def resize(self,sz,init) :
    """ Sets the size of this vector to sz, and, if init is set to True, initializes new values to 0 """
    QtCore.QString(self.client.send("Vector::resize("+self.handle+","+b2str(sz)+","+b2str(init)+")"))
  def setNewAndShift(self,inNew,inShift) :
    """  Sets the value which numNew() and numShift() will return. See also numNew() and numShift(). """
    QtCore.QString(self.client.send("Vector::setNewAndShift("+self.handle+","+b2str(inNew)+","+b2str(inShift)+")"))
  def zero(self) :
    """ Sets all values in the vector to zero. """
    QtCore.QString(self.client.send("Vector::zero("+self.handle+")"))
  def blank(self) :
    """ Sets all values in the vector to NOPOINT (NaN). """
    QtCore.QString(self.client.send("Vector::blank("+self.handle+")")) 
  def getNumPyArray(self) :
    """ Returns a numPy array of the vector. """
    return self.client.getArray(self.handle)
  def changeFrames(self,f0,n,skip,in_doSkip,in_doAve) :
    """ For DataVectors, sets the start and ending frame as well as skipping parameters.
    
    f0 is the starting frame, n is the number of frames, skip is the number of samples to read per frame, in_doSkip should be set to true if you want to actually skip samples. in_doAve represents whether the box filter should be applied."""
    return self.client.send("DataVector::changeFrames("+self.handle+","+b2str(f0)+","+b2str(n)+","+b2str(skip)+","+b2str(in_doSkip)+","+b2str(in_doAve)+")")
  def numFrames(self) :
    """ For DataVectors, returns the frame count of the data source.
    
    .. note::
       length() is the number of samples in this vector. numFrames() is the number of frames in the data source.
    """
    return self.client.send("DataVector::numFrames("+self.handle+","+")")
  def startFrame(self) :
    """ For DataVectors, returns the start frame from the data source.
    
    .. note::
       This applies to the data source. The actual vector starts at frame 0.
    """
    return self.client.send("DataVector::startFrame("+self.handle+")")
  def doSkip(self) :
    """ For DataVectors, whether or not samples are skipped.
    """
    return True if self.client.send("DataVector::doSkip("+self.handle+")")=="true" else False
  def doAve(self) :
    """ For DataVectors, whether or not the box filter is applied. """
    return True if self.client.send("DataVector::doAvg("+self.handle+")")=="true" else False
  def skip(self) :
    """ For DataVectors, if doSkip()==True, one sample is read every skip frames, else undefined. """
    return int(self.client.send("DataVector::skip("+self.handle+")"))
  def reload_(self) :
    """ For DataVectors, reloads. """
    self.client.send("DataVector::reload("+self.handle+")")
  def samplesPerFrame(self) :
    """ For DataVectors, number of samples contained per frame in the data source. (Not) to be confused with skip(), which is very different. """
    return int(self.client.send("DataVector::samplesPerFrame("+self.handle+")"))
  def fileLength(self) :
    """ For DataVectors, . . . """
    return int(self.client.send("DataVector::fileLength("+self.handle+")"))
  def readToEOF(self) :
    """ For DataVectors, returns True if read to end, else False. """
    return True if self.client.send("DataVector::readToEOF("+self.handle+")") else False
  def countFromEOF(self) :
    """ For DataVectors, returns True if read from end, else False. """
    return True if self.client.send("DataVector::readFromEOF("+self.handle+")") else False
  def descriptionTip(self) : 
    """ For DataVectors, returns lots of really awesome information. But only for DataVectors! """
    return str(self.client.send("DataVector::descriptionTip("+self.handle+")"))
  def isValid(self):
    """ For DataVectors, returns true if valid, else false. """
    return True if self.client.send("DataVector::isValid("+self.handle+")") else False
  
class DataVector(Vector):
  """ This class represents a vector you would create via "Create>Vector>Read from Data Source" from the menubar inside kst.
  
  TODO: implement the configure widget and implement edit functions. 
  
  The parameters of this function mirror the parameters within "Create>Vector>Read from Data Source". 
  
  start is start, or if from end, -1. drange is range, or if to end, -1. start and drange cannot both be -1, obviously.
  
  skip is False for no skip or the number of samples to read per frame.
  
  To create a vector from '/foo.bar' with field 'foo' from index 3 to index 10 skipping every other sample without a boxcar filter::
  
    import pykst as kst
    client = kst.Client()
    v = kst.DataVector(client,"/foo.bar","foo",False,True,False,3,10,2,False) """
  def __init__(self,client,filename,field="INDEX",changeDetection=False,timeInterval=True,dontUpdate=False,start=0,drange=-1,skip=False,boxcarFirst=False,name="") : 
    Vector.__init__(self,client)
    self.handle=QtCore.QString(self.client.send("#newVectorFromDataSource("+b2str(filename)+","+b2str(field)+","+b2str(changeDetection)+","+b2str(timeInterval)+","+b2str(dontUpdate)+","+b2str(start)+","+b2str(drange)+","+b2str(skip)+","+b2str(boxcarFirst)+","+b2str(name)+")"))
    self.handle.remove(0,self.handle.indexOf("ing ")+4)
    
class GeneratedVector(Vector) :
  """ This class represents a vector you would create via "Create>Vector>Generate" from the menubar inside kst.
  
  The parameters of this function mirror the parameters within "Create>Vector>Generate".
  
  TODO: implement edit functions."""
  def __init__(self,client,sfrom,to,samples,name="") :
    Vector.__init__(self,client)
    self.handle=QtCore.QString(self.client.send("#newVectorGenerate("+b2str(sfrom)+","+b2str(to)+","+b2str(samples)+","+b2str(name)+")"))
    self.handle.remove(0,self.handle.indexOf("ing ")+4)

class EditableVector(Vector) :
  """ Use this class to insert a vector into kst. Editable vectors can not be created by using the kst gui.
        
  TODO: implement names
  
  
  To import a numPy array of the first 20 powers of 2 into kst::
  
    import pykst as kst
    from numpy import *
    a=array([])
    a.resize(20)
    
    i=1
    for x in a:
      x = i
      i*=2
      
    client=kst.Client()
    v=kst.EditableVector(client)
    v.setFromList(a)"""
  def __init__(self,client) :
    Vector.__init__(self,client)
    self.handle=QtCore.QString(self.client.send("newEditableVectorAndGetHandle()"))
    self.handle.remove(0,self.handle.indexOf("ing ")+4)
    
  def setFromList(self,arr):
    """ Imports a numPy array into kst."""
    set_arr(arr,self.client.serverName,self.handle)
    return

class ExistingVector(Vector) :
  """ This class allows access to a non-editable vector created inside kst or through a script given a descriptive or short name.
  
  "handle" is a descriptive or short name of a vector created inside kst or through a script. """
  def __init__(self,client,handle) :
    Vector.__init__(self,client)
    self.handle=handle
    
  @classmethod
  def getList(cls,client):
    """ Returns a python list of all vectors. """
    x=QtCore.QString(client.send("getVectorList()"))
    ret=[]
    while x.contains('['):
      y=QtCore.QString(x)
      y.remove(0,1)
      y.remove(y.indexOf(']'),9999999)
      x.remove(0,x.indexOf(']')+1)
      ret.append(ExistingVector(client,y))
    return ret





class Matrix(NamedObject) :
  """ This is a class which some convenience classes within pykst use. You should not use it directly. """
  def __init__(self,client) :
    NamedObject.__init__(self,client)

  def getNumPyArray(self) :
    """ Returns a numPy array of the matrix. """
    return self.client.getArray2D(self.handle)

  def setXMin(self,x0=0) :
    """ set the left edge of the map defined by the matrix """
    self.client.send_si(self.handle, b2str("setXMinimum("+b2str(x0)+")"))

  def getXMin(self) :
    """ get the left edge of the map defined by the matrix """
    return self.client.send_si(self.handle, b2str("getXMinimum()"))

  def setYMin(self,y0=0) :
    """ set the bottom edge of the map defined by the matrix """
    self.client.send_si(self.handle, b2str("setYMinimum("+b2str(y0)+")"))

  def getYMin(self) :
    """ get the bottom edge of the map defined by the matrix """
    return self.client.send_si(self.handle, b2str("getYMinimum()"))

  def setXStep(self,dx=0) :
    """ set the x step size of the map defined by the matrix """
    self.client.send_si(self.handle, b2str("setXStepSize("+b2str(dx)+")"))

  def getXStep(self) :
    """ get the x step size of the map defined by the matrix """
    return self.client.send_si(self.handle, b2str("getXStepSize()"))

  def setYStep(self,dy=0) :
    """ set the y step size of the map defined by the matrix """
    self.client.send_si(self.handle, b2str("setYStepSize("+b2str(dy)+")"))

  def getYStep(self) :
    """ get the y step size of the map defined by the matrix """
    return self.client.send_si(self.handle, b2str("getYStepSize()"))

class DataMatrix(Matrix) :
  """ This class represents a matrix you would create via "Create>Matrix" from the menubar inside kst.
  
  TODO: edit functions, configure
      
  The parameters of this function mirror the parameters within "Create>Matrix". """
  def __init__(self,client,filename,field,xstart,ystart,xnframe,ynframe,skip=False,boxcarFirst=False,xmin=0,ymin=0,xstep=1,ystep=1,name="") :
    Matrix.__init__(self,client)
    self.handle=QtCore.QString(self.client.send("#newMatrix("+b2str(filename)+","+b2str(field)+","+b2str(xstart)+","+b2str(ystart)+","+b2str(xnframe)+","+b2str(ynframe)+","+b2str(skip)+","+b2str(boxcarFirst)+","+b2str(xmin)+","+b2str(ymin)+","+b2str(xstep)+","+b2str(ystep)+","+b2str(name)+")"))
    self.handle.remove(0,self.handle.indexOf("ing ")+4)

class ExistingMatrix(Matrix) :
  """ This class allows access to a matrix created inside kst or through a script given a descriptive or short name.
  
  "handle" is a descriptive or short name of a vector created inside kst or through a script. """
  def __init__(self,client,handle) :
    Matrix.__init__(self,client)
    self.handle=handle
  
  @classmethod
  def getList(cls,client):
    """ Returns a python list of all matracies. """
    x=QtCore.QString(client.send("getMatrixList()"))
    ret=[]
    while x.contains('['):
      y=QtCore.QString(x)
      y.remove(0,1)
      y.remove(y.indexOf(']'),99999999)
      x.remove(0,x.indexOf(']')+1)
      ret.append(ExistingMatrix(client,y))
    return ret

class EditableMatrix(Matrix) :
  """ Use this class to insert a matrix into kst. Editable vectors can not be created by using the kst gui. """

  def __init__(self,client) :
    Matrix.__init__(self,client)
    self.handle=QtCore.QString(self.client.send("newEditableMatrixAndGetHandle()"))
    self.handle.remove(0,self.handle.indexOf("ing ")+4)
    
  def setFromList(self,arr):
    """ Imports a numpy 2d array into kst."""
    set_matrix(arr,arr.shape[0],arr.shape[1],self.client.serverName,self.handle)
    return


class ExistingString(String) :
  """ This class allows access to a string created inside kst or through a script given a descriptive or short name.
  
  "handle" is a descriptive or short name of a string created inside kst or through a script. """
  def __init__(self,client,handle) :
    String.__init__(self,client)
    self.handle=handle
    
  @classmethod
  def getList(cls,client):
    x=QtCore.QString(client.send("getStringList()"))
    ret=[]
    while x.contains('['):
      y=QtCore.QString(x)
      y.remove(0,1)
      y.remove(y.indexOf(']'),9999999)
      x.remove(0,x.indexOf(']')+1)
      ret.append(ExistingString(client,y))
    return ret
  




class Curve(NamedObject) :
  """ This is a class which some convenience classes within pykst use. You should not use it directly.
  
  TODO: edit functions..."""
  def __init__(self,client) :
    NamedObject.__init__(self,client)
    

class NewCurve(Curve) :
  """ This class represents a string you would create via "Create>Curve" from the menubar inside kst.
  The parameters of this function mirror the parameters within "Create>Curve".
    
  Colors are given by a name such as 'red' or a hex number such as '#FF0000'.
    
  curvelinetype is the index of a pen style where 0 is SolidLine, 1 is DashLine, 2 is DotLine, 3 is DashDotLine, and 4 isDashDotDotLine,
    
  pointtype is the index of a point style. 0 is an X, 1 is an open square, 2 is an open circle, 3 is a filled circle,
  4 is a downward open triangle, 5 is an upward open triangle, 6 is a filled square, 7 is a plus, 8 is an asterisk,
  9 is a downward filled triangle, 10 is an upward filled triangle, 11 is an open diamond, and 12 is a filled diamond.
    
  headtype is the index of a point style. See details for pointtype.
    
  To place in an existing plot, set placeinexistingplot = plot
  
  To prevent a the curve from being placed in any plot, set donotplaceinanyplot=True.  The default is to place the curve in a new plot.
    
  Not specifying a parameter implies it's default value (i.e., the setting used on the previous curve whether through a script or
  by the GUI)."""
  def __init__(self,client,xaxis,yaxis,plusxerrorbar=0,plusyerrorbar=0,minusxerrorbar=0,minusyerrorbar=0,usexplusforminus=True,useyplusforminus=True,curvecolor="",curvelinetype="",curveweight="",uselines=True,usepoints=False,pointtype="",pointdensity="",usehead=False,headtype="",headcolor="",usebargraph=False,bargraphfill="",ignoreinauto="",donotplaceinanyplot=False,placeinexistingplot=0,placeinnewplot=True,placeinnewtab=False,scalefonts=True,autolayout=True,customgridcolumns=False,protectexistinglayout=False,name="") :
    Curve.__init__(self,client)
    if placeinexistingplot != 0:
      donotplaceinanyplot=False
      placeinnewplot=False
      placeinnewtab=False
      existingplothandle = placeinexistingplot.handle
    else:
      existingplothandle = " "
      
    if donotplaceinanyplot == True:
      placeinnewplot=False
      placeinnewtab=False
      existingplothandle = " "

    if plusxerrorbar==0:
      plusxerrorbarhandle = ""
    else:
      plusxerrorbarhandle = plusxerrorbar.handle
    if minusxerrorbar==0:
      minusxerrorbarhandle = ""
    else:
      minusxerrorbarhandle = minusxerrorbar.handle
    if plusyerrorbar==0:
      plusyerrorbarhandle = ""
    else:
      plusyerrorbarhandle = plusyerrorbar.handle
    if minusyerrorbar==0:
      minusyerrorbarhandle = ""
    else:
      minusyerrorbarhandle = minusyerrorbar.handle

    self.handle=QtCore.QString(self.client.send("#newCurve("+b2str(xaxis.handle)+","+b2str(yaxis.handle)+","+b2str(plusxerrorbarhandle)+","+b2str(plusyerrorbarhandle)+","+b2str(minusxerrorbarhandle)+","+b2str(minusyerrorbarhandle)+","+b2str(usexplusforminus)+","+b2str(useyplusforminus)+")"))
    self.client.send("#setCurveParameters("+b2str(curvecolor)+","+b2str(curvelinetype)+","+b2str(curveweight)+","+b2str(uselines)+","+b2str(usepoints)+","+b2str(pointtype)+","+b2str(pointdensity)+","+b2str(usehead)+","+b2str(headtype)+","+b2str(headcolor)+","+b2str(usebargraph)+","+b2str(bargraphfill)+","+b2str(ignoreinauto)+","+b2str(donotplaceinanyplot)+","+b2str(existingplothandle)+","+b2str(placeinnewplot)+","+b2str(placeinnewtab)+","+b2str(scalefonts)+","+b2str(autolayout)+","+b2str(customgridcolumns)+","+b2str(protectexistinglayout)+","+b2str(name)+")")
    self.handle.remove(0,self.handle.indexOf("ing ")+4)


class ExistingCurve(Curve) :
  """ This class allows access to a curve created inside kst or through a script given a descriptive or short name.
  
  "handle" is a descriptive or short name of a curve created inside kst or through a script. """
  def __init__(self,client,handle) :
    Curve.__init__(self,client)
    self.handle=handle
    
  @classmethod
  def getList(cls,client):
    x=QtCore.QString(client.send("getCurveList()"))
    ret=[]
    while x.contains('['):
      y=QtCore.QString(x)
      y.remove(0,1)
      y.remove(y.indexOf(']'),9999999)
      x.remove(0,x.indexOf(']')+1)
      ret.append(ExistingCurve(client,y))
    return ret


# EQUATIONS ###################################################################
class Equation(NamedObject) :
  """ This is the base class for Kst Equations.  To create a new equation use pykst.NewEquation() or
  to access an existing equation use pykst.ExistingEquation()

  For example, to plot f(x)=x^2 with x in range(-100,100) with 1000000 samples: ::
  
    import pykst as kst
    client = kst.Client()
    x=kst.GeneratedVector(client,-100,100,1000000)
    GYEquation=kst.NewEquation(client, "x^2", x)
    GYEqCurve = kst.NewCurve(client,GYEquation.X(), GYEquation.Y(), curvecolor="black", 
                curveweight=1, placeinnewplot=True)
  
  TODO: edit functions..."""
  def __init__(self,client) :
    NamedObject.__init__(self,client)

  def Y(self) :
    """ Returns a vector containing the output of the equation (ie, f(x)) """
    YHandle = QtCore.QString(self.client.send("DataObject::outputVectorHandle("+self.handle+", O)"))
    return ExistingVector(self.client, YHandle)

  def X(self) :
    """ Returns the vector which has been used as the independent variable of the equation """
    XHandle = QtCore.QString(self.client.send("DataObject::outputVectorHandle("+self.handle+", XO)"))
    return ExistingVector(self.client, XHandle)
    
class NewEquation(Equation) :
  """ This class represents an equation you would create via "Create>Equation" from the menubar inside kst.
  """
        
  
  def __init__(self,client,equation,xvector,interploate=False,name="") :
    Equation.__init__(self,client)
 
    self.handle=QtCore.QString(self.client.send("#newEquation("+b2str(equation)+","+b2str(xvector.handle)+","+b2str(interploate)+")"))
    self.handle.remove(0,self.handle.indexOf("ing ")+4)
  
class ExistingEquation(Equation) :
  """ This class allows access to an equation created inside kst or through a script given a descriptive or short name.
  "handle" is a descriptive or short name of an equation created inside kst or through a script. """
  def __init__(self,client,handle) :
    Equation.__init__(self,client)
    self.handle=handle
    
  @classmethod
  def getList(cls,client):
    x=QtCore.QString(client.send("getEquationList()"))
    ret=[]
    while x.contains('['):
      y=QtCore.QString(x)
      y.remove(0,1)
      y.remove(y.indexOf(']'),9999999)
      x.remove(0,x.indexOf(']')+1)
      ret.append(ExistingEquation(client,y))
    return ret


# FIT ###################################################################
class Fit(NamedObject) :
  """ This is a class which provides some methods common to all fits """
  def __init__(self,client) :
    NamedObject.__init__(self,client)
    
  def Fit(self) :
    """ a vector containing the fit  """
    YHandle = QtCore.QString(self.client.send("DataObject::outputVectorHandle("+self.handle+", Fit)"))
    return ExistingVector(self.client, YHandle)

  def Residuals(self) :
    """ a vector containing the Residuals  """
    YHandle = QtCore.QString(self.client.send("DataObject::outputVectorHandle("+self.handle+", Residuals)"))
    return ExistingVector(self.client, YHandle)

  def Parameters(self) :
    """ a vector containing the Parameters of the fit  """
    YHandle = QtCore.QString(self.client.send("DataObject::outputVectorHandle("+self.handle+", Parameters Vector)"))
    return ExistingVector(self.client, YHandle)

  def Covariance(self) :
    """ a vector containing the Covariance of the fit parameters """
    YHandle = QtCore.QString(self.client.send("DataObject::outputVectorHandle("+self.handle+", Covariance)"))
    return ExistingVector(self.client, YHandle)

  def ReducedChi2(self) :
    """ a scalar containing the reduced chi2 of the fit """
    YHandle = QtCore.QString(self.client.send("DataObject::outputScalarHandle("+self.handle+", chi^2/nu)"))
    return ExistingScalar(self.client, YHandle)


# LINEAR FIT #################################################################
class LinearFit(Fit) :
  """ This is a class which some convenience classes within pykst use. You should not use it directly.
  
  TODO: edit functions..."""
  def __init__(self,client) :
    NamedObject.__init__(self,client)

  def Low(self) :
    """ a vector containing the 1 sigma lower limit of the fit  """
    YHandle = QtCore.QString(self.client.send("DataObject::outputVectorHandle("+self.handle+", Lo Vector)"))
    return ExistingVector(self.client, YHandle)

  def High(self) :
    """ a vector containing the 1 sigma upper limit of the fit  """
    YHandle = QtCore.QString(self.client.send("DataObject::outputVectorHandle("+self.handle+", Hi Vector)"))
    return ExistingVector(self.client, YHandle)
    
class NewLinearFit(LinearFit) :
  """ This class represents a fit you would create via "Create>Plugin->Linear Fit" from the menubar inside kst or by using
  "rmb->fit->[curvename]", and then selecting "Linear Fit" in the plugin combo.  If the weightvector is listed, then it will be a weighted fit.  
  Otherwise it will be unweighted. 
  
  NewLinearFit() and ExistingLinearFit() have the same methods."""
  
  def __init__(self,client,xvector,yvector,weightvector=0,name="") :
    LinearFit.__init__(self,client)
    
    if weightvector==0:
      QtCore.QString(self.client.send("newPlugin(Linear Fit)"))
    else:
      QtCore.QString(self.client.send("newPlugin(Linear Weighted Fit)"))
      QtCore.QString(self.client.send("setInputVector(Weights Vector,"+weightvector.handle+")"))
      
    QtCore.QString(self.client.send("setInputVector(X Vector,"+xvector.handle+")"))
    QtCore.QString(self.client.send("setInputVector(Y Vector,"+yvector.handle+")"))
    self.handle=QtCore.QString(self.client.send("endEdit()"))
    self.handle.remove(0,self.handle.indexOf("ing ")+4)
  
class ExistingLinearFit(LinearFit) :
  """ This class allows access to an Linear Fit created inside kst or through a script given a descriptive or short name.
  
  "handle" is a descriptive or short name of a Linear Fit created inside kst or through a script. """
  def __init__(self,client,handle) :
    LinearFit.__init__(self,client)
    self.handle=handle

# POLYNOMIAL FIT #################################################################
class PolynomialFit(Fit) :
  """ This is a class which some convenience classes within pykst use. You should not use it directly.
  
  TODO: edit functions..."""
  def __init__(self,client) :
    NamedObject.__init__(self,client)

class NewPolynomialFit(PolynomialFit) :
  """ This class represents a fit you would create via "Create>Plugin->Polynomial Fit" from the menubar inside kst or by using
  "rmb->fit->[curvename]", and then selecting "Polynomial Fit" in the plugin combo. "order" can be either a Scalar or a number. 
  If the weightvector is listed, then it will be a weighted fit.  Otherwise it will be unweighted. 
  
  NewPolynomialFit() and ExistingPolynomialFit() have the same methods."""
  
  def __init__(self,client,xvector,yvector,order_in,name="", weightvector=0) :
    PolynomialFit.__init__(self,client)
    
    if isinstance(order_in, Scalar):
      order = order_in
    else :
      order = GeneratedScalar(client, order_in);

    if weightvector==0:
      QtCore.QString(self.client.send("newPlugin(Polynomial Fit)"))
    else:
      QtCore.QString(self.client.send("newPlugin(Polynomial Weighted Fit)"))
      QtCore.QString(self.client.send("setInputVector(Weights Vector,"+weightvector.handle+")"))
      
    QtCore.QString(self.client.send("setInputVector(X Vector,"+xvector.handle+")"))
    QtCore.QString(self.client.send("setInputVector(Y Vector,"+yvector.handle+")"))
    QtCore.QString(self.client.send("setInputScalar(Order Scalar,"+order.handle+")"))
    self.handle=QtCore.QString(self.client.send("endEdit()"))
    self.handle.remove(0,self.handle.indexOf("ing ")+4)
  
class ExistingPolynomialFit(PolynomialFit) :
  """ This class allows access to an Polynomial Fit created inside kst or through a script given a descriptive or short name.
  
  "handle" is a descriptive or short name of a Polynomial Fit created inside kst or through a script. """
  def __init__(self,client,handle) :
    PolynomialFit.__init__(self,client)
    self.handle=handle

# SINUSOID FIT #################################################################
class SinusoidFit(Fit) :
  """ This is a class which some convenience classes within pykst use. You should not use it directly.
  
  TODO: edit functions..."""
  def __init__(self,client) :
    NamedObject.__init__(self,client)

class NewSinusoidFit(SinusoidFit) :
  """ This class represents a fit you would create via "Create>Plugin->Sinusoid Fit" from the menubar inside kst or by using
  "rmb->fit->[curvename]", and then selecting "Sinusoid Fit" in the plugin combo. Period and Harmonics can be either a Scalar or a number. 
  If the weightvector is listed, then it will be a weighted fit.  Otherwise it will be unweighted. 
  
  NewSinusoidFit() and ExistingSinusoidFit() have the same methods."""
  
  def __init__(self,client,xvector,yvector,period_in, harmonics_in, weightvector=0, name = "") :
    SinusoidFit.__init__(self,client)
    
    if isinstance(period_in, Scalar):
      period = period_in
    else :
      period = GeneratedScalar(client, period_in);

    if isinstance(harmonics_in, Scalar):
      harmonics = harmonics_in;
    else :
      harmonics = GeneratedScalar(client, harmonics_in);

    if weightvector==0:
      QtCore.QString(self.client.send("newPlugin(Sinusoid Fit)"))
    else:
      QtCore.QString(self.client.send("newPlugin(Sinusoid Weighted Fit)"))
      QtCore.QString(self.client.send("setInputVector(Weights Vector,"+weightvector.handle+")"))
      
    QtCore.QString(self.client.send("setInputVector(X Vector,"+xvector.handle+")"))
    QtCore.QString(self.client.send("setInputVector(Y Vector,"+yvector.handle+")"))
    QtCore.QString(self.client.send("setInputScalar(Period Scalar,"+period.handle+")"))
    QtCore.QString(self.client.send("setInputScalar(Harmonics Scalar,"+harmonics.handle+")"))
    self.handle=QtCore.QString(self.client.send("endEdit()"))
    self.handle.remove(0,self.handle.indexOf("ing ")+4)
  
class ExistingSinusoidFit(SinusoidFit) :
  """ This class allows access to an Sinusoid Fit created inside kst or through a script given a descriptive or short name.
  
  "handle" is a descriptive or short name of a Sinusoid Fit created inside kst or through a script. """
  def __init__(self,client,handle) :
    SinusoidFit.__init__(self,client)
    self.handle=handle

# LORENTZIAN FIT #################################################################
class LorentzianFit(Fit) :
  """ This is a class which some convenience classes within pykst use. You should not use it directly.
  
  TODO: edit functions..."""
  def __init__(self,client) :
    NamedObject.__init__(self,client)

class NewLorentzianFit(LorentzianFit) :
  """ This class represents a linear fit you would create via "Create>Plugin->Lorentzian Fit" from the menubar inside kst or by using
  "rmb->fit->[curvename], and then selecting "Lorentzian Fit" in the plugin combo.  
  
  NewLorentzianFit() and ExistingLorentzianFit() have the same methods."""
  
  def __init__(self,client,xvector,yvector,weightvector=0,name="") :
    LorentzianFit.__init__(self,client)
    
    if weightvector==0:
      QtCore.QString(self.client.send("newPlugin(Lorentzian Fit)"))
    else:
      QtCore.QString(self.client.send("newPlugin(Lorentzian Weighted Fit)"))
      QtCore.QString(self.client.send("setInputVector(Weights Vector,"+weightvector.handle+")"))

    QtCore.QString(self.client.send("setInputVector(X Vector,"+xvector.handle+")"))
    QtCore.QString(self.client.send("setInputVector(Y Vector,"+yvector.handle+")"))
    self.handle=QtCore.QString(self.client.send("endEdit()"))
    self.handle.remove(0,self.handle.indexOf("ing ")+4)
  
class ExistingLorentzianFit(LorentzianFit) :
  """ This class allows access to an Lorentzian Fit created inside kst or through a script given a descriptive or short name.
  
  "handle" is a descriptive or short name of a Lorentzian Fit created inside kst or through a script. """
  def __init__(self,client,handle) :
    LorentzianFit.__init__(self,client)
    self.handle=handle

# GAUSSIAN FIT #################################################################
class GaussianFit(Fit) :
  """ This is a class which some convenience classes within pykst use. You should not use it directly.
  
  TODO: edit functions..."""
  def __init__(self,client) :
    NamedObject.__init__(self,client)

class NewGaussianFit(GaussianFit) :
  """ This class represents a fit you would create via "Create>Plugin->Gaussian Fit" from the menubar inside kst or by using
  "rmb->fit->[curvename], and then selecting "Gaussian Fit" in the plugin combo.  
  
  NewGaussianFit() and ExistingGaussianFit() have the same methods."""
  
  def __init__(self,client,xvector,yvector,weightvector=0,name="") :
    GaussianFit.__init__(self,client)
    
    if weightvector==0:
      QtCore.QString(self.client.send("newPlugin(Gaussian Fit)"))
    else:
      QtCore.QString(self.client.send("newPlugin(Gaussian Weighted Fit)"))
      QtCore.QString(self.client.send("setInputVector(Weights Vector,"+weightvector.handle+")"))

    QtCore.QString(self.client.send("setInputVector(X Vector,"+xvector.handle+")"))
    QtCore.QString(self.client.send("setInputVector(Y Vector,"+yvector.handle+")"))
    self.handle=QtCore.QString(self.client.send("endEdit()"))
    self.handle.remove(0,self.handle.indexOf("ing ")+4)
  
class ExistingGaussianFit(GaussianFit) :
  """ This class allows access to an Gaussian Fit created inside kst or through a script given a descriptive or short name.
  
  "handle" is a descriptive or short name of a Gaussian Fit created inside kst or through a script. """
  def __init__(self,client,handle) :
    GaussianFit.__init__(self,client)
    self.handle=handle

# EXPONENTIAL FIT #################################################################
class ExponentialFit(Fit) :
  """ This is a class which some convenience classes within pykst use. You should not use it directly.
  
  TODO: edit functions..."""
  def __init__(self,client) :
    NamedObject.__init__(self,client)
    
class NewExponentialFit(ExponentialFit) :
  """ This class represents an exponential fit you would create via "Create>Plugin->Exponential Fit" from the menubar inside kst or by using
  "rmb->fit->[curvename], and then selecting "Exponential Fit" in the plugin combo.  
  
  NewExponentialFit() and ExistingExponentialFit() have the same methods."""
  
  def __init__(self,client,xvector,yvector,weightvector=0,name="") :
    ExponentialFit.__init__(self,client)
    
    if weightvector==0:
      QtCore.QString(self.client.send("newPlugin(Exponential Fit)"))
    else:
      QtCore.QString(self.client.send("newPlugin(Exponential Weighted Fit)"))
      QtCore.QString(self.client.send("setInputVector(Weights Vector,"+weightvector.handle+")"))
      
    QtCore.QString(self.client.send("setInputVector(X Vector,"+xvector.handle+")"))
    QtCore.QString(self.client.send("setInputVector(Y Vector,"+yvector.handle+")"))
    self.handle=QtCore.QString(self.client.send("endEdit()"))
    self.handle.remove(0,self.handle.indexOf("ing ")+4)
  
class ExistingExponentialFit(ExponentialFit) :
  """ This class allows access to an Exponential Fit created inside kst or through a script given a descriptive or short name.
  
  "handle" is a descriptive or short name of a Exponential Fit created inside kst or through a script. """
  def __init__(self,client,handle) :
    ExponentialFit.__init__(self,client)
    self.handle=handle

# FILTER ###################################################################
class Filter(NamedObject) :
  """ This is a class which provides some methods common to many filters """
  def __init__(self,client) :
    NamedObject.__init__(self,client)
    
  def Y(self) :
    """ a vector containing the filtered output  """
    YHandle = QtCore.QString(self.client.send("DataObject::outputVectorHandle("+self.handle+", Y Vector)"))
    return ExistingVector(self.client, YHandle)


# LOW PASS FILTER #################################################################
class LowPassFilter(Filter) :
  """ This is a class which some convenience classes within pykst use. You should not use it directly.
  
  TODO: edit functions..."""
  def __init__(self,client) :
    NamedObject.__init__(self,client)
    
class NewLowPassFilter(LowPassFilter) :
  """ This class represents a filter you would create via "Create>Filter Plugin->Low Pass Filter" from the menubar inside kst or by using
  "rmb->filter->[curvename], and then selecting "Low Pas Filter" in the plugin combo. The parameters of this function mirror the parameters within
  the latter dialog.

  NewLowPassFilter() and ExistingLowPassFilter() have the same methods."""
  
  def __init__(self,client,yvector,order_in, cutoff_in,name="") :
    LowPassFilter.__init__(self,client)

    if isinstance(order_in, Scalar):
      order = order_in
    else :
      order = GeneratedScalar(client, order_in);

    if isinstance(cutoff_in, Scalar):
      cutoff = cutoff_in
    else :
      cutoff = GeneratedScalar(client, cutoff_in);
    
    QtCore.QString(self.client.send("newPlugin(Low Pass Filter)"))
    QtCore.QString(self.client.send("setInputVector(Y Vector,"+yvector.handle+")"))
    QtCore.QString(self.client.send("setInputScalar(Order Scalar,"+order.handle+")"))
    QtCore.QString(self.client.send("setInputScalar(Cutoff / Spacing Scalar,"+cutoff.handle+")"))
    
    self.handle=QtCore.QString(self.client.send("endEdit()"))
    self.handle.remove(0,self.handle.indexOf("ing ")+4)
  
class ExistingLowPassFilter(LowPassFilter) :
  """ This class allows access to an Linear Fit created inside kst or through a script given a descriptive or short name.
  
  "handle" is a descriptive or short name of a LowPassFilter created inside kst or through a script. """
  def __init__(self,client,handle) :
    LowPassFilter.__init__(self,client)
    self.handle=handle

# HIGH PASS FILTER #################################################################
class HighPassFilter(Filter) :
  """ This is a class which some convenience classes within pykst use. You should not use it directly.
  
  TODO: edit functions..."""
  def __init__(self,client) :
    NamedObject.__init__(self,client)
    
class NewHighPassFilter(HighPassFilter) :
  """ This class represents a filter you would create via "Create>Filter Plugin->High Pass Filter" from the menubar inside kst or by using
  "rmb->filter->[curvename], and then selecting "High Pas Filter" in the plugin combo. The parameters of this function mirror the parameters within
  the latter dialog.

  NewHighPassFilter() and ExistingHighPassFilter() have the same methods."""
  
  def __init__(self,client,yvector,order_in, cutoff_in,name="") :
    HighPassFilter.__init__(self,client)

    if isinstance(order_in, Scalar):
      order = order_in
    else :
      order = GeneratedScalar(client, order_in);

    if isinstance(cutoff_in, Scalar):
      cutoff = cutoff_in
    else :
      cutoff = GeneratedScalar(client, cutoff_in);
    
    QtCore.QString(self.client.send("newPlugin(High Pass Filter)"))
    QtCore.QString(self.client.send("setInputVector(Y Vector,"+yvector.handle+")"))
    QtCore.QString(self.client.send("setInputScalar(Order Scalar,"+order.handle+")"))
    QtCore.QString(self.client.send("setInputScalar(Cutoff / Spacing Scalar,"+cutoff.handle+")"))
    
    self.handle=QtCore.QString(self.client.send("endEdit()"))
    self.handle.remove(0,self.handle.indexOf("ing ")+4)
  
class ExistingHighPassFilter(HighPassFilter) :
  """ This class allows access to an HighPassFilter created inside kst or through a script given a descriptive or short name.
  
  "handle" is a descriptive or short name of a HighPassFilter created inside kst or through a script. """
  
  def __init__(self,client,handle) :
    HighPassFilter.__init__(self,client)
    self.handle=handle

# BAND PASS FILTER #################################################################
class BandPassFilter(Filter) :
  """ This is a class which some convenience classes within pykst use. You should not use it directly.
  
  TODO: edit functions..."""
  def __init__(self,client) :
    NamedObject.__init__(self,client)
    
class NewBandPassFilter(BandPassFilter) :
  """ This class represents a filter you would create via "Create>Filter Plugin->Band Pass Filter" from the menubar inside kst or by using
  "rmb->filter->[curvename], and then selecting "Band Pas Filter" in the plugin combo. The parameters of this function mirror the parameters within
  the latter dialog.

  NewBandPassFilter() and ExistingBandPassFilter() have the same methods."""
  
  def __init__(self,client,yvector,order_in, central_in, bandwidth_in,name="") :
    BandPassFilter.__init__(self,client)

    if isinstance(order_in, Scalar):
      order = order_in
    else :
      order = GeneratedScalar(client, order_in);

    if isinstance(central_in, Scalar):
      central = central_in
    else :
      central = GeneratedScalar(client, central_in);
    
    if isinstance(bandwidth_in, Scalar):
      bandwidth = bandwidth_in
    else :
      bandwidth = GeneratedScalar(client, bandwidth_in);

    QtCore.QString(self.client.send("newPlugin(Band Pass Filter)"))
    QtCore.QString(self.client.send("setInputVector(Y Vector,"+yvector.handle+")"))
    QtCore.QString(self.client.send("setInputScalar(Order Scalar,"+order.handle+")"))
    QtCore.QString(self.client.send("setInputScalar(Central Frequency / Sample Rate Scalar,"+central.handle+")"))
    QtCore.QString(self.client.send("setInputScalar(Band width Scalar,"+bandwidth.handle+")"))
    
    self.handle=QtCore.QString(self.client.send("endEdit()"))
    self.handle.remove(0,self.handle.indexOf("ing ")+4)
  
class ExistingBandPassFilter(BandPassFilter) :
  """ This class allows access to a BandPassFilter created inside kst or through a script given a descriptive or short name.
  
  "handle" is a descriptive or short name of a BandPassFilter created inside kst or through a script. """
  def __init__(self,client,handle) :
    BandPassFilter.__init__(self,client)
    self.handle=handle
    
# BAND STOP FILTER #################################################################
class BandStopFilter(Filter) :
  """ This is a class which some convenience classes within pykst use. You should not use it directly.
  
  TODO: edit functions..."""
  def __init__(self,client) :
    NamedObject.__init__(self,client)
    
class NewBandStopFilter(BandStopFilter) :
  """ This class represents a filter you would create via "Create>Filter Plugin->Band Stop Filter" from the menubar inside kst or by using
  "rmb->filter->[curvename], and then selecting "Band Pas Filter" in the plugin combo. The parameters of this function mirror the parameters within
  the latter dialog.

  NewBandStopFilter() and ExistingBandStopFilter() have the same methods."""
  
  def __init__(self,client,yvector,order_in, central_in, bandwidth_in,name="") :
    BandStopFilter.__init__(self,client)

    if isinstance(order_in, Scalar):
      order = order_in
    else :
      order = GeneratedScalar(client, order_in);

    if isinstance(central_in, Scalar):
      central = central_in
    else :
      central = GeneratedScalar(client, central_in);
    
    if isinstance(bandwidth_in, Scalar):
      bandwidth = bandwidth_in
    else :
      bandwidth = GeneratedScalar(client, bandwidth_in);

    QtCore.QString(self.client.send("newPlugin(Band Stop Filter)"))
    QtCore.QString(self.client.send("setInputVector(Y Vector,"+yvector.handle+")"))
    QtCore.QString(self.client.send("setInputScalar(Order Scalar,"+order.handle+")"))
    QtCore.QString(self.client.send("setInputScalar(Central Frequency / Sample Rate Scalar,"+central.handle+")"))
    QtCore.QString(self.client.send("setInputScalar(Band width Scalar,"+bandwidth.handle+")"))
    
    self.handle=QtCore.QString(self.client.send("endEdit()"))
    self.handle.remove(0,self.handle.indexOf("ing ")+4)
  
class ExistingBandStopFilter(BandStopFilter) :
  """ This class allows access to an BandStopFilter created inside kst or through a script given a descriptive or short name.
  
  "handle" is a descriptive or short name of a BandPassFilter created inside kst or through a script. """
  def __init__(self,client,handle) :
    BandStopFilter.__init__(self,client)
    self.handle=handle


# CUMULATIVE SUM #################################################################
class CumulativeSum(NamedObject) :
  """ This is a class which some convenience classes within pykst use. You should not use it directly.
  
  TODO: edit functions..."""
  def __init__(self,client) :
    NamedObject.__init__(self,client)
    
  def Y(self) :
    """ a vector containing the cumulative sum  """
    YHandle = QtCore.QString(self.client.send("DataObject::outputVectorHandle("+self.handle+", sum(Y)dX)"))
    return ExistingVector(self.client, YHandle)

class NewCumulativeSum(CumulativeSum) :
  """ This class represents a filter you would create via "Create>Filter Plugin->Cumulative Sum" from the menubar inside kst or by using
  "rmb->filter->[curvename], and then selecting "Cumulative Sum" in the plugin combo.  A cumulative sum is a a descrete integral.

  NewCumulativeSum() and ExistingCumulativeSum() have the same methods."""
  
  def __init__(self,client,yvector,dx_in,name="") :
    CumulativeSum.__init__(self,client)

    if isinstance(dx_in, Scalar):
      dx = dx_in
    else :
      dx = GeneratedScalar(client, dx_in);

    QtCore.QString(self.client.send("newPlugin(Cumulative Sum)"))
    QtCore.QString(self.client.send("setInputVector(Vector In,"+yvector.handle+")"))
    QtCore.QString(self.client.send("setInputScalar(Scale Scalar,"+dx.handle+")"))
    
    self.handle=QtCore.QString(self.client.send("endEdit()"))
    self.handle.remove(0,self.handle.indexOf("ing ")+4)
  
class ExistingCumulativeSum(CumulativeSum) :
  """ This class allows access to a CumulativeSum created inside kst or through a script given a descriptive or short name.
  
  "handle" is a descriptive or short name of a CumulativeSum created inside kst or through a script. """
  def __init__(self,client,handle) :
    CumulativeSum.__init__(self,client)
    self.handle=handle


# Despike Filter #################################################################
class DespikeFilter(NamedObject) :
  """ This is a class which some convenience classes within pykst use. You should not use it directly.
  
  TODO: edit functions..."""
  def __init__(self,client) :
    NamedObject.__init__(self,client)
    
  def Y(self) :
    """ a vector containing the despiked timestream  """
    YHandle = QtCore.QString(self.client.send("DataObject::outputVectorHandle("+self.handle+", Y)"))
    return ExistingVector(self.client, YHandle)

class NewDespikeFilter(DespikeFilter) :
  """ This class represents a filter you would create via "Create>Filter Plugin->Despike Filter" from the menubar inside kst or by using
  "rmb->filter->[curvename], and then selecting "Despike Filter" in the plugin combo.

  NewDespikeFilter() and ExistingDespikeFilter() have the same methods."""
  
  def __init__(self,client,yvector,nsigma_in, spacing_in,name="") :
    DespikeFilter.__init__(self,client)

    if isinstance(nsigma_in, Scalar):
      nsigma = nsigma_in
    else :
      nsigma = GeneratedScalar(client, nsigma_in);

    if isinstance(spacing_in, Scalar):
      spacing = spacing_in
    else :
      spacing = GeneratedScalar(client, spacing_in);

    QtCore.QString(self.client.send("newPlugin(Despike Filter)"))
    QtCore.QString(self.client.send("setInputVector(Y Vector,"+yvector.handle+")"))
    QtCore.QString(self.client.send("setInputScalar(NSigma Scalar,"+nsigma.handle+")"))
    QtCore.QString(self.client.send("setInputScalar(Spacing Scalar,"+spacing.handle+")"))
    
    self.handle=QtCore.QString(self.client.send("endEdit()"))
    self.handle.remove(0,self.handle.indexOf("ing ")+4)
  
class ExistingDespikeFilter(DespikeFilter) :
  """ This class allows access to a DespikeFilter created inside kst or through a script given a descriptive or short name.
  
  "handle" is a descriptive or short name of a DespikeFilter created inside kst or through a script. """
  def __init__(self,client,handle) :
    DespikeFilter.__init__(self,client)
    self.handle=handle



# DIFFERENTIATION #################################################################
class Differentiation(NamedObject) :
  """ This is a class which some convenience classes within pykst use. You should not use it directly.
  
  TODO: edit functions..."""
  def __init__(self,client) :
    NamedObject.__init__(self,client)
    
  def Y(self) :
    """ a vector containing the Derrivative  """
    YHandle = QtCore.QString(self.client.send("DataObject::outputVectorHandle("+self.handle+", Y')"))
    return ExistingVector(self.client, YHandle)

class NewDifferentiation(Differentiation) :
  """ This class represents a filter you would create via "Create>Filter Plugin->Fixed Step Differentiation" from the menubar inside kst or by using
  "rmb->filter->[curvename], and then selecting "Fixed Step Differentiation" in the plugin combo.

  NewDifferentiation() and ExistingDifferentiation() have the same methods."""
  
  def __init__(self,client,yvector,dx_in, name="") :
    Differentiation.__init__(self,client)

    if isinstance(dx_in, Scalar):
      dx = dx_in
    else :
      dx = GeneratedScalar(client, dx_in);

    QtCore.QString(self.client.send("newPlugin(Fixed Step Differentiation)"))
    QtCore.QString(self.client.send("setInputVector(Vector In,"+yvector.handle+")"))
    QtCore.QString(self.client.send("setInputScalar(Scalar In,"+dx.handle+")"))
    
    self.handle=QtCore.QString(self.client.send("endEdit()"))
    self.handle.remove(0,self.handle.indexOf("ing ")+4)
  
class ExistingDifferentiation(Differentiation) :
  """ This class allows access to an Differentiation filter created inside kst or through a script given a descriptive or short name.
  
  "handle" is a descriptive or short name of a Differentiation filter created inside kst or through a script. """
  def __init__(self,client,handle) :
    Differentiation.__init__(self,client)
    self.handle=handle

# SHIFT #################################################################
class Shift(NamedObject) :
  """ This is a class which some convenience classes within pykst use. You should not use it directly.
  
  TODO: edit functions..."""
  def __init__(self,client) :
    NamedObject.__init__(self,client)
    
  def Y(self) :
    """ a vector containing the shifted vector  """
    YHandle = QtCore.QString(self.client.send("DataObject::outputVectorHandle("+self.handle+", Shifted Vector)"))
    return ExistingVector(self.client, YHandle)

class NewShift(Shift) :
  """ This class represents a filter you would create via "Create>Filter Plugin->Shift" from the menubar inside kst or by using
  "rmb->filter->[curvename], and then selecting "Shift" in the plugin combo.

  NewShift() and ExistingShift() have the same methods."""
  
  def __init__(self,client,yvector,dx_in, name="") :
    Shift.__init__(self,client)

    if isinstance(dx_in, Scalar):
      dx = dx_in
    else :
      dx = GeneratedScalar(client, dx_in);

    QtCore.QString(self.client.send("newPlugin(Shift)"))
    QtCore.QString(self.client.send("setInputVector(Vector In,"+yvector.handle+")"))
    QtCore.QString(self.client.send("setInputScalar(dX,"+dx.handle+")"))
    
    self.handle=QtCore.QString(self.client.send("endEdit()"))
    self.handle.remove(0,self.handle.indexOf("ing ")+4)
  
class ExistingShift(Shift) :
  """ This class allows access to an Shift filter created inside kst or through a script given a descriptive or short name.
  
  "handle" is a descriptive or short name of a Shift filter created inside kst or through a script. """
  def __init__(self,client,handle) :
    Shift.__init__(self,client)
    self.handle=handle


# CUMULATIVE AVERAGE #################################################################
class CumulativeAverage(NamedObject) :
  """ This is a class which some convenience classes within pykst use. You should not use it directly.
  
  TODO: edit functions..."""
  def __init__(self,client) :
    NamedObject.__init__(self,client)

  def Y(self) :
    """ a vector containing the cumulative average  """
    YHandle = QtCore.QString(self.client.send("DataObject::outputVectorHandle("+self.handle+", Avg(Y)"))
    return ExistingVector(self.client, YHandle)
    
class NewCumulativeAverage(CumulativeAverage) :
  """ This class represents a filter you would create via "Create>Filter Plugin->Cumulative Average" from the menubar inside kst or by using
  "rmb->filter->[curvename], and then selecting "CumulativeAverage" in the plugin combo.

  NewCumulativeAverage() and ExistingCumulativeAverage() have the same methods."""
  
  def __init__(self,client,yvector,name="") :
    CumulativeAverage.__init__(self,client)

    QtCore.QString(self.client.send("newPlugin(Cumulative Average)"))
    QtCore.QString(self.client.send("setInputVector(Vector In,"+yvector.handle+")"))
    
    self.handle=QtCore.QString(self.client.send("endEdit()"))
    self.handle.remove(0,self.handle.indexOf("ing ")+4)
  
class ExistingCumulativeAverage(CumulativeAverage) :
  """ This class allows access to Cumulative Average created inside kst or through a script given a descriptive or short name.
  
  "handle" is a descriptive or short name of a Cumulative Average created inside kst or through a script. """
  def __init__(self,client,handle) :
    CumulativeAverage.__init__(self,client)
    self.handle=handle

# IMAGE #################################################################

class Image(NamedObject) :
  """ This is a class which some convenience classes within pykst use. You should not use it directly.
  
      TODO: edit functions... """
  def __init__(self,client) :
    NamedObject.__init__(self,client)
    
class ColorImage(Image) :
  """ This class represents an image you would create via "Create>Image>Color Map" from the menubar inside kst.
  Not to be confused with Pictures, Images are representations of matrices and are placed within plots.
  
  The parameters of this function mirror the parameters within "Create>Image>Color Map".
    
  To place in an existing plot, specify placeinexistingplot = plot.
  
  To prevent a the curve from being placed in any plot, set donotplaceinanyplot=True.  The default is to place the curve in a new plot.
    
  Not specifying a parameter implies it's default value (i.e., the setting used on the previous curve whether through a script or
  by the GUI)."""
  
  def __init__(self,client,matrix,palette,rtAutoThreshold,lower,upper,maxmin,smart,percentile,donotplaceinanyplot=False,placeinexistingplot=0,placeinnewplot=True,placeinnewtab=False,scalefonts=True,autolayout=True,customgridcolumns=False,protectexistinglayout=False,name="") :
    Image.__init__(self,client)
    if placeinexistingplot != 0:
      donotplaceinanyplot=False
      placeinnewplot=False
      placeinnewtab=False
      existingplothandle = placeinexistingplot.handle
    else:
      existingplothandle = " "
      
    if donotplaceinanyplot == True:
      placeinnewplot=False
      placeinnewtab=False
      existingplothandle = " "

    self.handle=QtCore.QString(self.client.send("#newImageFromColor("+b2str(matrix.handle)+","+b2str(palette)+","+b2str(rtAutoThreshold)+","+b2str(lower)+","+b2str(upper)+","+b2str(maxmin)+","+b2str(smart)+","+b2str(percentile)+","+b2str(donotplaceinanyplot)+","+b2str(existingplothandle)+","+b2str(placeinnewplot)+","+b2str(placeinnewtab)+","+b2str(scalefonts)+","+b2str(autolayout)+","+b2str(customgridcolumns)+","+b2str(protectexistinglayout)+","+b2str(name)+")"))
    self.handle.remove(0,self.handle.indexOf("ing ")+4)
    
class ContourImage(Image) :
  """ This class represents an image you would create via "Create>Image>Contour Map" from the menubar inside kst.
  Not to be confused with Pictures, Images are representations of matrices and are placed within plots.
  
  The parameters of this function mirror the parameters within "Create>Image>Contour Map".
    
  To place in an existing plot, specify placeinexistingplot = plot.
  
  To prevent a the curve from being placed in any plot, set donotplaceinanyplot=True.  The default is to place the curve in a new plot.
    
  Not specifying a parameter implies it's default value (i.e., the setting used on the previous curve whether through a script or
  by the GUI)."""
  def __init__(self,client,matrix,levels,color="black",weight="12",variable=False,donotplaceinanyplot=False,placeinexistingplot=0,placeinnewplot=True,placeinnewtab=False,scalefonts=True,autolayout=True,customgridcolumns=False,protectexistinglayout=False,name="") :
    Image.__init__(self,client)
    if placeinexistingplot != 0:
      donotplaceinanyplot=False
      placeinnewplot=False
      placeinnewtab=False
      existingplothandle = placeinexistingplot.handle
    else:
      existingplothandle = " "
      
    if donotplaceinanyplot == True:
      placeinnewplot=False
      placeinnewtab=False
      existingplothandle = " "

    self.handle=QtCore.QString(self.client.send("#newImageFromContourMap("+b2str(matrix.handle)+","+b2str(levels)+","+b2str(color)+","+b2str(weight)+","+b2str(variable)+","+b2str(donotplaceinanyplot)+","+b2str(existingplothandle)+","+b2str(placeinnewplot)+","+b2str(placeinnewtab)+","+b2str(scalefonts)+","+b2str(autolayout)+","+b2str(customgridcolumns)+","+b2str(protectexistinglayout)+","+b2str(name)+")"))
    self.handle.remove(0,self.handle.indexOf("ing ")+4)
    
class ColorContourImage(Image) :
  """ This class represents an image you would create via "Create>Image>Color Map and Contour Map" from the menubar inside kst.
  Not to be confused with Pictures, Images are representations of matrices and are placed within plots.
  
  The parameters of this function mirror the parameters within "Create>Image>Color Map and Contour Map".
    
  To place in an existing plot, specify placeinexistingplot = plot.
  
  To prevent a the curve from being placed in any plot, set donotplaceinanyplot=True.  The default is to place the curve in a new plot.
    
  Not specifying a parameter implies it's default value (i.e., the setting used on the previous curve whether through a script or
  by the GUI)."""
  def __init__(self,client,matrix,palette,rtAutoThreshold,lower,upper,maxmin,smart,percentile,levels,color="black",weight="12",variable=False,donotplaceinanyplot=False,placeinexistingplot=0,placeinnewplot=True,placeinnewtab=False,scalefonts=True,autolayout=True,customgridcolumns=False,protectexistinglayout=False,name="") :
    Image.__init__(self,client)
    
    if placeinexistingplot != 0:
      donotplaceinanyplot=False
      placeinnewplot=False
      placeinnewtab=False
      existingplothandle = placeinexistingplot.handle
    else:
      existingplothandle = " "
      
    if donotplaceinanyplot == True:
      placeinnewplot=False
      placeinnewtab=False
      existingplothandle = " "

    self.handle=QtCore.QString(self.client.send("#newImageFromColor("+b2str(matrix)+","+b2str(palette)+","+b2str(rtAutoThreshold)+","+b2str(lower)+","+b2str(upper)+","+b2str(maxmin)+","+b2str(smart)+","+b2str(percentile)+","+b2str(levels)+","+b2str(color)+","+b2str(weight)+","+b2str(variable)+","+b2str(donotplaceinanyplot)+","+b2str(existingplothandle)+","+b2str(placeinnewplot)+","+b2str(placeinnewtab)+","+b2str(scalefonts)+","+b2str(autolayout)+","+b2str(customgridcolumns)+","+b2str(protectexistinglayout)+","+b2str(name)+")"))
    self.handle.remove(0,self.handle.indexOf("ing ")+4)
    
class ExistingImage(Image) :
  """ This class allows access to an image created inside kst or through a script given a descriptive or short name.
  
  "handle" is a descriptive or short name of an image created inside kst or through a script. """
  def __init__(self,client,handle) :
    Image.__init__(self,client,handle)
    self.handle=handle

  @classmethod
  def getList(cls,client):
    x=QtCore.QString(client.send("getImageList()"))
    ret=[]
    while x.contains('['):
      y=QtCore.QString(x)
      y.remove(0,1)
      y.remove(y.indexOf(']'),9999999)
      x.remove(0,x.indexOf(']')+1)
      ret.append(ExistingImage(client,y))
    return ret




class Spectrum(NamedObject):
  """ This is the base class for Kst Spectra.  To create a new spectrum, use pykst.NewSpectrum() or
  to access an existing equation use pykst.ExistingSpectrum()

  For example, to plot the spectrum of a pykst.DataVector() "dataVectorGY1": ::
  
      GYSpectrum=kst.NewSpectrum(client, dataVectorGY1, length = 12, vectorUnits="^o/s", 
                                 rateUnits="Hz", rate=100.16)
      GYSpecCurve = kst.NewCurve(client,GYSpectrum.X(), GYSpectrum.Y(), curvecolor="green", 
                                 curveweight=1, placeinnewplot=True)

  """
  def __init__(self,client) :
    NamedObject.__init__(self,client)

  def Y(self) :
    """ Returns the Y vector of a spectrum """
    YHandle = QtCore.QString(self.client.send("DataObject::outputVectorHandle("+self.handle+", S)"))
    return ExistingVector(self.client, YHandle)

  def X(self) :
    """ Returns the X vector of a spectrum (ie, the frequency) """
    XHandle = QtCore.QString(self.client.send("DataObject::outputVectorHandle("+self.handle+", F)"))
    return ExistingVector(self.client, XHandle)

class NewSpectrum(Spectrum) :
  """ This class represents an spectrum you would create via "Create>Spectrum" from the menubar inside kst.
  The parameters of this function mirror the parameters within "Create>Spectrum".
            
  ToDo: examples """
  
  def __init__(self,client,vector,removeMean=True,apodize=True,function="",sigma=1.0,interleaved=True,length=10,interpolate=True,rate=1.0,vectorUnits="",rateUnits="",outputType="",name="") :
    Spectrum.__init__(self,client)
      
    self.handle=QtCore.QString(self.client.send("#newSpectrum("+b2str(vector.handle)+","+b2str(removeMean)+","+b2str(apodize)+","+b2str(function)+","+b2str(sigma)+","+b2str(interleaved)+","+b2str(length)+","+b2str(interpolate)+","+b2str(rate)+","+b2str(vectorUnits)+","+b2str(rateUnits)+","+b2str(outputType)+")"))
    self.handle.remove(0,self.handle.indexOf("ing ")+4)

class ExistingSpectrum(Spectrum) :
  """ This class allows access to an spectrum created inside kst or through a script given a descriptive or short name.
  "handle" is a descriptive or short name of an spectrum created inside kst or through a script. """
  def __init__(self,client,handle) :
    Spectrum.__init__(self,client)
    self.handle=handle
    
  @classmethod
  def getList(cls,client):
    x=QtCore.QString(client.send("getPSDList()"))
    ret=[]
    while x.contains('['):
      y=QtCore.QString(x)
      y.remove(0,1)
      y.remove(y.indexOf(']'),9999999)
      x.remove(0,x.indexOf(']')+1)
      ret.append(ExistingSpectrum(client,y))
    return ret


class Histogram(NamedObject):
  """ This is the base class for Kst Histograms.  To create a new histogram, use pykst.NewHistogram() or
  to access an existing histogram use pykst.ExistingHistogram()

  For example, to plot the histogram of a pykst.DataVector() "dataVectorGY1": ::
  
      GYHist=kst.NewHistogram(client, dataVectorGY1, rtAutoBin=True)
      GYHistCurve = kst.NewCurve(client,GYHist.X(), GYHist.Y(), curvecolor="black", 
                                 curveweight=1, uselines=False, usebargraph=True,
                                 bargraphfill="green",placeinnewplot=True)

  """
  def __init__(self,client) :
    NamedObject.__init__(self,client)

  def Y(self) :
    """ Returns the Y vector of a histogram """
    YHandle = QtCore.QString(self.client.send("DataObject::outputVectorHandle("+self.handle+", H)"))
    return ExistingVector(self.client, YHandle)

  def X(self) :
    """ Returns the X vector of a histogram """
    XHandle = QtCore.QString(self.client.send("DataObject::outputVectorHandle("+self.handle+", B)"))
    return ExistingVector(self.client, XHandle)
    

class NewHistogram(Histogram) :
  """ This class represents an histogram you would create via "Create>Histogram" from the menubar inside kst.
  The parameters of this function mirror the parameters within "Create>Histogram"."""
  
  def __init__(self,client,vector,rtAutoBin=True,binsFrom=0,binsTo=1,nbins=40,yaxisNormNumInBin=True,yaxisNormFracInBin=False,yaxisNormPercentInBin=False,yaxisNormPeakAt1=False,name="") :
    Histogram.__init__(self,client)
      
    self.handle=QtCore.QString(self.client.send("#newHistogram("+b2str(vector.handle)+","+b2str(rtAutoBin)+","+b2str(binsFrom)+","+b2str(binsTo)+","+b2str(nbins)+","+b2str(yaxisNormNumInBin)+","+b2str(yaxisNormFracInBin)+","+b2str(yaxisNormPercentInBin)+","+b2str(yaxisNormPeakAt1)+")"))
    self.handle.remove(0,self.handle.indexOf("ing ")+4)

class ExistingHistogram(Histogram) :
  """ This class allows access to an histogram created inside kst or through a script given a descriptive or short name.
  
  "handle" is a descriptive or short name of an histogram created inside kst or through a script. """
  def __init__(self,client,handle) :
    Histogram.__init__(self,client)
    self.handle=handle
    
  @classmethod
  def getList(cls,client):
    x=QtCore.QString(client.send("getPSDList()"))
    ret=[]
    while x.contains('['):
      y=QtCore.QString(x)
      y.remove(0,1)
      y.remove(y.indexOf(']'),9999999)
      x.remove(0,x.indexOf(']')+1)
      ret.append(ExistingHistogram(client,y))
    return ret

class Spectrogram(NamedObject):
  """ TODO... """
  def __init__(self,client) :
    NamedObject.__init__(self,client)





class ViewItem:
  """ This is a class which some convenience classes within pykst use. You should not use it directly.
  
  NOTE: Kst considers ViewItems to be NamedObjects, but except for plots, no mechanism for naming view items
  exists or would be useful. As such, within scripting, ViewItems do not have names."""
  def __init__(self,client):
    self.client=client
    
  def setHMargin(self,margin):
    """ This is equivalent to setting Apperance>Layout>Horizontal Margin within a view item dialog in kst. """
    #self.client.send(b2str("#setHMargin("+self.handle.toAscii()+","+b2str(margin)+")"))
    self.client.send_si(self.handle, b2str("setLayoutHorizontalMargin("+b2str(margin)+")"))

  def setVMargin(self,margin):
    """ This is equivalent to setting Apperance>Layout>Vertical Margin within a view item dialog in kst. """
    #self.client.send(b2str("#setVMargin("+self.handle.toAscii()+","+b2str(margin)+")"))
    self.client.send_si(self.handle, b2str("setLayoutVerticalMargin("+b2str(margin)+")"))

  def setHSpace(self,space):
    """ This is equivalent to setting Apperance>Layout>Horizontal Spacing within a view item dialog in kst. """
    #self.client.send(b2str("#setHSpace("+self.handle.toAscii()+","+b2str(space)+")"))
    self.client.send_si(self.handle, b2str("setLayoutHorizontalSpacing("+b2str(space)+")"))

  def setVSpace(self,space):
    """ This is equivalent to setting Apperance>Layout>Vertical Spacing within a view item dialog in kst. """
    #self.client.send(b2str("#setVSpace("+self.handle.toAscii()+","+b2str(space)+")"))
    self.client.send_si(self.handle, b2str("setLayoutVerticalSpacing("+b2str(space)+")"))

  def setFillColor(self,color):
    """ This is equivalent to setting Apperance>Fill>Color within a view item dialog in kst. Not all view items
    support fill color. Colors are given by a name such as 'red' or a hex number such as '#FF0000'. """
    self.client.send_si(self.handle, b2str("setFillColor("+b2str(color)+")"))
    
  def setFillStyle(self,style):
    """ This is equivalent to setting the index of Apperance>Fill>Style within a view item dialog in kst.
    Not all view items support fill styles. 0 is NoBrush, 1 is SolidPattern, 2 is Dense1Pattern, 3 is Dense2Pattern,
    4 is Dense3Pattern, 5 is Dense4Pattern, 6 is Dense5Pattern, 7 is Dense6Pattern, 8 is Dense7Pattern
    9 is HorPattern, 11 is VerPattern, 12 is CrossPattern, 13 is BDiagPattern and 14 is FDiagPattern."""
    self.client.send_si(self.handle, b2str("setIndexOfFillStyle("+b2str(style)+")"))

  def setStrokeStyle(self,style):
    """ This is equivalent to setting the index of Apperance>Stroke>Style within a view item dialog in kst. Not all view items
    support stroke styles. 0 is SolidLine, 1 is DashLine, 2 is DotLine, 3 is DashDotLine, 4 is DashDotDotLine, and 5 is
    CustomDashLine"""
    self.client.send_si(self.handle, b2str("setIndexOfStrokeStyle("+b2str(style)+")"))

  def setStrokeWidth(self,width):
    """ This is equivalent to setting Apperance>Stroke>Width within a view item dialog in kst. Not all view items
    support stroke width."""
    self.client.send_si(self.handle, b2str("setStrokeWidth("+b2str(width)+")"))

  def setStrokeBrushColor(self,color):
    """ This is equivalent to setting Apperance>Stroke>Brush Color within a view item dialog in kst. Not all view items
    support fill color. Colors are given by a name such as 'red' or a hex number such as '#FF0000'. """
    self.client.send_si(self.handle, b2str("setStrokeBrushColor("+b2str(color)+")"))

  def setStrokeBrushStyle(self,style):
    """ This is equivalent to setting the index of Apperance>Stroke>Brush Style within a view item dialog in kst.
    Not all view items support stroke brush styles. 0 is NoBrush, 1 is SolidPattern, 2 is Dense1Pattern, 3 is Dense2Pattern,
    4 is Dense3Pattern, 5 is Dense4Pattern, 6 is Dense5Pattern, 7 is Dense6Pattern, 8 is Dense7Pattern
    9 is HorPattern, 11 is VerPattern, 12 is CrossPattern, 13 is BDiagPattern and 14 is FDiagPattern."""
    self.client.send_si(self.handle, b2str("setIndexOfStrokeBrushStyle("+b2str(style)+")"))

  def setStrokeJoinStyle(self,style):
    """ This is equivalent to setting the index of Apperance>Stroke>Join Style within a view item dialog in kst.
    Not all view items support stroke join styles. 0 is MiterJoin, 1 is BevelJoin, 2 is RoundJoin,
    and 3 is SvgMiterJoin."""
    self.client.send_si(self.handle, b2str("setIndexOfStrokeJoinStyle("+b2str(style)+")"))

  def setStrokeCapStyle(self,style):
    """ This is equivalent to setting the index of Apperance>Stroke>Cap Style within a view item dialog in kst.
    Not all view items support stroke cap styles. 0 is FlatCap, 1 is SquareCap, and 2 is RoundCap"""
    self.client.send_si(self.handle, b2str("setIndexOfStrokeCapStyle("+b2str(style)+")"))

  def setFixedAspectRatio(self, fixed=True):
    """ This is equivalent to checking Dimensions>Fix aspect ratio within a view item dialog in kst.
    The behaviour of this is undefined in view items which always have fixed aspect ratios (e.g., circles)"""
    if fixed == True:
      self.client.send_si(self.handle, b2str("checkFixAspectRatio()"))
    else:
      self.client.send_si(self.handle, b2str("uncheckFixAspectRatio()"))

  def setPosX(self,pos):
    """ This is equivalent to setting Dimensions>Position within a view item dialog in kst.
    Positions refer to the center of an object and x-values range from 0.0 (left) to 1.0 (right)"""
    self.client.send_si(self.handle, b2str("setPosX("+b2str(pos)+")"))

  def setPosY(self,pos):
    """ This is equivalent to setting Dimensions>Position within a view item dialog in kst.
    Positions refer to the center of an object and y-values range from 0.0(top) to 1.0(bottom)"""
    self.client.send_si(self.handle, b2str("setPosY("+b2str(pos)+")"))

  def setSizeX(self,size):
    """ This is equivalent to setting Dimensions>Size within a view item dialog. In cases
    where the aspect ratio is fixed, this changes both sizes (x and y)"""
    #self.client.send(b2str("#setSizeX("+self.handle+","+b2str(size)+")"))
    self.client.send_si(self.handle, b2str("setGeoX("+b2str(size)+")"))

  def setSizeY(self,size):
    """ This is equivalent to setting Dimensions>Size within a view item dialog. In cases
    where the aspect ratio is fixed, this behaviour of this function is undefined."""
    #self.client.send(b2str("#setSizeY("+self.handle+","+b2str(size)+")"))
    self.client.send_si(self.handle, b2str("setGeoY("+b2str(size)+")"))

  def setRotation(self,rot):
    """ This is equivalent to setting Dimensions>Rotation within a view item dialog."""
    #self.client.send(b2str("#setRot("+self.handle+","+b2str(rot)+")"))
    self.client.send_si(self.handle, b2str("setRotation("+b2str(rot)+")"))

  def remove(self):
    """ This removes the object from Kst. You should be careful when using this, because any handles you have to this object will
        still exist and be invalid. Additionally, in Kst, ViewItems are not actually deleted, only hidden, causing even more
        problems. """
    self.client.send("eliminate("+self.handle.toAscii()+")")

class ExistingViewItem(ViewItem) :
  def  __init__(self,client,handle):
    ViewItem.__init__(self,client)
    self.handle=handle

# LABELS ######################################################################
class Label(ViewItem) :
  """ This class represents a label you would create via "Create>Annotations>Label" from the menubar inside kst.
  
  The parameters of this function mirror the parameters within the label edit dialog which can be acessed by right clicking on
  a label and selecting Edit. Colors are given by a name such as 'red' or a hex number such as '#FF0000'".
  
  See the kstOwl example."""
  def __init__(self,client,text,fontSize=12,bold=False,italic=False,fontColor="black",fontFamily="Serif",posX=0.1,posY=0.1,rot=0):
    ViewItem.__init__(self,client)
    self.client.send("newLabel()")
    self.handle=QtCore.QString(self.client.send("endEdit()"))
    self.handle.remove(0,self.handle.indexOf("ing ")+4)

    self.setText(text)
    self.setLabelFontSize(fontSize)
    self.setPosX(posX)
    self.setPosY(posY)
    self.setFixedAspectRatio(True)
    self.setRotation(rot)
    self.setFontColor(fontColor)
    self.setFontFamily(fontFamily)

    self.setFontBold(bold)
    self.setFontItalic(italic)

  def setText(self,text):
    """ Set text. It may be faster to insert strings within labels (e.g.,"[String (X1)]") and simply modify strings. """
    self.client.send_si(self.handle, b2str("setLabel("+b2str(text)+")"))

  def setLabelFontSize(self,size):
    """ This does not actually represent point size but is relative to the size of the window. """
    self.client.send_si(self.handle, b2str("setFontSize("+b2str(size)+")"))

  def setFontBold(self, bold = True):
    """ . . . """
    if bold == True:
      self.client.send_si(self.handle, b2str("checkLabelBold()"))
    else:
      self.client.send_si(self.handle, b2str("uncheckLabelBold()"))

  def setFontItalic(self, italic = True):
    """ . . . """
    if italic == True:
      self.client.send_si(self.handle, b2str("checkLabelItalic()"))
    else:
      self.client.send_si(self.handle, b2str("uncheckLabelItalic()"))

  def setFontColor(self,color):
    """ Colors are given by a name such as 'red' or a hex number such as '#FF0000' """
    self.client.send_si(self.handle, b2str("setLabelColor("+b2str(color)+")"))

  def setFontFamily(self,family):
    """ . . . """
    self.client.send_si(self.handle, b2str("setFontFamily("+b2str(family)+")"))


class ExistingLabel(Label):
  def  __init__(self,client,handle):
    ViewItem.__init__(self,client)
    self.handle=handle
    
  @classmethod
  def getList(cls,client):
    x=QtCore.QString(client.send("getLabelList()"))
    ret=[]
    while x.contains('['):
      y=QtCore.QString(x)
      y.remove(0,1)
      y.remove(y.indexOf(']'),9999999)
      x.remove(0,x.indexOf(']')+1)
      ret.append(ExistingLabel(client,y))
    return ret
  




class Box(ViewItem) :
  """ This class represents a box you would create via "Create>Annotations>Box" from the menubar inside kst.

  The parameters of this function mirror the parameters within the box edit dialog which can be acessed by right clicking on
  a box and selecting Edit. Colors are given by a name such as 'red' or a hex number such as '#FF0000'".
  See the kstOwl example."""
  def __init__(self,client,fixAspect=False,posX=0.1,posY=0.1,sizeX=0.1,sizeY=0.1,rot=0,fillColor="white",fillStyle=1,strokeStyle=1,strokeWidth=1,strokeBrushColor="black",strokeBrushStyle=1,strokeJoinStyle=1,strokeCapStyle=1) :
    ViewItem.__init__(self,client)

    self.client.send("newBox()")
    self.handle=QtCore.QString(self.client.send("endEdit()"))
    self.handle.remove(0,self.handle.indexOf("ing ")+4)

    self.setPosX(posX)
    self.setPosY(posY)
    self.setSizeX(sizeX)
    self.setSizeY(sizeY)
    if fixAspect==True:
        self.setFixedAspectRatio(True)
    else:
        self.setFixedAspectRatio(False)
    self.setRotation(rot)

    self.setStrokeBrushColor(strokeBrushColor)
    self.setFillColor(fillColor)
    self.setFillStyle(fillStyle)
    self.setStrokeStyle(strokeStyle)
    self.setStrokeWidth(strokeWidth)
    self.setStrokeBrushColor(strokeBrushColor)
    self.setStrokeBrushStyle(strokeBrushStyle)
    self.setStrokeJoinStyle(strokeJoinStyle)
    self.setStrokeCapStyle(strokeCapStyle)

  @classmethod
  def getList(cls,client):
    x=QtCore.QString(client.send("getBoxList()"))
    ret=[]
    while x.contains('['):
      y=QtCore.QString(x)
      y.remove(0,1)
      y.remove(y.indexOf(']'),9999999)
      x.remove(0,x.indexOf(']')+1)
      ret.append(ExistingViewItem(client,y))
    return ret





class Circle(ViewItem) :
  """ This class represents a circle you would create via "Create>Annotations>Circle" from the menubar inside kst.
  
  The parameters of this function mirror the parameters within the circle edit dialog which can be acessed by right clicking on
  a circle and selecting Edit. Colors are given by a name such as 'red' or a hex number such as '#FF0000'".
  See the bonjourMonde example"""
  def __init__(self,client,posX=0.1,posY=0.1,size=0.1,fillColor="white",fillStyle=1,strokeStyle=1,strokeWidth=1,strokeBrushColor="grey",strokeBrushStyle=1,strokeJoinStyle=1,strokeCapStyle=1) :
    ViewItem.__init__(self,client)

    self.client.send("newCircle()")
    self.handle=QtCore.QString(self.client.send("endEdit()"))
    self.handle.remove(0,self.handle.indexOf("ing ")+4)

    self.setPosX(posX)
    self.setPosY(posY)
    self.setSizeX(size)

    self.setStrokeBrushColor(strokeBrushColor)
    self.setFillColor(fillColor)
    self.setFillStyle(fillStyle)
    self.setStrokeStyle(strokeStyle)
    self.setStrokeWidth(strokeWidth)
    self.setStrokeBrushColor(strokeBrushColor)
    self.setStrokeBrushStyle(strokeBrushStyle)
    self.setStrokeJoinStyle(strokeJoinStyle)
    self.setStrokeCapStyle(strokeCapStyle)

  @classmethod
  def getList(cls,client):
    x=QtCore.QString(client.send("getCircleList()"))
    ret=[]
    while x.contains('['):
      y=QtCore.QString(x)
      y.remove(0,1)
      y.remove(y.indexOf(']'),9999999)
      x.remove(0,x.indexOf(']')+1)
      ret.append(ExistingViewItem(client,y))
    return ret
    




class Ellipse(ViewItem) :
  """ This class represents an ellipse you would create via "Create>Annotations>Ellipse" from the menubar inside kst.
  
  The parameters of this function mirror the parameters within the ellipse edit dialog which can be acessed by right clicking on
  an ellipse and selecting Edit. Colors are given by a name such as 'red' or a hex number such as '#FF0000'"."""
  def __init__(self,client,fixAspect=False,posX=0.1,posY=0.1,sizeX=0.1,sizeY=0.1,rot=0,fillColor="white",fillStyle=1,strokeStyle=1,strokeWidth=1,strokeBrushColor="black",strokeBrushStyle=1,strokeJoinStyle=1,strokeCapStyle=1) :
    ViewItem.__init__(self,client)
    self.client.send("newEllipse()")
    self.handle=QtCore.QString(self.client.send("endEdit()"))
    self.handle.remove(0,self.handle.indexOf("ing ")+4)

    self.setPosX(posX)
    self.setPosY(posY)
    self.setSizeX(sizeX)
    self.setSizeY(sizeY)
    if fixAspect==True:
        self.setFixedAspectRatio(True)
    else:
        self.setFixedAspectRatio(False)
    self.setRotation(rot)

    self.setStrokeBrushColor(strokeBrushColor)
    self.setFillColor(fillColor)
    self.setFillStyle(fillStyle)
    self.setStrokeStyle(strokeStyle)
    self.setStrokeWidth(strokeWidth)
    self.setStrokeBrushColor(strokeBrushColor)
    self.setStrokeBrushStyle(strokeBrushStyle)
    self.setStrokeJoinStyle(strokeJoinStyle)
    self.setStrokeCapStyle(strokeCapStyle)

  @classmethod
  def getList(cls,client):
    x=QtCore.QString(client.send("getEllipseList()"))
    ret=[]
    while x.contains('['):
      y=QtCore.QString(x)
      y.remove(0,1)
      y.remove(y.indexOf(']'),9999999)
      x.remove(0,x.indexOf(']')+1)
      ret.append(ExistingViewItem(client,y))
    return ret





class Line(ViewItem) :
  """ This class represents a line you would create via "Create>Annotations>Line" from the menubar inside kst.

  Colors are given by a name such as 'red' or a hex number such as '#FF0000'"."""
  def __init__(self,client,posX=0.1,posY=0.1,length=0.1,rot=0,strokeStyle=1,strokeWidth=1,strokeBrushColor="black",strokeBrushStyle=1,strokeJoinStyle=1,strokeCapStyle=1) :
    ViewItem.__init__(self,client)
    self.client.send("newLine()")
    self.handle=QtCore.QString(self.client.send("endEdit()"))
    self.handle.remove(0,self.handle.indexOf("ing ")+4)

    self.setPosX(posX)
    self.setPosY(posY)
    self.setSizeX(length)
    self.setRotation(rot)

    self.setStrokeBrushColor(strokeBrushColor)
    self.setStrokeStyle(strokeStyle)
    self.setStrokeWidth(strokeWidth)
    self.setStrokeBrushColor(strokeBrushColor)
    self.setStrokeBrushStyle(strokeBrushStyle)
    self.setStrokeJoinStyle(strokeJoinStyle)
    self.setStrokeCapStyle(strokeCapStyle)

  @classmethod
  def getList(cls,client):
    x=QtCore.QString(client.send("getCircleList()"))
    ret=[]
    while x.contains('['):
      y=QtCore.QString(x)
      y.remove(0,1)
      y.remove(y.indexOf(']'),9999999)
      x.remove(0,x.indexOf(']')+1)
      ret.append(ExistingViewItem(client,y))
    return ret



class Arrow(ViewItem) :
  """ This class represents an arrow you would create via "Create>Annotations>Arrow" from the menubar inside kst.
  
  The parameters of this function mirror the parameters within the arrow edit dialog which can be acessed by right clicking on
  a arrow and selecting Edit. Colors are given by a name such as 'red' or a hex number such as '#FF0000'"."""
  def __init__(self,client,posX=0.1,posY=0.1,length=0.1, rot=0,arrowAtStart = False, arrowAtEnd = True, arrowSize = 12.0, strokeStyle=1,strokeWidth=1,strokeBrushColor="black",strokeBrushStyle=1,strokeJoinStyle=1,strokeCapStyle=1) :
    ViewItem.__init__(self,client)
    self.client.send("newArrow()")
    self.handle=QtCore.QString(self.client.send("endEdit()"))
    self.handle.remove(0,self.handle.indexOf("ing ")+4)

    self.setPosX(posX)
    self.setPosY(posY)
    self.setSizeX(length)
    self.setRotation(rot)

    self.setStrokeBrushColor(strokeBrushColor)
    self.setStrokeStyle(strokeStyle)
    self.setStrokeWidth(strokeWidth)
    self.setStrokeBrushColor(strokeBrushColor)
    self.setStrokeBrushStyle(strokeBrushStyle)
    self.setStrokeJoinStyle(strokeJoinStyle)
    self.setStrokeCapStyle(strokeCapStyle)
    self.setArrowAtStart(arrowAtStart)
    self.setArrowAtEnd(arrowAtEnd)
    self.setArrowSize(arrowSize)

  def setArrowAtStart(self, arrow=True) :
    """ Set whether an arrow head is shown at the start of the line """
    if arrow==True:
      self.client.send_si(self.handle, b2str("arrowAtStart(true)"))
    else:
      self.client.send_si(self.handle, b2str("arrowAtStart(false)"))

  def setArrowAtEnd(self, arrow=True) :
    """ Set whether an arrow head is shown at the end of the line """
    if arrow==True:
      self.client.send_si(self.handle, b2str("arrowAtEnd(true)"))
    else:
      self.client.send_si(self.handle, b2str("arrowAtEnd(false)"))

  def setArrowSize(self, arrowSize) :
    self.client.send_si(self.handle, b2str("arrowHeadScale("+b2str(arrowSize)+")"))

  @classmethod
  def getList(cls,client):
    x=QtCore.QString(client.send("getArrowList()"))
    ret=[]
    while x.contains('['):
      y=QtCore.QString(x)
      y.remove(0,1)
      y.remove(y.indexOf(']'),9999999)
      x.remove(0,x.indexOf(']')+1)
      ret.append(ExistingViewItem(client,y))
    return ret
    
    
    
    
class Picture(ViewItem) :
  """ This class represents a picture you would create via "Create>Annotations>Picture" from the menubar inside kst.
  
  The parameters of this function mirror the parameters within the picture edit dialog which can be acessed by right clicking on
  a picture and selecting Edit. Colors are given by a name such as 'red' or a hex number such as '#FF0000'".
  
  See the kstOwl example."""
  def __init__(self,client,filename,posX=0.1,posY=0.1,size=0.1,rot=0) :
    ViewItem.__init__(self,client)
    self.client.send("newPicture("+b2str(filename)+")")
    self.handle=QtCore.QString(self.client.send("endEdit()"))
    self.handle.remove(0,self.handle.indexOf("ing ")+4)

    self.setPosX(posX)
    self.setPosY(posY)
    self.setFixedAspectRatio(True)
    self.setSizeX(size)
    #fixme: aspect ratio is wrong.
    #self.setSizeY(size)
    self.setRotation(rot)

  def setPicture(self,pic):
    """ BUG: aspect ratio is not changed. There is no parellel for this function within the kst GUI. """
    self.client.send_si(self.handle, b2str("setPicture("+b2str(pic)+")"))

class ExistingPicture(Picture):
  def __init__(self,client,handle):
    ViewItem.__init__(self,client)
    self.handle=handle
    
  @classmethod
  def getList(cls,client):
    x=QtCore.QString(client.send("getPictureList()"))
    ret=[]
    while x.contains('['):
      y=QtCore.QString(x)
      y.remove(0,1)
      y.remove(y.indexOf(']'),9999999)
      x.remove(0,x.indexOf(']')+1)
      ret.append(ExistingPicture(client,y))
    return ret





class SVG(ViewItem) :
  """ This class represents a SVG you would create via "Create>Annotations>SVG" from the menubar inside kst.
  
  The parameters of this function mirror the parameters within the SVG edit dialog which can be acessed by right clicking on
  a SVG and selecting Edit. Colors are given by a name such as 'red' or a hex number such as '#FF0000'"."""
  def __init__(self,client,filename,posX=0.1,posY=0.1,size=0.1,rot=0) :
    ViewItem.__init__(self,client)
    self.client.send("newSvgItem("+b2str(filename)+")")
    self.handle=QtCore.QString(self.client.send("endEdit()"))
    self.handle.remove(0,self.handle.indexOf("ing ")+4)

    self.setPosX(posX)
    self.setPosY(posY)
    self.setSizeX(size)
    self.setSizeY(size)
    self.setFixedAspectRatio(True)
    self.setRotation(rot)


class ExistingSVG(SVG):
  def __init__(self,client,handle):
    ViewItem.__init__(self,client)
    self.handle=handle
    
  @classmethod
  def getList(cls,client):
    x=QtCore.QString(client.send("getSVGList()"))
    ret=[]
    while x.contains('['):
      y=QtCore.QString(x)
      y.remove(0,1)
      y.remove(y.indexOf(']'),9999999)
      x.remove(0,x.indexOf(']')+1)
      ret.append(ExistingSVG(client,y))
    return ret





class Plot(ViewItem) :
  """ This class represents a Plot you would create via "Create>Annotations>Plot" from the menubar inside kst.
  TODO: a lot.
  
  The parameters of this function mirror a few of the parameters within the plot edit dialog which can be acessed by right clicking on
  a plot and selecting Edit. To add curves and images to plots, use functions inside this class which don't exist yet or use the Curve/Equation/Image
  conb2structors. See the ksNspire example. """
  def __init__(self,client,posX=0.1,posY=0.1,sizeX=0.1,sizeY=0.1,rot=0,name="") :
    ViewItem.__init__(self,client)
    self.client.send("newPlot()")
    self.client.send("setPosX("+b2str(posX)+")")
    self.client.send("setPosY("+b2str(posY)+")")
    self.client.send("setGeoX("+b2str(sizeX)+")")
    self.client.send("setGeoY("+b2str(sizeY)+")")
    self.client.send("setRotation("+b2str(rot)+")")
    if name!="":
      self.client.send("uncheckAuto()")
      self.client.send("setName("+b2str(name)+")")
    else:
      self.client.send("checkAuto()")
    self.handle=QtCore.QString(self.client.send("endEdit()"))
    self.handle.remove(0,self.handle.indexOf("ing ")+4)

  def setXRange(self,x0 = 0.0, x1 = 10.0) :
    """ Set X zoom range from x0 to x1 """
    self.client.send_si(self.handle, b2str("setXRange("+b2str(x0)+","+b2str(x1)+")"))

  def setYRange(self, y0 = 0.0, y1 = 10.0) :
    """ Set Y zoom range from y0 to y1 """
    self.client.send_si(self.handle, b2str("setYRange("+b2str(y0)+","+b2str(y1)+")"))

  def setXAuto(self) :
    """ Set X zoom range to autoscale """
    self.client.send_si(self.handle,b2str("setXAuto()"))

  def setYAuto(self) :
    """ Set Y zoom range to autoscale """
    self.client.send_si(self.handle, b2str("setPlotYAuto()"))

  def setXAutoBorder(self) :
    """ Set X zoom range to autoscale with a small border """
    self.client.send_si(self.handle, b2str("setPlotXAutoBorder()"))

  def setYAutoBorder(self) :
    """ Set Y zoom range to autoscale with a small border """
    self.client.send_si(self.handle,b2str("setYAutoBorder()"))

  def setXNoSpike(self) :
    """ Set X zoom range to spike insensitive autoscale """
    self.client.send_si(self.handle,b2str("setXNoSpike()"))

  def setYNoSpike(self) :
    """ Set Y zoom range to spike insensitive autoscale """
    self.client.send_si(self.handle,b2str("setYNoSpike()"))

  def setXAC(self, r=0.2) :
    """ Set X zoom range to fixed range, centered around the mean.  Similar to AC coupling on an oscilloscope. """
    self.client.send_si(self.handle,b2str("setXAC("+b2str(r)+")"))

  def setYAC(self, r=0.2) :
    """ Set Y zoom range to fixed range, centered around the mean.  Similar to AC coupling on an oscilloscope. """
    self.client.send_si(self.handle,b2str("setYAC("+b2str(r)+")"))

  def setGlobalFont(self, family="", bold=False, italic=False) :
    """ Set the global plot font.  By default, the axis labels all use this, unless they have been set to use their own.
        If the parameter 'family' is empty, the font family will be unchanged.
        The font will be bold if parameter 'bold' is set to 'bold' or 'true'.
        The font will be italic if parameter 'italic' is set to 'italic' or 'true'."""
    self.client.send_si(self.handle,b2str("setGlobalFont("+family+","+b2str(bold)+","+b2str(italic)+")"))

  def setTopLabel(self, label="") :
    """ Set the plot top label """
    self.client.send_si(self.handle,b2str("setTopLabel("+label+")"))

  def setBottomLabel(self, label="") :
    """ Set the plot bottom label """
    self.client.send_si(self.handle,b2str("setBottomLabel("+label+")"))

  def setLeftLabel(self, label="") :
    """ Set the plot left label """
    self.client.send_si(self.handle,b2str("setLeftLabel("+label+")"))

  def setRightLabel(self, label="") :
    """ Set the plot right label """
    self.client.send_si(self.handle,b2str("setRightLabel("+label+")"))

  def setTopLabelAuto(self) :
    """ Set the top label to auto generated. """
    self.client.send_si(self.handle,b2str("setTopLabelAuto()"))
  def setBottomLabelAuto(self) :
    """ Set the bottom label to auto generated. """
    self.client.send_si(self.handle,b2str("setBottomLabelAuto()"))

  def setLeftLabelAuto(self) :
    """ Set the left label to auto generated. """
    self.client.send_si(self.handle,b2str("setLeftLabelAuto()"))

  def setRightLabelAuto(self) :
    """ Set the right label to auto generated. """
    self.client.send_si(self.handle,b2str("setRightLabelAuto()"))

  def normalizeXtoY(self) :
    """ Adjust the X zoom range so X and Y have the same scale per unit (square pixels) """
    self.client.send_si(self.handle, b2str("normalizeXtoY()"))

  def setLogX(self) :
    """ Set X axis to log mode. """
    self.client.send_si(self.handle, b2str("setLogX()"))

  def setLogY(self) :
    """ Set X axis to log mode. """
    self.client.send_si(self.handle, b2str("setLogY()"))

  def setYAxisReversed(self, reversed=True) :
    """ set the Y axis to decreasing from bottom to top. """
    if reversed == True:
      self.client.send_si(self.handle, b2str("setYAxisReversed()"))
    else:
      self.client.send_si(self.handle, b2str("setYAxisNotReversed()"))

  def setXAxisReversed(self, reversed=True) :
    """ set the X axis to decreasing from left to right. """
    if reversed == True:
      self.client.send_si(self.handle, b2str("setXAxisReversed()"))
    else:
      self.client.send_si(self.handle, b2str("setXAxisNotReversed()"))


class ExistingPlot(Plot):
  def __init__(self,client,handle):
    ViewItem.__init__(self,client)
    self.handle=handle
    
  @classmethod
  def getList(cls,client):
    x=QtCore.QString(client.send("getPlotList()"))
    ret=[]
    while x.contains('['):
      y=QtCore.QString(x)
      y.remove(0,1)
      y.remove(y.indexOf(']'),9999999)
      x.remove(0,x.indexOf(']')+1)
      ret.append(ExistingPlot(client,y))
    return ret





class Button(ViewItem) :
  """ This represents a button inside a View. When the button is pressed, it sends a message via a socket.
  
  socket is a QtNetwork.QLocalSocket that is not connected to anything. The message "clicked" will be sent
  when the button is pressed. See the bonjourMonde example. """
  def __init__(self,client,text,socket,posX=0.1,posY=0.1,sizeX=0.1,sizeY=0.1,rot=0) :
    ViewItem.__init__(self,client)
    self.client.send("newButton()")
    self.client.send("setPosX("+b2str(posX)+")")
    self.client.send("setPosY("+b2str(posY)+")")
    self.client.send("setGeoX("+b2str(sizeX)+")")
    self.client.send("setGeoY("+b2str(sizeY)+")")
    self.client.send("setText("+b2str(text)+")")
    self.client.send("setRotation("+b2str(rot)+")")
    self.handle=QtCore.QString(self.client.send("endEdit()"))
    self.handle.remove(0,self.handle.indexOf("ing ")+4)
    socket.connectToServer(client.serverName)
    socket.waitForConnected(300)
    socket.write(b2str("attachTo("+self.handle+")"))
    
  def setText(self,text):
    """ Sets the text of the button. """
    self.client.send("beginEdit("+self.handle+")")
    self.client.send("setText("+b2str(text)+")")
    self.client.send("endEdit()")





class LineEdit(ViewItem) :
  """ This represents a line edit inside a View. When the lineedit's value is changed, it sends a message via a socket.
  
  socket is a QtNetwork.QLocalSocket that is not connected to anything. The message "valueSet:VAL" where VAL is some text
  will be sent when the text is changed. See the ksNspire example. """
  def __init__(self,client,text,socket,posX=0.1,posY=0.1,sizeX=0.1,sizeY=0.1,rot=0) :
    ViewItem.__init__(self,client)
    self.client.send("newLineEdit()")
    self.client.send("setPosX("+b2str(posX)+")")
    self.client.send("setPosY("+b2str(posY)+")")
    self.client.send("setGeoX("+b2str(sizeX)+")")
    self.client.send("setGeoY("+b2str(sizeY)+")")
    self.client.send("setText("+b2str(text)+")")
    self.client.send("setRotation("+b2str(rot)+")")
    self.handle=QtCore.QString(self.client.send("endEdit()"))
    self.handle.remove(0,self.handle.indexOf("ing ")+4)
    socket.connectToServer(b2str(client.serverName))
    socket.waitForConnected(300)
    socket.write(b2str("attachTo("+self.handle+")"))
    
  def setText(self,text):
    """ Sets the text of the line edit. """
    self.client.send("beginEdit("+self.handle+")")
    self.client.send("setText("+b2str(text)+")")
    self.client.send("endEdit()")
