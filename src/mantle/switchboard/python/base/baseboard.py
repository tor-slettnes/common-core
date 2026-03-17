'''
Abstract switchboard implementation.
'''

__all__ = ['SwitchboardBase']
__docformat__ = 'javadoc en'
__author__ = 'Tor Slettnes'

### Standard Python modules
from abc import abstractmethod
from typing import Callable, Sequence, Mapping
from threading import Lock
from logging import Logger
import os

### Core modules
from cc.core.paths import FilePathInput
from cc.core.decorators import virtual
from cc.core.invocation import method_path
from cc.core.settingsstore import SettingsStore
from cc.protobuf.dissecter import dissecter
from cc.protobuf.signal import SignalStore, MappingAction
from cc.protobuf.variant import PyValueMap

### Modules within package
from ..protobuf import (
    Signal, Specification, Status,
    InterceptorSpec, InterceptorMethod, InterceptorPhase,
    InterceptorInvocation, InterceptorResult,
    ExceptionHandling, InvocationStyle,
    State, StateMask, StateSet, encodeStateSet,
    SwitchSelectionInput,
)

from .switch import Switch
from .observer import SwitchboardObserver


class SwitchboardBase (SwitchboardObserver):
    '''
    Switchboard abstract base
    '''

    SPEC_SIGNAL = 'specification'
    STATUS_SIGNAL = 'status'
    initialized = False

    ### Map of decorated interceptors and corresponding arguments
    decorated_interceptor_map = {}

    def __init__(self, logger: Logger|None = None):
        SwitchboardObserver.__init__(self, logger = logger)

        self.init_intercept()
        self.switches = {}
        self._switch_lock = Lock()
        self._connect_signals()

    def _connect_signals(self):
        self.signal_store.connect_signal(
            self.SPEC_SIGNAL,
            self._on_signal_spec)

        self.signal_store.connect_signal(
            self.STATUS_SIGNAL,
            self._on_signal_status)

    def _disconnect_signals(self):
        self.signal_store.disconnect_signal(
            self.SPEC_SIGNAL,
            self._on_signal_spec)

        self.signal_store.disconnect_signal(
            self.STATUS_SIGNAL,
            self._on_signal_status)

    def _on_signal_spec(self, msg: Signal):
        if switch := self._get_or_map_switch(msg):
            switch.update_specification(msg.specification)

    def _on_signal_status(self, msg: Signal):
        if switch := self._get_or_map_switch(msg):
            switch.update_status(msg.status)

    def _get_or_map_switch(self, msg: Signal) -> Switch|None:
        switch = None

        if switch_name := msg.mapping_key:
            with self._switch_lock:
                match msg.mapping_action:
                    case MappingAction.ADDITION | MappingAction.UPDATE:
                        try:
                            switch = self.switches[switch_name]
                        except KeyError:
                            switch = self.switches[switch_name] = self._new_switch(switch_name)

                    case MappingAction.REMOVAL:
                        switch = self.switches.pop(switch_name, None)

        return switch


    @abstractmethod
    def initialize(self):
        self.initialized = True

    @abstractmethod
    def deinitialize(self):
        self.initialized = False


    @abstractmethod
    def _new_switch(self, switch_name: str) -> Switch:
        '''
        Create a local Switch object without adding it to the board.
        Intended for local updates in response to server signals.
        '''

    @abstractmethod
    def get_switches(self,
                     selection: SwitchSelectionInput|None = None,
                     with_ancestors: bool = False) -> Mapping[str, Switch]:
        '''
        Get avaialble switches directly from the server

        @param selection
            Switch name patterns to include in response.  These may be strings
            representing individual switches, shell-style globbing expressions
            containing placeholders such as '*', '?', and/or '[a-z]', or
            compiled regular expression objects.

        @param with_ancestors
            Recursively include ancestors, i.e., switches that are direct or
            indirect dependencies of those included in 'selection'.

        @return
            A map of ProtoBuf `Switch` structures.
        '''


    @abstractmethod
    def get_status(self,
                   selection: SwitchSelectionInput|None = None,
                   with_ancestors: bool = False,
                   ) -> Mapping[str, Status]:
        '''
        Get a mapping of switch names and corresponding statuses.

        @param selection
            Switch name patterns to include in response.  These may be strings
            representing individual switches, shell-style globbing expressions
            containing placeholders such as '*', '?', and/or '[a-z]', or
            compiled regular expression objects.

        @param with_ancestors
            Recursively include ancestors, i.e., switches that are direct or
            indirect dependencies of those included in 'selection'.

        @return
            A map of switch names and corresponding Status objects.
        '''


    def get_switch(self,
                   switch_name: str,
                   required: bool = False,
                   ) -> Switch|None:
        '''
        Get the named switch.

        @returns
            The named `Switch` instance if it exists, otherwise `None`.
        '''

        if switch := self.switches.get(switch_name):
            return switch

        for name, switch in self.switches.items():
            if switch_name in switch.aliases:
                return switch

        if required:
            raise KeyError(f"No such switch: {switch_name}")

        return None


    @abstractmethod
    def get_or_add_switch(self,
                          switch_name: str,
                          initially_active: bool = False,
                          ) -> Switch:
        '''
        Get the named switch, or create it if missing.

        This differs from `add_switch()` in that it return immediately even if
        the switch has to be added, sending the service request to do in the
        background.  This is useful if you want to avoid blockng in case the
        Switchboard service is not yet available.

        If you need to modify the switch after creation (e.g. to modify its
        dependencies or state), use `add_switch()` instead.  This ensures that
        the switch exists on the server side before you attempt those
        modifications.

        @returns
            An existing or new `Switch`
        '''


    @abstractmethod
    def add_switch(self,
                   switch_name: str,
                   initially_active: bool = False,
                   ) -> Switch:
        '''
        Add a new switch, or obtain the existing instance if any.

        This differs from `get_or_add_switch()` in that it always sends a
        service request to add the switch, even if it already exists locally.
        and returns only after the server has responded to this request.
        This makes it safe to then perform other operations on the switch,
        such as modifying its dependencies or its state.

        On the other hand, this call may block or fail if the server is
        unavailable.


        @param switch_name
            Name for the new switch.

        @param initially_active
            Initial active value

        @returns
            Switch object
        '''

    @abstractmethod
    def remove_switch(self,
                      switch_name: str,
                      propagate: bool = True,
                      ) -> bool:
        '''
        Remove an existing switch.

        @param switch_name
            Name of switch to remove.

        @param propagate
            Automatically update the state of successors (downstream dependents)
            once this switch has been removed.

        @returns
            True if the switch was removed, False if it did not exist.
        '''

    @abstractmethod
    def clear_switches(self,
                       reload: bool = False,
                       ) -> bool:
        '''
        Clear all switches, optionally reloading defaults.

        @param reload
            Reload default switches

        @returns
            True if changes were made, False otherwise.
        '''

    @abstractmethod
    def import_switches(self,
                        declarations: PyValueMap,
                        replace_specifications: bool = False,
                        replace_statuses: bool = False,
                        invoke_interceptors: InvocationStyle = InvocationStyle.ALL) -> int:
        '''
        Import switches from a list of key/value declarations, like those
        found in settings files.

        @param declarations
            A list of key/value objects, like those read from a settings file.

        @param replace_specifications
            Replace existing specifications: aliases, localizations, dependencies
            (but not interceptors)

        @param replace_statuses
            Replace existing switch statuses

        @param invoke_interceptors
            If statuses were added or changed, also invoke interceptors

        @return
            Number of switches that were added
        '''

    @abstractmethod
    def export_switches(self,
                        selection: SwitchSelectionInput|None = None,
                        include_specifications: bool = False,
                        include_statuses: bool = True) -> Mapping[str, Mapping]:
        '''
        Export switches to dictionary, in a format that can subsequently be
        imported with `import_switches()`.

        @param selection
            Switch name patterns to include in export.  These may be strings
            representing individual switches, shell-style globbing expressions
            containing placeholders such as '*', '?', and/or '[a-z]', or
            compiled regular expression objects.

        @param include_specifications
            Include switch specifications in export

        @param include_statuses
            Include current switch statuses in export

        @return
            A list of dictionaries, each representing one switch
        '''


    def load_switches(self,
                      filename: FilePathInput,
                      replace_specifications: bool = False,
                      replace_statuses: bool = False,
                      invoke_interceptors: InvocationStyle = InvocationStyle.ALL):
        '''
        Load switches from a settings file.  Valid file formats are those
        supported by `cc.core.settingsstore.SettingsStore`, including JSON,
        YAML, and INI.

        The settings schema is described in `common-swiches.yaml`, normally
        installed under `/usr/share/common-core/settings/switches/`.
        At the root of the settings tree, switch names are mapped to
        corresponding declarations.
        '''

        declarations = SettingsStore(filename)
        return self.import_switches(
            declarations = declarations,
            replace_specifications = replace_specifications,
            replace_statuses = replace_statuses,
            invoke_interceptors = invoke_interceptors)


    @abstractmethod
    def save_switches(self,
                      filename: FilePathInput,
                      selection: SwitchSelectionInput|None = None,
                      include_specifications: bool = False,
                      include_statuses: bool = True):

        '''
        Save switches to a JSON file.

        @param filename
            Output filename. Relative names are resolved with respect to the
            default host-specific setting folder; see
            `cc.core.settingsstore.SettingsStore` for details.

        @param selection
            Switch name patterns to include in export.  These may be strings
            representing individual switches, shell-style globbing expressions
            containing placeholders such as '*', '?', and/or '[a-z]', or
            compiled regular expression objects.

        @param include_specifications
            Include switch specifications in export

        @param include_statuses
            Include current switch statuses in export

        The settings schema is described in `common-swiches.yaml`, normally
        installed under `/usr/share/common-core/settings/switches/`.
        This file can later be imported using `load_switches()`.
        '''


    @abstractmethod
    def init_intercept(self):
        '''
        Initialize interception handling.
        '''
        self.interceptor_methods = {}


    @abstractmethod
    def add_interceptor(self,
                        interceptor_name: str,
                        switch_selection: SwitchSelectionInput,
                        state_transitions: StateSet,
                        callback: InterceptorMethod,
                        phase: InterceptorPhase = InterceptorPhase.NORMAL,
                        asynchronous: bool = False,
                        immediate: bool = False,
                        rerun: bool = False,
                        future: bool = False,
                        on_cancel: ExceptionHandling = ExceptionHandling.ABORT,
                        on_error: ExceptionHandling = ExceptionHandling.FAIL,
                        ) -> bool:
        '''
        Add a new interceptor to be executed once the switch enters the
        specified state(s).

        The interceptor may raise a `CancelIntercept` exception to indicate that
        the state transition should be cancelled.  In this case, the value of
        `on_cancel` determines what happens next:


          - ExceptionHandling.IGNORE:
            proceed with the state change regardless

          - ExceptionHandling.ABORT:
            return to the previous settled state without further actions

          - ExceptionHandling.FAIL:
            redirect the switch to the `FAILED` state

          - ExceptionHandling.REVERT:
            re-enter the previous settled state anew, invoking any applicable
            interceptors on the way.


        **Inputs:**

        @param interceptor_name
            Unique name/id for this interceptor.

        @param switch_selection
            Switch name patterns to which this interceptor is applied.  These
            may be strings representing individual switches, shell-style
            globbing expressions, or compiled regular expression objects.

        @param state_transitions
            A bitmask representing states for which the inerceptor is invoked.
            Often just a single transitional state, i.e., `ACTIVATING`,
            `DEACTIVATING` or `FAILING`.

        @param asynchronous
            Allow state to transition to the next state (normally `ACITVE`,
            `INACTIVE` or `FAILED`) even as this interceptor continues to run in
            the background.

        @param phase
            Run this interceptor prior to (EARLY), concurrent with (NORMAL), or
            subsequent to (LATE) the main interceptors for the specified state
            transitions.

        @param immediate
            If the interceptor's trigger states include this switch's current
            state OR the transitional state preceding it (for instance, if the
            switch is currently ACTIVE and the interceptor triggers on either
            ACTIVATING and ACTIVE), invoke it immediately. In this case, unless
            `asynchronous` flag is also True, the call blocks until the
            interceptor has completed.

        @param rerun
            Whether to invoke interceptor when (explicitly) re-entering one of
            the specified states, even if the switch is already in that state.

        @param future
            Add this interceptor also to matchihng switches created in the
            future.

        @param on_cancel
            How to proceed if state change is cancelled. The default value
            `DEFAULT` is equivalent to `ABORT`.

        @param on_error
            How to proceed if the interceptor encounters an error. The default
            value `DEFAULT` is equivalent to `FAIL`.

        @returns
            True if the interceptor was added.
        '''


    @abstractmethod
    def remove_interceptor(self,
                           interceptor_name: str,
                           switch_selection: SwitchSelectionInput|None = None,
                           abandon_pending: bool = True,
                           ) -> bool:
        '''
        Remove an existing interceptor

        @param[in] name
            Interceptor name.

        @param[in] switch_selection
            Specific switches from which this interceptor will be removed.
            By default it is removed from all existing switches, and will
            not be added to switches created in the future.

        @param abandon_pending
            Abandon any pending invocations of this interceptor.

        @returns
            `true` if a removal took place.
        '''


    def register_interceptor(self,
                             interceptor_name: str,
                             method: InterceptorMethod,
                             ) -> bool:
        '''
        Register an interceptor method to be invoked whenever the specified
        switch changes state.

        @return
            True if the interceptor was added
        '''

        is_new = interceptor_name not in self.interceptor_methods
        self.interceptor_methods[interceptor_name] = method
        return is_new


    def deregister_interceptor(self,
                               interceptor_name: str,
                               ) -> bool:
        '''
        Unregister an previusly registered interceptor method.

        @return
            True if the interceptor existed and was removed.
        '''

        try:
            del self.interceptor_method[interceptor_name]
        except KeyError:
            return False
        else:
            return True


    @classmethod
    def interceptor(cls,
                    switch_selection: SwitchSelectionInput,
                    state_transitions: StateMask|StateSet = State.PENDING,
                    phase: InterceptorPhase = InterceptorPhase.NORMAL,
                    asynchronous: bool = False,
                    immediate: bool = False,
                    rerun: bool = False,
                    future: bool = True,
                    on_cancel: ExceptionHandling = ExceptionHandling.ABORT,
                    on_error: ExceptionHandling = ExceptionHandling.FAIL):
        '''
        Generate a decorator function to declare an interceptor for
        Switchboard state transitions.

        To actually register these interceptors with the server, the object
        instance that contains the decorated methods must invoke
        `register_decorated_handlers()`, passing in its own object as the sole
        argument.

        For more details, including a descripion of input arguments, see
        `add_interceptor()`.

        **Example:**

        ```python
        from cc.platform.switchboard import (
            AsyncClient as Switchboard,
            State,
            InterceptorInvocation,
        )

        def __init__(self):
            self.switchboard = Switchboard('localhost')
            self.switchboard.register_decorated_interceptors(self)
            self.switchboard.initialize()

        @SwitchboardBase.interceptor('SomeSwitch', State.ACTIVATING)
        async def activate_someswitch(self, invocation: InterceptorInvocation):
            print("Entering switch %r interceptor %r for transition to %r"%(
                invocation.switch_name,
                invocation.interceptor_name,
                invocation.state))
        ```
        '''

        kwargs = locals()
        del kwargs['cls']

        def decorator(method: InterceptorMethod):
            '''
            Method decorator for Switchboard interceptor
            '''

            cls.decorated_interceptor_map[method] = kwargs
            return method

        return decorator


    def register_decorated_handlers(self, instance: object):
        '''
        Register to invoke handler methods wrapped by any of the following
        decorator methods:

          ```python
          @SwitchboardBase.interceptor(...)
          @SwitchboardBsae.specification_handler(...)
          @SwitchboardBase.status_handler(...)
          ```

        This must be invoked for those interceptors to become effective.

        @param instance
            The object instance that contains the decorated methods.
        '''
        self.register_decorated_interceptors(instance)
        self.connect_decorated_handlers(instance)


    @virtual
    def register_decorated_interceptors(self, instance: object) -> int:
        '''
        Register interceptor decorated interceptor methods with server.
        This must be invoked for those interceptors to become effective.

        @param instance
            Instance of the class that contains decorated interceptors

        @returns
            Number of interceptors that were registered.
        '''
