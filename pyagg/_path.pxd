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

from libcpp cimport bool


cdef extern from "agg_path_storage.h" namespace "agg":
    cdef cppclass path_storage:
        path_storage()

        void remove_all()
        void free_all()
        unsigned start_new_path()
        unsigned total_vertices() const
        unsigned vertex(unsigned idx, double* x, double* y) const
        void move_to(double x, double y)
        void move_rel(double dx, double dy)
        void line_to(double x, double y)
        void line_rel(double dx, double dy)
        void arc_to(double rx, double ry, double angle, bool large_arc_flag,
                    bool sweep_flag, double x, double y)
        void arc_rel(double rx, double ry, double angle, bool large_arc_flag,
                     bool sweep_flag, double dx, double dy)
        void curve3(double x_ctrl, double y_ctrl, double x_to, double y_to)
        void curve3_rel(double dx_ctrl, double dy_ctrl, double dx_to, double dy_to)
        void curve4(double x_ctrl1, double y_ctrl1, double x_ctrl2,
                    double y_ctrl2,  double x_to, double y_to)
        void curve4_rel(double dx_ctrl1, double dy_ctrl1, double dx_ctrl2,
                        double dy_ctrl2, double dx_to, double dy_to)
        void close_polygon()


cdef extern from "path.h":
    cdef void add_ellipse_to_path(path_storage& path,
                                  const double x, const double y,
                                  const double rx, const double ry)
