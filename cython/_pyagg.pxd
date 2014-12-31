# The MIT License (MIT)
#
# Copyright (c) 2014 WUSTL ZPLAB
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
# Authors: Erik Hvatum <ice.rikh@gmail.com>

import cython
cimport numpy
import numpy
from libc.stdint cimport uint8_t

cdef extern from "agg_rendering_buffer.h" namespace "agg":
    cdef cppclass row_ptr_cache[T]:
        row_ptr_cache()
        row_ptr_cache(T* buf, unsigned width, unsigned height, int stride)
        void attach(T* buf, unsigned width, unsigned height, int stride)
        T* buf()
#       const T* buf() const
        unsigned width() const
        unsigned height() const
        int stride() const
        unsigned stride_abs() const
        T* row_ptr(int, int y, unsigned)
        T* row_ptr(int y)
        const T* row_ptr(int y) const
#        row_data row(int y) const
#       T const* const* rows() const
#        template<class RenBuf> void copy_from(const RenBuf& src)
        void clear(T value)
