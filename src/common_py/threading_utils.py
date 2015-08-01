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
Some utilities for threading
"""

import Queue
import threading
import copy

class ThreadedBool:
    """
    Thread-safe boolean flag

    ThreadedBool uses a threading.Lock object to determine if it is true or not.
    If the lock is acquired, bool is considered true; if it has been released,
    it is considered false.
    """
    def __init__(self, value):
        """
        Initialise a new ThreadedBool

        @param value - set initial state based on whether value evaluates to
                       true or false
        @returns a ThreadedBool
        """
        self._lock = threading.Lock()
        if value:
            self.set_true()
        else:
            self.set_false()

    def __nonzero__(self):
        """
        Boolean operator for ThreadedBool

        @returns True if bool is set to True, else False
        """
        # returns True if acquired; else False
        is_true = not self._lock.acquire(False)
        if not is_true: # it wasn't acquired, better return it to original state
            self._lock.release()
        return is_true

    def set_true(self):
        """Set internal value to True"""
        self._lock.acquire(False)

    def set_false(self):
        """Set internal value to False"""
        try:
            self._lock.release()
        except threading.ThreadError: # was already released
            pass       

class ThreadedValue:
    """
    Thread-safe value

    ThreadedValue uses a queue object to store the value, which is thread safe.
    Values are always returned by value
    """
    def __init__(self, value):
        """
        Initialise a new ThreadedBool

        @param value - set initial value
        @returns a ThreadedValue
        """
        self._queue = Queue.Queue()
        self._queue.put_nowait(copy.deepcopy(value))

    def get_value(self):
        """
        Thread-safe get the value stored
        """
        value = self._queue.get()
        value_cpy = copy.deepcopy(value)
        self._queue.put(value)
        return value_cpy

    def set_value(self, value):
        """
        Thread-safe put the value stored
        """
        self._queue.put(value) # put the new value on
        self._queue.get() # then remove the old value

def generate_fifo_queue(queue):
    """
    Generator for looping over a fifo queue.

    Yield elements from a fifo queue, and replace them on the queue once they
    have been used

    raises StopIteration if the Queue is empty or we have looped once over all
    elements

    Note that this will put the first element in the queue to the end

    e.g. 
    for item in generate_fifo_queue(queue):
        print item
    will print all items in the queue
    """
    for item in yield_fifo_queue(queue):
        yield item
        queue.put_nowait(item)


def yield_fifo_queue(queue):
    """
    Yield elements of a fifo queue, but don't put them back when done

    Yield elements from a fifo queue, and replace them on the queue once they
    have been used

    raises StopIteration if the Queue is empty or we have already looped over
    any elements in the queue (e.g. if caller puts some elements back on the
    queue). 
    
    Note that after iterating over the queue, the first element in the queue
    will become the last element (sorry)

    e.g.
    for item in yield_fifo_queue(queue):
        if item == "tasty":
            queue.put(item)
    will loop over the queue and remove items that are not "tasty"
    """
    item_list = []
    while True:
        try:
            item = queue.get_nowait()
        except Queue.Empty:
            raise StopIteration("No items in queue")
        if item in item_list: # we have looped once
            queue.put_nowait(item)
            raise StopIteration("Looped over the queue")
        yield item
        item_list.append(item)

def go_back_fifo_queue(queue):
    """
    Cycle through a fifo queue until the last-but-one is on the top
    """
    item_list = []
    for item in generate_fifo_queue(queue):
        item_list.append(item)
    if len(item_list) < 2:
        return
    for item in yield_fifo_queue(queue):
        try:
            if item == item_list[-2]:
                break
        except Exception:
            raise
        finally:
            queue.put_nowait(item)

