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

@cython.internal
cdef class FontBase:
    """ Base class for fonts
    """
    cdef _font.Font* _this

    def __dealloc__(self):
        del self._this

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


cdef class FreeTypeFont(FontBase):
    """FreeTypeFont(path, size, face_index=0)

    :param path: A Unicode string containing the path of a font file.
    :param size: The size of the font
    :param face_index: For .ttc fonts, the index of the desired font within
                       the collection.
    """

    def __cinit__(self, path, double size, unsigned face_index=0):
        path = _get_utf8_text(path, "Font path must be a unicode string")
        self._this = new _font.Font(path, size, face_index,
                                    # Ignored argument values
                                    _enums.FontWeight.k_FontWeightAny, False)

    def copy(self):
        return FreeTypeFont(self._this.face_or_path(), self._this.height(),
                            self._this.face_index())

    property face_index:
        def __get__(self):
            return self._this.face_index()

    property filepath:
        def __get__(self):
            return self._this.face_or_path().decode('utf8')



cdef class Win32Font(FontBase):
    """Win32Font(face_name, size, weight=FontWeight.Regular, italic=False)

    :param face_name: A Unicode string containing the the face name of a font
                      known to the system.
    :param size: The size of the font.
    :param weight: A FontWeight value.
    :param italic: If True, pick an italic font.
    """
    def __cinit__(self, face, double size,
                  FontWeight weight=FontWeight.Regular, bool italic=False):
        face = _get_utf8_text(face, "Font face must be a unicode string")
        self._this = new _font.Font(face, size, 0, weight, italic)

    def copy(self):
        return Win32Font(self._this.face_or_path(), self._this.height(),
                         self._this.weight(), self._this.italic())

    property face_name:
        def __get__(self):
            return self._this.face_or_path().decode('utf8')

    property italic:
        def __get__(self):
            return self._this.italic()
        def __set__(self, bool value):
            self._this.italic(value)

    property weight:
        def __get__(self):
            return self._this.weight()
        def __set__(self, FontWeight value):
            self._this.weight(value)
