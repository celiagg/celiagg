# The MIT License (MIT)
#
# Copyright (c) 2016 WUSTL ZPLAB
# Copyright (c) 2016-2021 Celiagg Contributors
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#
# Authors: John Wiggins


cdef class Transform:
    """Transform(sx=1.0, shy=0.0, shx=0.0, sy=1.0, tx=0.0, ty=0.0)
    A 2D affine transform.

    After construction, the same names can be used as get/set properties for
    individual components of the transform.

    NOTE: If you're familar with SVG, you'll notice the format of this
    constructor. The arguments are exactly the same as the SVG `matrix`
    attribute.

    :param sx: Scale X
    :param shy: Shear/Skew Y
    :param shx: Shear/Skew X
    :param sy: Scale Y
    :param tx: Translation X
    :param ty: Translation Y
    """
    cdef _transform.trans_affine* _this

    def __init__(self, double sx=1.0, double shy=0.0, double shx=0.0,
                 double sy=1.0, double tx=0.0, double ty=0.0):
        self._this.sx = sx
        self._this.sy = sy
        self._this.shx = shx
        self._this.shy = shy
        self._this.tx = tx
        self._this.ty = ty

    def __cinit__(self):
        self._this = new _transform.trans_affine()

    def __dealloc__(self):
        del self._this

    def __repr__(self):
        name = type(self).__name__
        return "{}({}, {}, {}, {}, {}, {})".format(
            name, self._this.sx, self._this.shy, self._this.shx, self._this.sy,
            self._this.tx, self._this.ty
        )

    property sx:
        def __get__(self):
            return self._this.sx

    property sy:
        def __get__(self):
            return self._this.sy

    property shx:
        def __get__(self):
            return self._this.shx

    property shy:
        def __get__(self):
            return self._this.shy

    property tx:
        def __get__(self):
            return self._this.tx

    property ty:
        def __get__(self):
            return self._this.ty

    cdef _assign_obj(self, _transform.trans_affine trans):
        self._this.reset()
        self._this.multiply(trans)

    def copy(self):
        """copy()
        Returns a deep copy of the object.
        """
        return Transform(self._this.sx, self._this.shy, self._this.shx,
                         self._this.sy, self._this.tx, self._this.ty)

    def reset(self):
        """reset()
        Resets to the identity transform.
        """
        self._this.reset()

    def invert(self):
        """invert()
        Inverts the matrix.
        """
        self._this.invert()

    def scale(self, double sx, double sy):
        """scale(x, y)
        Apply a scaling to the transform

        :param x: The scale factor in X
        :param y: The scale factor in Y
        """
        self._this.premultiply(_transform.trans_affine_scaling(sx, sy))

    def rotate(self, double angle):
        """rotate(angle)
        Apply a rotation to the transform.

        :param angle: The desired rotation in radians.
        """
        self._this.premultiply(_transform.trans_affine_rotation(angle))

    def skew(self, double sx, double sy):
        """skew(x, y)
        Apply a skew to the transform.

        :param x: The skew factor in X
        :param y: The skew factor in Y
        """
        self._this.premultiply(_transform.trans_affine_skewing(sx, sy))

    def translate(self, double x, double y):
        """translate(x, y)
        Apply a translation to the transform.

        :param x: The translation in X
        :param y: The translation in Y
        """
        self._this.premultiply(_transform.trans_affine_translation(x, y))

    def premultiply(self, Transform other):
        """premultiply(other)
        Pre-multiply this transform with another transform.

        :param other: A ``Transform`` instance to premultiply
        """
        self._this.premultiply(dereference(other._this))

    def multiply(self, Transform other):
        """multiply(other)
        Post-multiply this transform with another transform.

        :param other: A ``Transform`` instance to multiply
        """
        self._this.multiply(dereference(other._this))

    def worldToScreen(self, double x, double y):
        """worldToScreen(x, y)
        Transforms a point from world space to screen space.

        :param x: The X coordinate of the point to transform.
        :param y: The Y coordinate of the point to transform.
        :return: An (x, y) tuple containing the transformed point.
        """
        self._this.transform(&x, &y)
        return (x, y)

    def screenToWorld(self, double x, double y):
        """screenToWorld(x, y)
        Transforms a point from screen space to world space.

        :param x: The X coordinate of the point to transform.
        :param y: The Y coordinate of the point to transform.
        :return: An (x, y) tuple containing the transformed point.
        """
        self._this.inverse_transform(&x, &y)
        return (x, y)
