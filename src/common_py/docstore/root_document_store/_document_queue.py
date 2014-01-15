#!/usr/bin/env python

#  This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
# 
#  MAUS is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
# 
#  MAUS is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
# 
#  You should have received a copy of the GNU General Public License
#  along with MAUS.  If not, see <http://www.gnu.org/licenses/>.

"""
Server-side ROOT document store
"""

import os

from docstore.root_document_store import ServerSocket

class DocumentQueue:
    def __init__(self, poll_frequency, port, timeout, retry_time):
        self.server_socket = ServerSocket(poll_frequency, [poll], timeout, retry_time)
        

    tmp = os.path.join(os.path.expandvars("MAUS_TMP_DIR"), "root_docstore")
