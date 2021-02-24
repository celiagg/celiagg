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
    """Font(path, size, cache_type, face_index=0)

    :param path: A Unicode string containing the path of a ``Font`` file
    :param size: The size of the font
    :param cache_type: A ``FontCacheType``
    :param face_index: For .ttc fonts, the index of the desired font within
                        the collection.
    """
    cdef _font.Font* _this

    def __cinit__(self, fontName, double size, FontCacheType ch,
                    unsigned face_index=0):
        fontName = _get_utf8_text(fontName,
                                  "Font path must be a unicode string")
        self._this = new _font.Font(fontName, size, ch, face_index)

    def __dealloc__(self):
        del self._this

    def copy(self):
        return Font(self._this.filepath(), self._this.height(),
                    self._this.cache_type(), self._this.face_index())

    property cache_type:
        def __get__(self):
            return FontCacheType(self._this.cache_type())

    property face_index:
        def __get__(self):
            return self._this.face_index()

    property filepath:
        def __get__(self):
            return self._this.filepath()

    property flip:
        def __get__(self):
            return self._this.flip()
        def __set__(self, bool value):
            self._this.flip(value)

    property height:
        def __get__(self):
            return self._this.height()
        def __set__(self, double value):
            self._this.height(value)

    property hinting:
        def __get__(self):
            return self._this.hinting()
        def __set__(self, bool value):
            self._this.hinting(value)
