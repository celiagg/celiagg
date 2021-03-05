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
cimport _enums


cdef extern from "font.h":
    cdef cppclass Font:
        Font(char* fileName, double height, unsigned face_index, _enums.FontWeight weight, bool italic)

        unsigned face_index() const
        const char* face_or_path() const
        bool flip() const
        void flip(bool flip)
        double height() const
        void height(double height)
        bool hinting() const
        void hinting(bool hint)
        bool italic() const
        void italic(bool italic)
        _enums.FontWeight weight() const
        void weight(_enums.FontWeight weight)
