# The MIT License (MIT)
#
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


cdef class FontCache:
    """FontCache()

    An object which manages a render cache of glyphs.
    """
    cdef _font_cache.FontCache* _this

    def __cinit__(self):
        self._this = new _font_cache.FontCache()

    def __dealloc__(self):
        del self._this

    def width(self, Font font, text):
        """width(font, text)
        Measures the width of a string rendered with ``font``.

        :param font: a ``Font`` instance
        :param text: a unicode string
        """
        if not isinstance(font, Font):
            raise TypeError("font must be a Font instance")

        cdef:
            Font fnt = <Font>font

        text = _get_utf8_text(text, "Argument must be a unicode string")

        # Pass an identity transform
        self._this.activate(dereference(fnt._this), _transform.trans_affine())
        return self._this.measure_width(text)
