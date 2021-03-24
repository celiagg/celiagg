# The MIT License (MIT)
#
# Copyright (c) 2016 WUSTL ZPLAB
# Copyright (c) 2016-2023 Celiagg Contributors
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

cdef class VertexSource:
    """An object which supplies vertex pairs and other information to
    low-level drawing routines.

    NOTE: This class should not be explicitly instantiated.
    """
    cdef _vertex_source.VertexSource* _this

    def __dealloc__(self):
        del self._this

    def __iter__(self):
        for pt in self.vertices():
            yield pt

    def bounding_rect(self):
        """Returns a bounding rectangle for the vertices of the source in the
        format: (x, y, w, h)
        """
        if self.length() == 0:
            return (0, 0, 0, 0)

        verts = self.vertices()
        xs = verts[:, 0]
        ys = verts[:, 1]
        xmin, xmax = xs.min(), xs.max()
        ymin, ymax = ys.min(), ys.max()
        return (xmin, ymin, xmax - xmin, ymax - ymin)

    def copy(self):
        """Returns a deep copy of the object.
        """
        raise NotImplementedError

    def final_point(self):
        """Returns the last vertex that will be returned by the source.
        """
        raise NotImplementedError

    def length(self):
        """Returns the number of vertices returned by ``vertices()``
        """
        return self._this.total_vertices()

    def vertices(self):
        """Returns all the vertices in the source as a numpy array.
        """
        cdef unsigned count = self._this.total_vertices()
        cdef double[:,::1] points = numpy.empty((count, 2), dtype=numpy.float64,
                                                order='c')

        self._this.rewind(0)
        for i in range(0, count):
            self._this.vertex(&points[i][0], &points[i][1])

        return points.base


cdef class BSpline(VertexSource):
    """BSpline(points)
    A B-Spline curve.

    :param points: 2D array of (x, y) pairs
    """
    cdef object _points

    def __cinit__(self, points):
        cdef double[:,::1] _points = numpy.asarray(points, dtype=numpy.float64,
                                                   order='c')

        if _points.shape[1] != 2:
            msg = 'Points argument must be an iterable of (x, y) pairs.'
            raise ValueError(msg)

        self._this = <_vertex_source.VertexSource*> new _vertex_source.BsplineSource(
            &_points[0][0], _points.shape[0]
        )
        # hold a reference to the backing ndarray
        self._points = _points

    def copy(self):
        """Returns a deep copy of the object.
        """
        points = self._points.copy()
        return BSpline(points)

    def final_point(self):
        """Returns the last vertex that will be returned by the source.
        """
        return self._points[-1]


cdef class Path(VertexSource):
    """A path object.
    """

    def __cinit__(self):
        self._this = <_vertex_source.VertexSource*> new _vertex_source.PathSource()

    def copy(self):
        """Returns a deep copy of the object.
        """
        cdef:
            Path cpy = Path()
            _vertex_source.PathSource* ths = <_vertex_source.PathSource*>self._this
            _vertex_source.PathSource* other = <_vertex_source.PathSource*>cpy._this

        other.concat_path[_vertex_source.PathSource](dereference(ths))
        return cpy

    def final_point(self):
        """Returns the last vertex that will be returned by the source.
        """
        cdef:
            _vertex_source.PathSource* pth = <_vertex_source.PathSource*>self._this
            double x, y
        pth.last_vertex(&x, &y)
        return (x, y)

    def begin(self):
        """Begins a new subpath.
        """
        cdef _vertex_source.PathSource* pth = <_vertex_source.PathSource*>self._this
        pth.begin()

    def close(self):
        """Closes the current subpath.
        """
        cdef _vertex_source.PathSource* pth = <_vertex_source.PathSource*>self._this
        pth.close()

    def reset(self):
        """Clears the path.
        """
        cdef _vertex_source.PathSource* pth = <_vertex_source.PathSource*>self._this
        pth.reset()

    def add_path(self, Path other):
        """add_path(other)
        Add another path to this path.

        :param other: A ``Path`` object which will be concatenated onto the end
                      of this path.
        """
        cdef _vertex_source.PathSource* pth = <_vertex_source.PathSource*>self._this
        cdef _vertex_source.PathSource* othr = <_vertex_source.PathSource*>other._this
        pth.concat_path[_vertex_source.PathSource](dereference(othr))

    def move_to(self, double x, double y):
        """move_to(x, y)
        Moves the current position of the path.

        :param x: X coordinate of the new current position
        :param y: Y coordinate of the new current position
        """
        cdef _vertex_source.PathSource* pth = <_vertex_source.PathSource*>self._this
        pth.move_to(x, y)

    def line_to(self, double x, double y):
        """line_to(x, y)
        Adds a line segment to the path from the current position to the given
        position.

        :param x: X coordinate of the line end point
        :param y: Y coordinate of the line end point
        """
        cdef _vertex_source.PathSource* pth = <_vertex_source.PathSource*>self._this
        pth.line_to(x, y)

    def arc_to(self, double x1, double y1, double x2, double y2, double radius):
        """arc_to(x1, y1, x2, y2, radius)
        Adds an arc to the path connecting two points

        :param x1: The starting X point of the arc
        :param y1: The starting Y point of the arc
        :param x2: The end X point of the arc
        :param y2: The end Y point of the arc
        :param radius: The radius of the arc
        """
        cdef _vertex_source.PathSource* pth = <_vertex_source.PathSource*>self._this
        pth.arc_to(x1, y1, x2, y2, radius)

    def quadric_to(self, double x_ctrl, double y_ctrl, double x_to, double y_to):
        """Adds a quadratic Bezier curve to the path, starting from the
        current position.

        :param x_ctrl: Control point X
        :param y_ctrl: Control point Y
        :param x_to: End point X
        :param y_to: End point Y
        """
        cdef _vertex_source.PathSource* pth = <_vertex_source.PathSource*>self._this
        pth.quadric_to(x_ctrl, y_ctrl, x_to, y_to)

    def cubic_to(self, double x_ctrl1, double y_ctrl1,
                 double x_ctrl2, double y_ctrl2, double x_to, double y_to):
        """cubic_to(x_ctrl1, y_ctrl1, x_ctrl2, y_ctrl2, x_to, y_to)
        Adds a cubic Bezier curve to the path, starting from the current pen
        position.

        :param x_ctrl1: First control point X
        :param y_ctrl1: First control point Y
        :param x_ctrl2: Second control point X
        :param y_ctrl2: Second control point Y
        :param x_to: End point X
        :param y_to: End point Y
        """
        cdef _vertex_source.PathSource* pth = <_vertex_source.PathSource*>self._this
        pth.cubic_to(x_ctrl1, y_ctrl1, x_ctrl2, y_ctrl2, x_to, y_to)

    def arc(self, double x, double y, double radius, double start_angle,
            double end_angle, bool clockwise):
        """arc(x, y, radius, start_angle, end_angle, clockwise)
        Adds an arc to the path.

        :param x: Center X coordinate of the arc
        :param y: Center Y coordinate of the arc
        :param radius: Radius of the arc
        :param start_angle: Starting angle, in radians
        :param end_angle: Ending angle, in radians
        :param clockwise: If True, the arc is drawn in a clockwise direction
        """
        cdef _vertex_source.PathSource* pth = <_vertex_source.PathSource*>self._this
        pth.arc(x, y, radius, start_angle, end_angle, clockwise)

    def ellipse(self, double cx, double cy, double rx, double ry):
        """ellipse(cx, cy, rx, ry)
        Adds an ellipse to the path.

        :param cx: Center X coordinate of the ellipse
        :param cy: Center Y coordinate of the ellipse
        :param rx: Ellipse radii x dimension.
        :param ry: Ellipse radii y dimension.
        """
        cdef _vertex_source.PathSource* pth = <_vertex_source.PathSource*>self._this
        pth.ellipse(cx, cy, rx, ry)

    def rect(self, double x, double y, double width, double height):
        """rect(x, y, width, height)
        Adds a rectangle to the path.

        :param x: Rectangle X position
        :param y: Rectangle Y position
        :param width: Rectangle widtht
        :param height: Rectangle height
        """
        cdef _vertex_source.PathSource* pth = <_vertex_source.PathSource*>self._this

        pth.move_to(x, y)
        pth.line_to(x + width, y)
        pth.line_to(x + width, y + height)
        pth.line_to(x, y + height)
        pth.close()

    @cython.boundscheck(False)
    @cython.wraparound(False)
    def rects(self, rects):
        """rects(rects)
        Adds multiple rectangles to the path.

        :param rects: 2D array of (x, y, w, h) sequences
        """
        cdef _vertex_source.PathSource* pth = <_vertex_source.PathSource*>self._this
        cdef double[:,::1] _rects = numpy.asarray(rects, dtype=numpy.float64,
                                                  order='c')
        cdef int count = _rects.shape[0]
        cdef double x, y, w, h

        if _rects.shape[1] != 4:
            msg = "rects argument must be an iterable of (x, y, w, h) tuples."
            raise ValueError(msg)

        for i in range(0, count):
            x = _rects[i, 0]
            y = _rects[i, 1]
            w = _rects[i, 2]
            h = _rects[i, 3]

            pth.move_to(x, y)
            pth.line_to(x + w, y)
            pth.line_to(x + w, y + h)
            pth.line_to(x, y + h)
            pth.close()

    @cython.boundscheck(False)
    @cython.wraparound(False)
    def lines(self, points):
        """lines(points)
        Adds multiple connected line segmests to the path.

        :param points: 2D array of (x, y) pairs
        """
        cdef _vertex_source.PathSource* pth = <_vertex_source.PathSource*>self._this
        cdef double[:,::1] _points = numpy.asarray(points,
                                                      dtype=numpy.float64,
                                                      order='c')
        cdef int count = _points.shape[0]

        if _points.shape[1] != 2:
            msg = "points argument must be an iterable of (x, y) tuples."
            raise ValueError(msg)

        pth.move_to(_points[0, 0], _points[0, 1])
        for i in range(1, count):
            pth.line_to(_points[i, 0], _points[i, 1])

    @cython.boundscheck(False)
    @cython.wraparound(False)
    def lines_set(self, starts, ends):
        """lines_set(starts, ends)
        Adds multiple disconnected line segments to the path.

        :param starts: 2D array of (x, y) pairs
        :param ends: 2D array of (x, y) pairs
        """
        cdef _vertex_source.PathSource* pth = <_vertex_source.PathSource*>self._this
        cdef double[:,::1] _starts = numpy.asarray(starts, dtype=numpy.float64,
                                                   order='c')
        cdef double[:,::1] _ends = numpy.asarray(ends, dtype=numpy.float64,
                                                 order='c')
        cdef int starts_count = _starts.shape[0]
        cdef int ends_count = _ends.shape[0]

        if _starts.shape[1] != 2:
            msg = "starts argument must be an iterable of (x, y) tuples."
            raise ValueError(msg)
        if _ends.shape[1] != 2:
            msg = "ends argument must be an iterable of (x, y) tuples."
            raise ValueError(msg)
        if starts_count != ends_count:
            msg = "starts argument must match length of ends argument."
            raise ValueError(msg)

        for i in range(0, starts_count):
            pth.move_to(_starts[i, 0], _starts[i, 1])
            pth.line_to(_ends[i, 0], _ends[i, 1])


cdef class ShapeAtPoints(VertexSource):
    """ShapeAtPoints(source, points)
    Replicates a shape at mutiple points.

    :param source: A ``VertexSource`` object (``BSpline``, ``Path``, etc.)
    :param points: A sequence of (x, y) pairs where the shape defined by
                   ``source`` should be drawn.
    """
    cdef VertexSource _sub_source
    cdef object _points

    def __cinit__(self, source, points):
        if not isinstance(source, VertexSource):
            raise TypeError("source must be a VertexSource instance")

        cdef VertexSource vs = <VertexSource>source
        cdef double[:,::1] _points = numpy.asarray(points, dtype=numpy.float64,
                                                   order='c')

        if _points.shape[1] != 2:
            msg = 'Points argument must be an iterable of (x, y) pairs.'
            raise ValueError(msg)

        self._this = <_vertex_source.VertexSource*> new _vertex_source.RepeatedSource(
            dereference(vs._this), &_points[0][0], _points.shape[0]
        )
        # Keep references for safety
        self._sub_source = vs
        self._points = _points

    def copy(self):
        """Returns a deep copy of the object.
        """
        source = self._sub_source.copy()
        points = self._points.copy()
        return ShapeAtPoints(source, points)

    def final_point(self):
        """Returns the last vertex that will be returned by the source.
        """
        x, y = self._sub_source.last_vertex()
        tx, ty = self._points[-1]
        return (x + tx, y + ty)
