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

    def __iter__(self):
        for pt in self.vertices():
            yield pt

    def begin(self):
        return self._this.start_new_path()

    def close(self):
        self._this.close_polygon()

    def reset(self):
        """ Clears the path.
        """
        self._this.remove_all()

    def vertices(self):
        """ Returns all the vertices in the path as a numpy array.
        """
        cdef unsigned count = self._this.total_vertices()
        cdef double[:,::1] points = numpy.empty((count, 2),
                                                dtype=numpy.float64,
                                                order='c')
        for i in range(0, count):
            self._this.vertex(<unsigned>i, &points[i][0], &points[i][1])

        return points.base

    def move_to(self, double x, double y):
        """ Moves the current position of the path.
            x, y: (x, y) coordinate of the new current position
        """
        self._this.move_to(x, y)

    def line_to(self, double x, double y):
        """ Adds a line segment to the path from the current position to the
        given position.
            x, y: (x, y) coordinate of the line end point
        """
        self._this.line_to(x, y)

    def arc_to(self, double rx, double ry, double angle, bool large_arc_flag,
               bool sweep_flag, double x, double y):
        self._this.arc_to(rx, ry, angle, large_arc_flag, sweep_flag, x, y)

    def quadric_to(self, double x0, double y0, double x_ctrl, double y_ctrl,
                   double x_to, double y_to):
        """ Adds a quadratic Bezier curve to the path.
          x0, y0: Start point
          x_ctrl, y_ctrl: Control point
          x_to, y_to: End point
        """
        self._this.move_to(x0, y0)
        self._this.curve3(x_ctrl, y_ctrl, x_to, y_to)

    def cubic_to(self, double x0, double y0, double x_ctrl1, double y_ctrl1,
                 double x_ctrl2, double y_ctrl2, double x_to, double y_to):
        """ Adds a cubic Bezier curve to the path.
          x0, y0: Start point
          x_ctrl1, y_ctrl1: First control point
          x_ctrl2, y_ctrl2: Second control point
          x_to, y_to: End point
        """
        self._this.move_to(x0, y0)
        self._this.curve4(x_ctrl1, y_ctrl1, x_ctrl2, y_ctrl2, x_to, y_to)

    def ellipse(self, double cx, double cy, double rx, double ry):
        """ Adds an ellipse to the path.
            cx, cy: Center (x, y) coordinate of the ellipse
            rx, ry: Ellipse radii in x and y dimensions. 
        """
        _path.add_ellipse_to_path(dereference(self._this), cx, cy, rx, ry)

    def rect(self, double x, double y, double width, double height):
        """ Adds a rectangle to the path.
            x, y: Rectangle position
            width, height: Rectangle width and height
        """
        cdef _path.path_storage* pth = self._this

        pth.move_to(x, y)
        pth.line_to(x + width, y)
        pth.line_to(x + width, y + height)
        pth.line_to(x, y + height)
        pth.close_polygon()

    @cython.boundscheck(False)
    @cython.wraparound(False)
    def rects(self, rects):
        """ Adds multiple rectangles to the path.
            rects: 2D array of (x, y, w, h) sequences
        """
        cdef double[:,::1] rects_npy = numpy.asarray(rects,
                                                     dtype=numpy.float64,
                                                     order='c')
        cdef int count = rects_npy.shape[0]
        cdef _path.path_storage* pth = self._this
        cdef double x, y, w, h

        if rects_npy.shape[1] != 4:
            msg = "rects argument must be an iterable of (x, y, w, h) tuples."
            raise ValueError(msg)

        for i in range(0, count):
            x = rects_npy[i, 0]
            y = rects_npy[i, 1]
            w = rects_npy[i, 2]
            h = rects_npy[i, 3]

            pth.move_to(x, y)
            pth.line_to(x + w, y)
            pth.line_to(x + w, y + h)
            pth.line_to(x, y + h)
            pth.close_polygon()

    @cython.boundscheck(False)
    @cython.wraparound(False)
    def lines(self, points):
        """ Adds multiple connected line segmests to the path.
            points: 2D array of (x, y) pairs
        """
        cdef double[:,::1] points_npy = numpy.asarray(points,
                                                      dtype=numpy.float64,
                                                      order='c')
        cdef int count = points_npy.shape[0]
        cdef _path.path_storage* pth = self._this

        if points_npy.shape[1] != 2:
            msg = "points argument must be an iterable of (x, y) tuples."
            raise ValueError(msg)

        pth.move_to(points_npy[0, 0], points_npy[0, 1])
        for i in range(1, count):
            pth.line_to(points_npy[i, 0], points_npy[i, 1])
        pth.close_polygon()

    @cython.boundscheck(False)
    @cython.wraparound(False)
    def lines_set(self, starts, ends):
        """ Adds multiple disconnected line segments to the path.
            points: 2D array of (x, y) pairs
        """
        cdef double[:,::1] starts_npy = numpy.asarray(starts,
                                                      dtype=numpy.float64,
                                                      order='c')
        cdef double[:,::1] ends_npy = numpy.asarray(ends, dtype=numpy.float64,
                                                    order='c')
        cdef int starts_count = starts_npy.shape[0]
        cdef int ends_count = ends_npy.shape[0]
        cdef _path.path_storage* pth = self._this

        if starts_npy.shape[1] != 2:
            msg = "starts argument must be an iterable of (x, y) tuples."
            raise ValueError(msg)
        if ends_npy.shape[1] != 2:
            msg = "ends argument must be an iterable of (x, y) tuples."
            raise ValueError(msg)
        if starts_count != ends_count:
            msg = "starts argument must match length of ends argument."
            raise ValueError(msg)

        for i in range(0, starts_count):
            pth.move_to(starts_npy[i, 0], starts_npy[i, 1])
            pth.line_to(ends_npy[i, 0], ends_npy[i, 1])
            pth.close_polygon()
