#!/usr/bin/env python
# -*- coding: utf-8 -*-

from opencc import OpenCC

__doc__ = 'OpenCC Python binding'
__version__ = '0.4.2'

__all__ = [OpenCC, ]

if __name__ == '__main__':
    a = OpenCC('zhs2zht.ini')
    print(a.convert('開放中文轉換'))

    a = OpenCC('zht2zhs.ini')
    print(a.convert('開放中文轉換'))
