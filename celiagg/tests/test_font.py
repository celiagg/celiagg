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
import sys
import unittest

import celiagg as agg

is_windows = sys.platform in ('win32', 'cygwin')


class TestFont(unittest.TestCase):
    @unittest.skipIf(is_windows, "Don't test FreeTypeFont on Windows")
    def test_freetype_font(self):
        with agg.example_font() as path:
            # Test default arguments
            font = agg.Font(path, 12.0)
            self.assertEqual(font.filepath, path)
            self.assertEqual(font.height, 12.0)
            self.assertEqual(font.face_index, 0)

            # Then optional
            font = agg.Font(path, 12.0, face_index=42)
            self.assertEqual(font.face_index, 42)

    @unittest.skipIf(not is_windows, "Don't test Win32Font on other platforms")
    def test_win32_font(self):
        with agg.example_font() as face_name:
            # Test default arguments
            font = agg.Font(face_name, 12.0)
            self.assertEqual(font.face_name, face_name)
            self.assertEqual(font.height, 12.0)
            self.assertEqual(font.weight, agg.FontWeight.Regular)
            self.assertFalse(font.italic)

            # Then optional
            font = agg.Font(face_name, 12.0, agg.FontWeight.ExtraBold, True)
            self.assertEqual(font.weight, agg.FontWeight.ExtraBold)
            self.assertTrue(font.italic)
