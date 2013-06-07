#!/usr/bin/env python
# -*- coding: utf-8 -*-

import unittest
from opencc import OpenCC

class OpenCCTest(unittest.TestCase):
    def test_base_zhs2zht(self):
        c = OpenCC('zhs2zht.ini')
        self.assertEqual(c.convert('开放中文转换'), '開放中文轉換')
        c.close()

    def test_unicode_zhs2zht(self):
        c = OpenCC('zhs2zht.ini')
        self.assertEqual(c.convert(u'开放中文转换'), u'開放中文轉換')
        c.close()

    def test_base_zht2zhs(self):
        c = OpenCC('zht2zhs.ini')
        self.assertEqual(c.convert('開放中文轉換'), '开放中文转换')
        c.close()

    def test_unicode_zht2zhs(self):
        c = OpenCC('zht2zhs.ini')
        self.assertEqual(c.convert(u'开放中文转换'), u'開放中文轉換')
        c.close()

    def test_conversion_mode(self):
        c = OpenCC('zhs2zht.ini')
        c.set_conversion_mode(0)

        try:
            c.set_conversion_mode(3)
        except ValueError, e:
            self.assertEqual(e.message, 'ValueError: conversion mode must be in [0,1,2].')
        finally:
            c.close()

    def test_convert_text(self):
        c = OpenCC('zhs2zht.ini')
        try:
            c.convert(3)
        except TypeError, e:
            self.assertEqual(e.message, 'TypeError: must be string or buffer.')
        finally:
            c.close()

if __name__ == '__main__':
    unittest.main()

