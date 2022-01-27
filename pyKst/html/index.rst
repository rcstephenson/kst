.. toctree::
   :maxdepth: 2

Welcome to pykst's documentation!
=================================
PyKst is a python interface to kst. With PyKst, scripts can control and share data with a kst session.

Clients
*******************
To interact with a kst session, you must create a client. To open an interface with the last kst session started, or if none open, a new one, try::

	import pykst as kst
	client = kst.Client()

.. automodule:: pykst
   :members: Client
   :exclude-members: getArray, getList, send

Primitives
**********
Scalars, Vectors, Matrices, and Strings (Primitives) can be used in your scripts to share data with kst. All methods are pass-by-value: for example, if you get a value of a scalar and change it, the data inside kst is not changed. You need to explicitly call setValue(). You can get an instance of an existing primitive through the ExistingScalar, ExistingVector, ExistingMatrix, and ExistingString classes. These classes also provide a class method getList() which returns a list of all instances of that type.

Scalars
-------
.. autoclass:: DataSourceScalar
   :members:
   :inherited-members:

.. autoclass:: VectorScalar
   :members:
   :inherited-members:

.. autoclass:: GeneratedScalar
   :members:
   :inherited-members:


Vectors
-------
.. autoclass:: DataVector
   :members:
   :inherited-members:

.. autoclass:: GeneratedVector
   :members:
   :inherited-members:


Matrices
---------
.. autoclass:: DataMatrix
   :members:
   :inherited-members:


Strings
-------
.. autoclass:: DataSourceString
   :members:
   :inherited-members:
   :exclude-members: setValue 

.. autoclass:: GeneratedString
   :members:
   :inherited-members:

Relations
*********
Relations are objects which can be added to a plot.

Curves
------
.. autoclass:: Curve 
   :members:
   :inherited-members:


Annotations
***********
Annotations along with interactive items (see the next section) can be used to turn kst into a graphing calculator, a tetris client, and maybe even a web browser...

.. autoclass:: Label
   :members:
   :inherited-members:
   :exclude-members: set_fill_color, set_fill_style, set_stroke_style, set_stroke_width, set_stroke_brush_color, set_stroke_brush_style, set_stroke_join_style, set_stroke_cap_style, set_fixed_aspect_ratio, set_size
.. autoclass:: Box
   :members:
   :inherited-members:

.. autoclass:: Circle
   :members:
   :inherited-members:
   :exclude-members: set_stroke_join_style, set_stroke_cap_style, set_fixed_aspect_ratio, set_size

.. autoclass:: Ellipse
   :members:
   :inherited-members:
   :exclude-members: set_stroke_join_style, set_stroke_cap_style

.. autoclass:: Line
   :members:
   :inherited-members:
   :exclude-members: set_fill_color, set_fill_style, set_stroke_join_style, set_fixed_aspect_ratio, set_size

.. autoclass:: Arrow
   :members:
   :inherited-members:
   :exclude-members: set_fill_color, set_fill_style, set_stroke_join_style, set_fixed_aspect_ratio, set_size

.. autoclass:: Picture
   :members:
   :inherited-members:
   :exclude-members: set_fill_color, set_fill_style, set_stroke_style, set_stroke_width, set_stroke_brush_color, set_stroke_brush_style, set_stroke_join_style, set_stroke_cap_style

.. autoclass:: SVG
   :members:
   :inherited-members:
   :exclude-members: set_fill_color, set_fill_style, set_stroke_style, set_stroke_width, set_stroke_brush_color, set_stroke_brush_style, set_stroke_join_style, set_stroke_cap_style

.. autoclass:: Plot
   :members:
   :inherited-members:

Interactive items
*****************
Interactive items are controls which are part of a kst view and use QtNetwork.QLocalSocket to notify the script when the user has interacted with them. They can only be created through scripts.

.. autoclass:: Button
   :members:
   :inherited-members:

.. autoclass:: LineEdit
   :members:
   :inherited-members:
