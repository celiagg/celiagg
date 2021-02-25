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

from libcpp cimport bool


cdef extern from "vertex_source.h":

    cdef cppclass VertexSource:
        void rewind(unsigned path_id)
        unsigned vertex(double* x, double* y)
        unsigned total_vertices()

    cdef cppclass BsplineSource:
        BsplineSource(const double* points,
                      const size_t point_count)

    cdef cppclass PathSource:
        PathSource()

        void begin()
        void close()
        void reset()
        unsigned last_vertex(double* x, double* y)
        void concat_path[T](T& vs, unsigned path_id)

        void move_to(double x, double y)
        void line_to(double x, double y)
        void arc_to(double x1, double y1, double x2, double y2, double radius)
        void quadric_to(double x_ctrl, double y_ctrl, double x_to, double y_to)
        void cubic_to(double x_ctrl1, double y_ctrl1, double x_ctrl2,
                      double y_ctrl2,  double x_to, double y_to)

        void arc(double x, double y, double radius,
                 double start_angle, double end_angle, bool cw)
        void ellipse(double cx, double cy, double rx, double ry)

    cdef cppclass RepeatedSource:
        RepeatedSource(VertexSource& source,
                       const double* points,
                       const size_t point_count)
