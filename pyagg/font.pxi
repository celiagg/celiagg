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


cpdef enum FontCacheType:
    RasterFontCache = _enums.RasterFontCache
    VectorFontCache = _enums.VectorFontCache 


cdef class Font:
    cdef _font.Font* _this

    def __cinit__(self, fontName, double size, FontCacheType ch):
        """Font(path, height, bold, italic, cache_type)
        path : A unicode string containing the path of a Font file
        height : The size of the font
        cache_type : A FontCacheType
        """
        fontName = _get_utf8_text(fontName,
                                  "Font path must be a unicode string")
        self._this = new _font.Font(fontName, size, ch)

    def __dealloc__(self):
        del self._this

    def copy(self):
        return Font(self._this.filepath(), self._this.height(),
                    self._this.cacheType())

    property cache_type:
        def __get__(self):
            return FontCacheType(self._this.cacheType())

    property filepath:
        def __get__(self):
            return self._this.filepath()

    property height:
        def __get__(self):
            return self._this.height()

    property hinting:
        def __get__(self):
            return self._this.hinting()
        def __set__(self, bool value):
            self._this.hinting(value)

    def width(self, text):
        """width(self, text):
        text : a unicode string
        """
        text = _get_utf8_text(text, "Argument must be a unicode string")
        return self._this.stringWidth(text)
