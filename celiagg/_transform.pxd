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


cdef extern from "agg_trans_affine.h" namespace "agg":

    cdef cppclass trans_affine:
        double sx
        double shy
        double shx
        double sy
        double tx
        double ty
        trans_affine()

        const trans_affine& reset()
        const trans_affine& multiply(const trans_affine& m)
        const trans_affine& premultiply(const trans_affine& m)
        const trans_affine& multiply_inv(const trans_affine& m)
        const trans_affine& premultiply_inv(const trans_affine& m)
        const trans_affine& invert()
        void transform(double* x, double* y) const
        void inverse_transform(double* x, double* y) const

    cdef cppclass trans_affine_rotation(trans_affine):
        trans_affine_rotation(double a)

    cdef cppclass trans_affine_scaling(trans_affine):
        trans_affine_scaling(double x, double y)

    cdef cppclass trans_affine_translation(trans_affine):
        trans_affine_translation(double x, double y)

    cdef cppclass trans_affine_skewing(trans_affine):
        trans_affine_skewing(double x, double y)
