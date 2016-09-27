# The MIT License (MIT)
#
# Copyright (c) 2016 WUSTL ZPLAB
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

    cdef _assign_obj(self, _transform.trans_affine trans):
        self._this.reset()
        self._this.multiply(trans)

    def reset(self):
        self._this.reset()

    def invert(self):
        self._this.invert()

    def scale(self, double sx, double sy):
        self._this.scale(sx, sy)

    def rotate(self, double angle):
        self._this.rotate(angle)

    def skew(self, double sx, double sy):
        self._this.multiply(_transform.trans_affine_skewing(sx, sy))

    def translate(self, double x, double y):
        self._this.translate(x, y)

    def worldToScreen(self, double x, double y):
        self._this.transform(&x, &y)
        return (x, y)

    def screenToWorld(self, double x, double y):
        self._this.inverse_transform(&x, &y)
        return (x, y)
