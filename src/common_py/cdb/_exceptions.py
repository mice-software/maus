"CDB Exceptions module."

# pylint: disable=C0103,R0801

class CdbError(Exception):

    """
    The CdbError is the base class for all CDB exceptions.

    """

    def __init__(self, msg, exception=None):
        """Creates an exception. The message is required, but the exception
        is optional."""
        self._msg = msg
        self._exception = exception
        Exception.__init__(self, msg)

    def get_message(self):
        """Return a message for this exception."""
        return self._msg

    def get_exception(self):
        """Return the embedded exception, or None if there was none."""
        return self._exception

    def __str__(self):
        """Create a string representation of the exception."""
        return self._msg


class CdbPermanentError(CdbError):

    """
    The CdbPermanentError exception maybe due to to bad data being passed in or
    an unexpected internal error.

    """

    def __init__(self, msg, exception=None):
        super(CdbPermanentError, self).__init__(msg, exception)


class CdbTemporaryError(CdbError):

    """
    The CdbTemporaryError exception indicates that the problem maybe transient
    and retrying the request MAY succeed. 

    """
    
    def __init__(self, msg, exception=None):
        super(CdbTemporaryError, self).__init__(msg, exception)

