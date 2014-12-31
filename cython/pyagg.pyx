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
cimport _pyagg

cdef class row_ptr_cache:
    cdef _pyagg.row_ptr_cache[uint8_t]* _this
    cdef uint8_t[:,:,::1] py_buf

    def __cinit__(self, uint8_t[:,:,::1] buf=None):
        if buf is None:
            self.py_buf = None
            self._this = new _pyagg.row_ptr_cache[uint8_t]()
        else:
            # In order to ensure that our backing memory is not deallocated from underneath us, we retain a
            # reference to the memory view supplied to the constructor (buf) as self.py_buf, to be kept in
            # lock step with row_ptr_cache<>'s reference to that memory and destroyed when that reference
            # is lost.
            self.py_buf = buf
            self._this = new _pyagg.row_ptr_cache[uint8_t](&buf[0][0][0], buf.shape[1], buf.shape[0], buf.strides[0])

    def __dealloc__(self):
        del self._this

    def attach(self, uint8_t[:,:,::1] buf=None):
        if buf is None:
            self._this.attach(NULL, 0, 0, 0)
        else:
            self._this.attach(&buf[0][0][0], buf.shape[1], buf.shape[0], buf.strides[0])
        self.py_buf = buf

    def detach(self):
        self.attach(None)

    def width(self):
        return self._this.width()

    def height(self):
        return self._this.height()

    def buf(self):
        # User is not likely to be concerned with the details of the cython memory view around array or buffer
        # supplied to the constructor or attach, but instead expects the original array or buffer (which is why
        # self.py_buf.base is returned rather than self.py_buf).
        if self.py_buf is None:
            return None
        else:
            return self.py_buf.base
