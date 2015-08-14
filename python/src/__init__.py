"""
    otads --- An OpenTURNS module
    ===================================================

    Contents
    --------
      'otads' is python module for Open TURNS

"""

import sys
if sys.platform.startswith('win'):
    # this ensures OT dll is loaded
    import openturns

from .otads import *

__version__ = '1.0h'
