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
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
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

cdef class Path:
    cdef _path.path_storage* _this

    def __cinit__(self):
        self._this = new _path.path_storage()

    def __dealloc__(self):
        del self._this

    def close(self):
        self._this.close_polygon()

    def reset(self):
        self._this.remove_all()

    def move_to(self, double x, double y):
        self._this.move_to(x, y)

    def move_rel(self, double dx, double dy):
        self._this.move_rel(dx, dy)

    def line_to(self, double x, double y):
        self._this.line_to(x, y)

    def line_rel(self, double dx, double dy):
        self._this.line_rel(dx, dy)

    def arc_to(self, double rx, double ry, double angle, bool largeArcFlag,
               bool sweepFlag, double x, double y):
        self._this.arc_to(rx, ry, angle, largeArcFlag, sweepFlag, x, y)

    def arc_rel(self, double rx, double ry, double angle, bool largeArcFlag,
                bool sweepFlag, double dx, double dy):
        self._this.arc_rel(rx, ry, angle, largeArcFlag, sweepFlag, dx, dy)

    def quadric_to(self, double xCtrl, double yCtrl, double xTo, double yTo):
        self._this.curve3(xCtrl, yCtrl, xTo, yTo)

    def quadric_rel(self, double dxCtrl, double dyCtrl, double dxTo, double dyTo):
        self._this.curve3_rel(dxCtrl, dyCtrl, dxTo, dyTo)

    def cubic_to(self, double xCtrl1, double yCtrl1, double xCtrl2,
                 double yCtrl2, double xTo, double yTo):
        self._this.curve4(xCtrl1, yCtrl1, xCtrl2, yCtrl2, xTo, yTo)

    def cubic_rel(self, double dxCtrl1, double dyCtrl1, double dxCtrl2,
                  double dyCtrl2, double dxTo, double dyTo):
        self._this.curve4_rel(dxCtrl1, dyCtrl1, dxCtrl2, dyCtrl2, dxTo, dyTo)
