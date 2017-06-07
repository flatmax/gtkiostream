#!/bin/bash
#    copyright Copyright 2013 Matt R. Flax <flatmax@flatmax.org> All Rights Reserved.
#
#    This code has a restricted license, you may not use this code without the permission of Matt Flax.
#
# Author Matt Flax <flatmax@flatmax.org>

# autotools invocation for developers
autoreconf --force --install 2>&1 | sed 's/:/ /'

