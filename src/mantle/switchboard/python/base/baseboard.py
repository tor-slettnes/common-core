'''
Abstract switchboard implementation.
'''

__docformat__ = 'javadoc en'
__author__ = 'Tor Slettnes'

### Standard Python modules
from abc import abstractmethod
from collections import namedtuple
from typing import Callable, Sequence, Mapping
from logging import Logger
import os

### Core modules
from cc.core.paths import FilePathInput
from cc.core.decorators import virtual
from cc.core.invocation import method_path
from cc.core.settingsstore import SettingsStore
from cc.protobuf.wellknown import BoolValue
from cc.protobuf.status import Error
from cc.protobuf.signal import SignalStore, MappingAction
from cc.protobuf.variant import KeyValueMap, PyValueMap

### Modules within package
from ..protobuf import (
    Signal, SwitchInfo, SwitchSelectionInput, SwitchMap,
    Specification, SpecificationMap,
    Dependency, DependencyMap, DependencyPolarity,
    DependencyStatus, DependencyStatusMap,
    Status, StatusMap, GetAttributesResponse, ErrorMap,
    InterceptorMethod, InterceptorPhase,
    InterceptorInvocation, InterceptorResult,
    ExceptionHandling, InvocationStyle, CascadeStyle,
    State, StateMask, StateSet, encodeStateSet,
    ExportResponse,
)

from .switch import Switch
from .observer import SwitchboardObserver


AddSwitchResult = namedtuple('AddSwitchResult', ('switch', 'added'))

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
        # return Switch(switch_name, self)


    def get_switch(self,
                   switch_name: str,
                   required: bool = False,
                   ) -> Switch|None:
        '''
        Return the local proxy for the named switch, if available.

        @param switch_name
            Name or alias of switch to obtain

        @param required
            Raise a KeyError if the switch does not exist.

        @note
            `Switch` objects are created and updated locally in response to
            asynchronous specification and status updates from the Switchboard
            server. As such, do not rely on such a proxy immediately after
            invoking `add_switch()`, or from an interceptor that gets invoked as
            a new switch is created on the server side.  If you need to ensure
            the local switch proxy exists before proceeding, use
            `get_or_add_switch()` instead.

        @throws KeyError
            The `required` argument is set, and specified switch/proxy does not
            exist.

        @return
            `Switch` proxy if it exists, otherwise None.
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
                          ) -> AddSwitchResult:
        '''
        Get the local proxy for the named switch, or create it if missing.

        This differs from `add_switch()` in that the Switch proxy object is
        returned immediately from the local cache if it exists. Only if it does
        not is a request sent to the server to add the switch.

        This call may block or fail if the switch proxy does not yet exist and
        the server is unavailable.

        @param switch_name
            Name or alias of switch to obtain

        @param initially_active
            Set the initial value of the switch on creation. Until dependencies
            are added, this also determines its initial state ('INACTIVE` or
            'ACTIVE').

        @returns
            An existing or new `Switch` object
        '''


    @abstractmethod
    def add_switch(self,
                   switch_name: str,
                   initially_active: bool = False,
                   ) -> AddSwitchResult:
        '''
        Add a new switch, or obtain the existing instance if any.

        This differs from `get_or_add_switch()` in that it always sends a
        service request to add the switch, even if it already exists locally.
        and returns only after the server has responded to this request.

        This call may block or fail if the server is unavailable.

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
        found in declaration files and/or exported via `export_switches()`.

        @param declarations
            A list of key/value objects, like those read from declaratio files.

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
                        include_statuses: bool = True,
                        ) -> PyValueMap:
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
            A recursive dictionary of switches and their coorresponding
            declarations.
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
    def get_switch_info(self,
                        selection: SwitchSelectionInput|None = None,
                        with_ancestors: bool = False,
                        ) -> Mapping[str, object]:
        '''
        Get and decode available switch information directly from the server.

        @param selection
            Switch name patterns to include in response.  These may be strings
            representing individual switches, shell-style globbing expressions
            containing placeholders such as '*', '?', and/or '[a-z]', or
            compiled regular expression objects.

        @param with_ancestors
            Recursively include ancestors, i.e., switches that are direct or
            indirect dependencies of those included in 'selection'.

        @return
            A map of switch names to dynamically created data objects,
            decoded from the ProtoBuf `SwitchMap` response from the server.
        '''


    @abstractmethod
    def set_specification(self,
                          switch_name: str,
                          specification: Specification,
                          replace_aliases: bool = False,
                          replace_localizations: bool = False,
                          replace_dependencies: bool = False,
                          replace_interceptors: bool = False,
                          active: bool|None = None,
                          update_state: bool|None = None) -> bool:
        '''
        Set or update the specification of a switch.
        '''


    @abstractmethod
    def get_specifications(self,
                           selection: SwitchSelectionInput|None = None,
                           with_ancestors: bool = False,
                           ) -> Mapping[str, object]:
        '''
        Get a mapping of switch names and corresponding specifications.

        @param selection
            Switch name patterns to include in response.  These may be strings
            representing individual switches, shell-style globbing expressions
            containing placeholders such as '*', '?', and/or '[a-z]', or
            compiled regular expression objects.

        @param with_ancestors
            Recursively include ancestors, i.e., switches that are direct or
            indirect dependencies of those included in 'selection'.

        @return
            A map of switch names to dynamically created data objects, decoded
            from the ProtoBuf `SpecificationMap` response from the server.
        '''


    @abstractmethod
    def add_dependency(self,
                       switch_name: str,
                       predecessor_name: str,
                       trigger_states: StateSet = State.SETTLED,
                       polarity: DependencyPolarity = DependencyPolarity.POSITIVE,
                       hard: bool = False,
                       sufficient: bool = False,
                       allow_update: bool|None = None,
                       reevaluate: bool|None = None,
                       ) -> bool:
        '''
        Add a new upstream dependency (direct ancestor) to this switch.

        @param switch_name
            The name of the switch from which we are adding a dependency.

        @param predecessor_name
            The name of an existing switch that is to be removed as a predecessor

        @param trigger_states
            A bitmask representing which of the predecessor's state transitions
            that will automatically trigger a reevaluation of this switch's
            state, based on this and its other dependencies. A value of zero
            means that this switch does not automatically update its state in
            response to a change in this predecessor. See also `set_auto()`.
            The default value, `State.SETTLED`, is equivalent to `State.ACTIVE |
            State.INACTIVE | State.FAILED`.

        @param polarity
            Whether this is a normal/positive dependency, a negative/conflicting
            dependency, or this switch toggles/flip-flops in response to the
            predecessor's value changing.

        @param hard
            Hard dependency: This switch cannot be set unless this dependency is
            satisfied

        @param sufficient
            Whether or not this dependency alone is sufficient to activate this
            switch, irrespective of other dependencies. This implies a logical
            OR instead of AND conditition, and as a side effect, this dependency
            becomes redundant if the successor's other dependencies are
            satisfied.

        @param allow_update
            Allow existing dependency to be updated

        @param reevaluate
            Recalculate this switch's state after adding the dependency.
        '''


    @abstractmethod
    def remove_dependency(self,
                          switch_name: str,
                          predecessor_name: str,
                          reevaluate: bool = True,
                          ) -> bool:
        '''
        Remove an existing dependency from a switch.

        @param switch_name
            The name of the switch from which we are removing a dependency.

        @param predecessor_name
            The name of an existing switch that is to be removed as a predecessor

        @param reevaluate
           Recalculate state after removing dependency
        '''

    @abstractmethod
    def get_dependencies(self,
                         switch_name: str) -> Mapping[str, Dependency]:
        '''
        Obtain a map of direct dependencies for the given switch

        @param switch_name
            The name of the switch for which we are obtaining dependencies

        @return
            A map of switch names and corresponding Dependency objects.
        '''


    @abstractmethod
    def get_ancestors(self,
                      switch_name: str) -> Sequence[str]:
        '''
        Obtain a list of direct and indirect ancestors of a switch.
        '''

    @abstractmethod
    def get_descendants(self,
                        switch_name: str) -> Sequence[str]:
        '''
        Obtain a list of direct and indirect descendants of a switch.
        '''

    @abstractmethod
    def set_target(self,
                   switch_name: str,
                   target_state: State|None,
                   error: Error|Exception|str|None = None,
                   attributes: PyValueMap|None = None,
                   clear_existing: bool = False,
                   invoke_interceptors: InvocationStyle = InvocationStyle.ALL,
                   cascade_descendants: CascadeStyle = CascadeStyle.ASYNC,
                   reenter: bool = False,
                   on_cancel: ExceptionHandling = ExceptionHandling.DEFAULT,
                   on_error: ExceptionHandling = ExceptionHandling.DEFAULT,
                   ) -> bool:
        '''
        Transition a switch to the specified `target_state`. If not provided, then
          * if `error` is given, the target state is inferred as `State.FAILED`;
          * otherwise, the target state is inferred based on the current state of the
            switch's dependencies.

        @param switch_name
            Name of switch whose state we are changing.

        @param target_state
            Desired target state. Normally this is one of the "settled" states
            (ACTIVE, INACTIVE, or FAILED), in which case the switch will first
            change to the corresponding pending state (ACTIVATING, DEACTIVATING,
            FAILING), triggering any associated descendant updates and
            interceptor invocations on the way.

        @param error
            Any error data associated with this switch. Ignored if the target is
            specified but is not one of `FAILING` or `FAILED`.

        @param attributes
            Arbitrary key/value pairs assigned to the switch. These may be
            cleared in a future state change.

        @param clear_existing
            Clear all existing attributes before setting those provided here.

        @param invoke_interceptors
            Run interceptors associated with each state transition (e.g.  if
            `target_state` is ACTIVE, first run interceptors for ACTIVATING, and
            if successful, those for ACTIVE).  Options are: NONE (do not run
            interceptors), ALL (run interceptors on switch and all its
            descendants), or INDIRECT (run interceptors only on descendants).

        @param cascade_descendants
            Whether and how to cascade the state change, if any, the switch's
            descendants, starting with its immediate successors. Only switches
            with dependencies that include the corresponding state transition(s)
            of this switch are reevaluated.

        @param reenter
            Make the transition (via the applicable pending state) even if the
            switch is already in the desired target state, invoking any relevant
            interceptors on the way.

        @param on_cancel
            What to do if the state transition is cancelled, e.g. if pre-empted
            by another target setting while executing interceptrs.  If omitted,
            the highest-numbered `on_cancel` attribute amongst the associated
            interceptors is used.

        @param on_error
            What to do if the interceptors associated with the state transition
            encounters errors.  If omitted, the highest-numbered `on_error`
            attribute amongst the associated interceptors is used.

        @returns
            True iff the target state was modified
        '''


    @abstractmethod
    def get_statuses(self,
                     selection: SwitchSelectionInput|None = None,
                     with_ancestors: bool = False,
                     ) -> Mapping[str, object]:
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
            A map of switch names to dynamically created data objects,
            decoded from the ProtoBuf `StatusMap` response from the server.
        '''


    @abstractmethod
    def get_dependency_statuses(self,
                                switch_name: str,
                                ) -> Mapping[str, object]:
        '''
        Obtain a recursive tree of ancestors for a switch, with indication
        of each ancestor's current state and whether the dependency is
        satisfied.

        @param switch_name
            Switch name for which we are obtaining dependency statuses.

        @return
            A map of switch names to dynamically created data objects, decoded
            from the ProtoBuf `DependencyStatusMap` response from the server.
        '''


    @abstractmethod
    def set_attributes(self,
                       switch_name: str,
                       attributes: PyValueMap|None = None,
                       clear_existing: bool = False,
                       ) -> bool:
        '''
        Assign arbitrary key/value pairs to a switch.
        See `set_target()` for details on the input arguments.
        '''

    @abstractmethod
    def get_attributes(self,
                       switch_name: str,
                       inherit: bool = False) -> PyValueMap:
        '''
        Obtain a key/value map of attributes assigned to a switch

        @param inherit
            Also recursively merge in attributes from its ancestors.

        @return
            A recursive key/value map of values assigned to this switch, decoded
            from the ProtoBuf `GetAttributesResponse` response from the serer.
        '''

    @abstractmethod
    def get_culprits(self,
                     switch_name: str,
                     expected_position: bool = True) -> Mapping[str, Status]:
        '''
        Obtain root causes for a switch not being in the expected positiion.

        @param switch_name
          Switch whose culprits we are querying

        @param expected_position
          Report culprits iff the current value of the switch is different from this.

        @returns
          A map of switch names to dynamically created data objects, decoded
          from the ProtoBuf `StatusMap` response from the server.
        '''

    @abstractmethod
    def get_errors(self,
                   switch_name: str,
                   ) -> Mapping[str, object]:
        '''
        Obtain errors assigned to a switch and its ancestors.

        @param switch_name
          Switch whose errors we are querying

        @returns
          A map of switch names to dynamically created data objects, decoded
          from the ProtoBuf `ErrorMap` response from the server.
        '''


    @abstractmethod
    def invoke_interceptor(self,
                           interceptor_name: str,
                           switch_name: str,
                           state: State|None = None
                           ) -> Error|None:
        '''
        Manually invoke a specific interceptor, as if it were triggered by a
        switch changing its current state.  Primarily a diagnostic tool.

        @param interceptor_name
            Name of interceptor to invoke.

        @param switch_name
            Switch on behalf of which the interceptor is to be invoked

        @param state
            State to pass as input argument for the interceptor. If not
            provided, defaults to the current state of this switch.
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
                        method: InterceptorMethod,
                        phase: InterceptorPhase = InterceptorPhase.NORMAL,
                        decode: bool = False,
                        asynchronous: bool = False,
                        rerun: bool = False,
                        immediate: bool = False,
                        future: bool = False,
                        on_cancel: ExceptionHandling = ExceptionHandling.ABORT,
                        on_error: ExceptionHandling = ExceptionHandling.FAIL,
                        ) -> bool:
        '''
        Add a new interceptor method to be executed once the switch enters
        the specified state(s).  This will typically comprise one or more
        _transitional_ states: `ACTIVATING`, `DEACTIVATING`, and/or `FAILING`.
        Only after all interceptors for a given state transition have completed
        can the switch proceed to its next state, typically a corresponding
        _stable_ state: `ACTIVE`, `INACTIVE`, or `FAILED`.

        Upon invocation, the method receives as its sole input an
        `cc.platform.switchboard.protobuf.InterceptorInvocation` object, the
        prototype of which is defined in `switchboard_types.proto`.
        Alternatively, if `decode` is set to `True`, the method will instead
        receive an instance of an equivalent, dynamically created Python
        dataclass by the same name.  The main difference is that in the latter
        case, nested value types have been decoded into native Python objects;
        for example `state` will be provided as a Python `IntEnum` value, and
        `cascaded_attributes` as a plain `dict` rather than a
        `cc.protobuf.variant.KeyValueMap()` object.

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

        The the `on_error` input similarly determines the next course of action
        in response to any other exception.

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

        @param decode
            Upon invocation, rather than directly passing in the original
            `cc.platform.switchboard.protobuf.InterceptorInvocation` message
            that was received from the Switchboard service, decode this message
            into aninstance of an equivalent, dynamically created Python
            dataclass.

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

        self.register_interceptor(interceptor_name, method, decode)
        self.start_intercepting()


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

        self.deregister_interceptor(interceptor_name)


    def register_interceptor(self,
                             interceptor_name: str,
                             method: InterceptorMethod,
                             decode: bool,
                             ) -> bool:
        '''
        Register an interceptor method to be invoked whenever the specified
        switch changes state.

        @return
            True if the interceptor was added
        '''

        is_new = interceptor_name not in self.interceptor_methods
        self.interceptor_methods[interceptor_name] = (method, decode)
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
                    decode: bool = False,
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
        `register_decorated_interceptors()` (possibly indirectly via
        `register_decorated_handlers()`), passing in its own object as the sole
        argument.

        For more details, including a descripion of input arguments, see
        `add_interceptor()`.

        **Example:**

        ```python
        from cc.platform.switchboard.grpc import AsyncClient as SwitchboardClient
        from cc.platform.switchboard.protobuf import State, InterceptorInvocation

        def __init__(self):
            self.switchboard = SwitchboardClient('localhost')
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


    @abstractmethod
    def is_intercepting(self) -> bool:
        '''
        Indicate whether the the client is operating any interceptors.
        '''

    @abstractmethod
    def start_intercepting(self):
        '''
        Start a worker task to handle Interceptor invocations from the
        server. Does nothing if the task is already running.
        '''

    @abstractmethod
    def stop_intercepting(self, wait=True):
        '''
        Stop any running worker task to handle Interceptor invocations from
        the server.

        @param wait
            Wait for the worker task to finish before returning.
        '''


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

