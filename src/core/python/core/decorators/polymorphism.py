'''
Function decorators for object oriented annotation: abstraction, polymorphism
'''

__docformat__ = 'javadoc en'
__author__ = 'Tor Slettnes'

from typing import Callable

#-------------------------------------------------------------------------------
# @override - indicate overriden function

try:
    ### Python 3.12 or above already has @override
    from typing import override
except ImportError:
    ### For Python 3.11 or earlier, we create a dummy
    def override(function: Callable):
        '''
        Function decorator that indicates an override from the parent class.
        '''
        return function

#-------------------------------------------------------------------------------
# @virtual

def virtual(function: Callable):
    '''
    Function decorator that indicates a function that *may* be overridden in
    subclasses.  This is different from @abstractmethod, which forces override
    (more like "pure virtual" function in C++, Java, etc).
    '''
    return function

