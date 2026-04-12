'''
Abstract representation of a Switchboard node ("Switch")
'''

__docformat__ = 'javadoc en'
__author__ = 'Tor Slettnes'

### Standard ProtoBuf modules
from typing import Mapping, Sequence, Set, Callable
from weakref import ref

### Core modules
from cc.core.decorators import virtual
from cc.core.docbase import DocBase
from cc.core.maputils import recursive_merge
from cc.core.invocation import safe_invoke
from cc.protobuf.status import Error, encodeError, Level
from cc.protobuf.variant import (
    PyValueMap, KeyValueMap,
    encodeKeyValueMap, decodeKeyValueMap,
)

### Swithboard modules
from ..protobuf import (
    SwitchboardDissecter,
    Specification, SpecificationMap,
    Dependency, DependencyMap, DependencyPolarity, DependencyStatus, DependencyStatusMap,
    Localization, LocalizationMap, LocalizationsInput, encodeLocalization, encodeLocalizationMap,
    InterceptorSpec, InterceptorInvocation, InterceptorMethod,
    InterceptorPhase, ExceptionHandling, InvocationStyle, CascadeStyle,
    LanguageCode, LanguageChoice, GetAttributesResponse,
    Status, StatusMap, ErrorMap, State, StateSet, encodeStateSet,
    DEFAULT_LANGUAGE,
)

InterceptorName = str
SwitchUpdateSubscriber = Callable[['Switch'], None]


class Switch (DocBase, SwitchboardDissecter):
    '''
    Local proxy object representing a single switch.
    '''

    def __init__(self,
                 name: str,
                 board: 'SwitchboardBase',
                 ):
        self.name = name
        self._board = ref(board)
        self.subscriptions = {}
        self.raw_specification = Specification()
        self.raw_status = Status()

    def __repr__ (self):
        return "Switch(%r, %s)"%(self.name, self.current_state.name)

    @classmethod
    def _inputs(cls,
                inputs: dict,
                ignore: Sequence|str|None = None,
                ):

        ignore_list = ['self', 'board']
        if isinstance(ignore, str):
            ignore_list.append(ignore)
        elif isinstance(ignore, Sequence):
            ignore_list.extend(ignore)

        params = {
            k:v
            for (k, v) in inputs.items()
            if not k in ignore_list
        }
        return params

    @property
    def board(self) -> object:
        if board := self._board():
            return board
        else:
            raise RuntimeError(
                "Switchboard is not available (shutdown in progress?)"
            )

    @property
    @SwitchboardDissecter.decode_response
    def specification(self) -> object:
        '''
        Return the complete specification for this switch.
        '''
        return self.raw_specification

    @property
    @SwitchboardDissecter.decode_response
    def status(self) -> object:
        '''
        Return the complete status for this switch.
        '''
        return self.raw_status


    @property
    def is_primary(self) -> bool:
        '''
        Indicate whether switch is `is_primary`, i.e. whether to stop
        descending further into its dependencies when looking for culprits
        '''
        return self.specification.is_primary

    @property
    def aliases(self) -> Sequence[str]:
        '''
        Return a list of aliases for this string.
        '''
        return self.specification.aliases

    @property
    def localizations(self) -> Mapping[str, object]:
        '''
        Return language codes mapped to text strings for this switch,
        including its description, texts explaining each target action
        (True/False), and texts explaining each state (UNSET, DEACTICATING,
        INACTIVE, ACTIVATING, ACTIVE, FAILING, FAILED).
        '''
        return self.specification.localizations

    @property
    def dependencies(self) -> Mapping[str, object]:
        '''
        Return a map of dependencies for this switch. @sa add_dependency().
        '''
        return self.specification.dependencies

    @property
    def successors(self) -> Mapping[str, 'Switch']:
        '''
        Return a map of immediate successors to this switch, i.e., switches
        with a direct dependency on this one.
        '''
        successors = {}
        for candidate in self.board.switches.values():
            if self.name in candidate.dependencies:
                successors[candidate.name] = candidate
        return successors

    @property
    def predecessors(self) -> Mapping[str, 'Switch']:
        '''
        Return a map of immediate ancestors to this switch, i.e., switches
        that are direct dependencies of this one.
        '''
        predecessors = {}
        for predecessor_name, dep in self.dependencies.items():
            if predecessor := self.board.get_switch(predecessor_name):
                predecessors[predecessor_name] = predecessor
        return predecessors

    @property
    def interceptors(self) -> Mapping[str, object]:
        '''
        Return a list of interceptors associated with this switch.

        @see add_interceptor()
        '''

        return self.specification.interceptors

    @property
    def active(self) -> bool:
        '''
        Indicate whether the switch is currently active
        '''
        return self.status.active

    @property
    def pending(self) -> bool:
        '''
        Indicate whether the switch is currently in a "pending" state
        (DEACTIVATING, ACTIVATING, FAILING).
        '''
        return self.status.pending

    @property
    def settled(self) -> bool:
        '''
        Indicate whether the switch is currently in a "settled" state
        (INACTIVE, ACTIVE, FAILED).
        '''
        return not self.status.pending

    @property
    def target(self) -> bool|None:
        '''
        Return the target position (False/True) if a transition is currently
        in progress, otherwise None.
        '''

        if self.current_state == State.ACTIVATING:
            return True
        elif self.current_state == State.DEACTIVATING:
            return False
        else:
            return None

    @property
    def state(self) -> object:
        '''
        Return the current state of this switch.
        Identical to `currrent_state`.
        '''

        return State(self.status.current_state)


    @property
    def current_state(self) -> State:
        '''
        Return the current state of this switch
        '''

        return State(self.status.current_state)


    @property
    def settled_state(self) -> State:
        '''
        Return the settled state of this switch. This will be one of
        UNSET, ACTIVE, INACTIVE, FAILED.
        '''

        return State(self.status.settled_state)


    @property
    def error(self) -> object|None:
        '''
        Return any error currently associated with switch
        '''
        return self.status.error


    @property
    def attributes(self) -> dict:
        '''
        Return a dictinoary of arbitrary key/value pairs currently associated
        with this switch.
        '''
        return self.status.attributes


    @property
    def cascaded_attributes(self) -> dict:
        '''
        Create and return a dictionary of attributes for this switch, with
        those from its ancestors merged in recursively.  Preference is tiven to
        keys from this switch, but the merging order of values from ancestors is
        undetermined.
        '''
        attributes = self.attributes
        for predecessor in self.predecessors.values():
            recursive_merge(attributes, predecessor.cascaded_attributes)

        return attributes


    def update_specification(self, specification: Specification):
        self.raw_specification.CopyFrom(specification)
        return self.publish_update()


    def update_status(self, status: Status):
        self.raw_status.CopyFrom(status)
        return self.publish_update()


    def publish_update(self):
        for (handle, callback) in self.subscriptions.items():
            self.publish_update_to(callback)

    def publish_update_to(self, callback: SwitchUpdateSubscriber):
        safe_invoke(
            callback,
            args = (self,),
            log_call = self.logger.debug,
            log_failure = self.logger.error,
        )

    def subscribe_updates(self,
                          handle   : str,
                          callback : SwitchUpdateSubscriber,
                          immediate: bool = False):
        '''
        Register a callback to be invoked whenever this switch changes
        specification or status

        @param handle
            Unique handle for this callback. If a callback is already
            registered with the same handle, it will be replaced.

        @param callback
            Method to be invoked whenever this switch changes specification or state.
            Must take this switch instance as its sole argument.

        @param immediate
            Immediately invoke calback with current status.
        '''

        self.subscriptions[handle] = callback
        if immediate:
            self.publish_update_to(callback)


    def unsubscribe_updates(self,
                            handle: str) -> bool:
        '''
        Unregiser a callback handler

        @param handle
            Handle for the registered callback.

        @returns
            True if the specified handle was found and removed, False otherwise.
        '''

        return self.subscriptions.pop(handle, None) is not None


    def set_specification(self,
                          specification: Specification,
                          replace_aliases: bool = False,
                          replace_localizations: bool = False,
                          replace_dependencies: bool = False,
                          replace_interceptors: bool = False,
                          active: bool|None = None,
                          update_state: bool|None = None) -> bool:
        '''
        Set or update the specification of this switch.
        '''

        return self.board.set_specification(
            self.name,
            **self._inputs(locals()))


    def get_specification(self) -> Specification:
        '''
        Return specifications for this switch.
        '''

        specs = self.board.get_specifications(selection = [self.name])
        return specs.get(self.name)


    def get_status(self) -> Status:
        '''
        Retrieve status of this switch from the server
        '''

        specs = self.board.get_specifications(selection = [self.name])
        return specs.get(self.name)


    def set_localizations(self,
                          localizations: LocalizationsInput,
                          replace: bool = False,
                          ) -> bool:
        '''
        Update localization map for this switch.

        @param localization
            Either of the following: a preconstructed ProtoBuf `LocalizationMap`
            message, or language codes mapped to ProtoBuf `Localization` types,
            or language codes mapped to dictionaries comprising `description`,
            `activate_text`, `deactivate_text`, and/or `state_texts` keys.  In
            the latter case, `state_texts` a mapping from
            `cc.protobuf.switchboard.State` to string.
        '''

        spec = Specification(
            localizations = encodeLocalizationMap(localizations))

        return self.set_specification(
            spec,
            replace_localizations=replace)


    def get_localization(self,
                         language_choices: LanguageChoice = DEFAULT_LANGUAGE,
                         ) -> Localization|None:
        '''
        Return language-specific text strings for this switch, including its
        description, texts explaining each target action (True/False), and texts
        explaining each state (UNSET, DEACTICATING, INACTIVE, ACTIVATING,
        ACTIVE, FAILING, _FAILED).

        @param language_choices
            Either a single language code or a sequence of language codes, each
            comprising an ISO language and optionally ISO country code (such as
            "en", "en_US", "nb_NO", "nn_NO").
        '''

        if isinstance(language_choices, (list, tuple)):
            candidates = choices

        elif isinstance(language_choices, str):
            candidates = (language_choices,)

        else:
            raise TypeError("`language_choices` must be a string, list or tuple, not %s"%
                            (type(languages).__name__,))

        for candidate in candidates:
            try:
                return self.localizations[candidate]
            except KeyError:
                pass
        else:
            return None


    def set_localization(self,
                         language_code: LanguageCode = DEFAULT_LANGUAGE,
                         localization: Localization|None = None,
                         *,
                         description: str|None = None,
                         activate_text: str|None = None,
                         deactivate_text: str|None = None,
                         state_texts: Mapping[State, str]|None  = None
                         ) -> bool:
        '''
        Change or add localization for this switch.

        @param language_code
            ISO language code, optionally followed by underscore and country code
            (e.g. "en" or "en_US")

        @param localization
            Preconstructed ProtoBuf `Localization` message.
            Alternatively this can be constructed from the following arguments.

        @param description
            Description of this switch

        @param activate_text
            Update texts describing the activation of this switch

        @param deactivate_text
            Update texts describing the deactivation of this switch

        @param state_texts
            Description of each possible state.

        The strings `activate_text`, `deactivate_text` and the mapped strings
        for `state_texts` should be suitable for building error messages about
        dependency conflicts: "cannot {activate_text} because {state_text}".

        Example:

          ```python
          from cc.protobuf.switchboard import Switch, State

          door_open = Switch("Door:Open")
          door_open.set_localization(
              "en",
              description = "Door open status",
              state_texts = {
                  State.ACTIVE: "the door is open"
              })

          door_locked = Switch("Door:Locked")
          door_locked.add_dependency("Door:Open", inverse=True, hard=True)
          door_locked.set_localization(
              "en",
              description = "Door lock status",
              activate_text = "lock the door",
              deactivate_text = "unlock the door")

          door_locked.set_target(True)
          ```

        Possible failure:
          ```
          >>> RuntimeError: Cannot lock the door because the door is open
          ```
        '''

        update = encodeLocalization(
            description = description,
            activate_text = activate_text,
            deactivate_text = deactivate_text,
            state_texts = state_texts)

        return self.set_localizations({language_code: update})

    def get_description(self,
                        language_choices: LanguageChoice = DEFAULT_LANGUAGE,
                        ) -> str:
        '''
        Return human readable description of this switch, e.g. `"door lock"`

        @param language_choices
            Either a single language code or a sequence of language codes,
            which will be used in order to look up the switch description.
            See `get_localization()` for details.
        '''
        if localization := self.get_localization(language_choices):
            return localization.description


    def get_activate_text(self,
                          language_choices: LanguageChoice = DEFAULT_LANGUAGE,
                          ) -> str:
        '''
        Obtain human readable text describing the trasition to ACTIVE.

        @param language_choices
            Either a single language code or a sequence of language codes,
            which will be used in order to look up the target texts.
            For details, @see get_localization().

        '''

        if localization := self.get_localization(language_choices):
            return localization.activate_text

    def get_deactivate_text(self,
                            language_choices: LanguageChoice = DEFAULT_LANGUAGE,
                            ) -> str:
        '''
        Obtain human readable text describing the trasition to INACTIVE.

        @param language_choices
            Either a single language code or a sequence of language codes,
            which will be used in order to look up the target texts.
            For details, @see get_localization().

        '''

        if localization := self.get_localization(language_choices):
            return localization.deactivate_text


    def get_target_text(self,
                        language_choices: LanguageChoice = DEFAULT_LANGUAGE,
                        target: bool|None = None
                        ) -> str:
        '''
        If the switch is currently in the ACTIVATING or DEACTIVATING state,
        return a human readable text describing the corresponding target
        possition; otherwise None.

        @param language_choices
            Either a single language code or a sequence of language codes,
            which will be used in order to look up the target texts.
            For details, @see get_localization().

        @returns
            A human readable text describing the current target position
            iff the switch is currently in the ACTIVATING or DEACTIVATING
            state; None otherwise.

        @see set_localization().
        '''

        if target is None:
            target = self.is_active()

        if target:
            return self.get_activate_text(language_choices)
        else:
            return self.get_deactivate_text(language_choices)

    def set_activate_text(self,
                          text: str,
                          language_code: LanguageCode = DEFAULT_LANGUAGE,
                          ) -> bool:
        '''
        Update texts describing the transition of this switch to the ACTIVE
        state, in a format that can be included as '{text}' in a statement such
        as 'cannot {text} because {state_text}'.

        @see set_localization()
        '''
        return self.set_localization(
            language_code,
            activate_text = text)

    def set_deactivate_text(self,
                            text: str,
                            language_code: LanguageCode = DEFAULT_LANGUAGE,
                            ) -> bool:
        '''
        Update texts describing the transition of this switch to the INACTIVE
        state, in a format that can be included as '{text}' in a statement such
        as 'cannot {text} because {state_text}'.

        @see set_localization()
        '''
        return self.set_localization(
            language_code,
            deactivate_text = text)

    def get_state_texts(self,
                        language_choices: LanguageChoice = DEFAULT_LANGUAGE,
                        ) -> Mapping[State, str]:
        '''
        Obtain human readable text describing each available state.

        @param language_choices
            Either a single language code or a sequence of language codes,
            which will be used in order to look up the state texts.
            See `get_localization()` for details.

        @returns
            Text description for each possible switch state, in a format
            that can be included as '{state_text}' in a statement such as
            'cannot {target_text} because {state_text}'.
        '''

        if localization := self.get_localization(language_choices):
            return localization.state_texts

    def get_state_text(self,
                       language_choices: LanguageChoice = DEFAULT_LANGUAGE,
                       ) -> str|None:
        '''
        Obtain human readable text describing the current state. If no text is
        available, returns `None`.

        @returns
            Text string describing the current state, if defined.
        '''

        if state_texts := self.get_state_texts(language_choices):
            return state_texts.get(self.status.current_state)


    def set_state_texts(self,
                        texts: Mapping[State, str],
                        language_code: LanguageCode = DEFAULT_LANGUAGE,
                        ) -> bool:
        '''
        Update texts describing possible states of this switch in a format
        that can be included as '{state_text}' in a statement such as 'cannot
        {target_text} because {state_text}'.

        @see set_localization()
        '''
        self.set_localization(language_code,
                              state_texts = texts)

    def set_aliases(self, *aliases: str):
        '''
        Set aliases for this switch. This erases any existing aliases.
        '''
        spec = Specification(aliases = aliases)
        return self.set_specification(spec, replace_aliases = True)


    def add_aliases(self, *aliases: str):
        '''
        Add one or more aliases for this swtich.
        '''
        spec = Specification(aliases = aliases)
        return self.set_specification(spec)


    def remove_aliases(self, *aliases: str):
        '''
        Remove one or more aliases for this swtich.
        '''
        new_aliases = self.aliases
        new_aliases.difference_update(aliases)
        return self.set_aliases(*new_aliases)


    def set_primary(self, primary: bool):
        '''
        Set the `is_primary` flag of this switch.

        A primary switch is considered a _root cause_ when searching for
        culprits for its own or descendants' failure to activate. That is, do
        not descend further into this switch's ancestors; "the buck stops here".
        '''
        spec = Specification(is_primary = primary)
        return self.set_specification(spec)

    def all_ancestors(self) -> Mapping[str, 'Switch']:
        '''
        Return a map of all direct and indirect ancestors to this switch, as
        determined by following its dependencies recursively until a candidate
        has no further dependencies.
        '''

        ancestors = {}
        for predecessor_name, dep in self.dependencies.items():
            if predecessor := self.board.get_switch(predecessor_name):
                ancestors[predecessor_name] = predecessor
                ancestors.update(predecessor.all_ancestors())
        return ancestors

    def ultimate_ancestors(self,
                           beyond_primary: bool = False) -> Mapping[str, 'Switch']:
        '''
        Return a map of ultimate ancestors to this switch, as determined by
        following its dependencies recursively until a candidate either has no
        further dependencies, or its `is_primary` flag is True.

        @param beyond_primary
           Keep following dependencies even through switches for which the
           `is_primary` flag is True.
        '''

        ancestors = {}
        for predecessor_name, dep in self.dependencies.items():
            if predecessor := self.board.get_switch(predecessor_name):
                if ((beyond_primary or not predecessor.is_primary)
                    and predecessor.dependencies):
                    ancestors.update(predecessor.ultimate_ancestors(beyond_primary))
                else:
                    ancestors[predecessor.name] = predecessor

        return ancestors


    def add_dependency(self,
                       predecessor: str,
                       trigger_states: StateSet = State.SETTLED,
                       polarity: DependencyPolarity = DependencyPolarity.POSITIVE,
                       hard: bool = False,
                       sufficient: bool = False,
                       allow_update: bool|None = None,
                       reevaluate: bool|None = None,
                       ) -> bool:
        '''
        Add a new upstream dependency (direct ancestor) to this switch.

        @param predecessor
            Either an existing Switch or the name of the switch on which we are
            adding a dependency.

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

        if isinstance(predecessor, Switch):
            predecessor = predecessor.name

        return self.board.add_dependency(
            self.name,
            predecessor,
            **self._inputs(locals(), ignore='predecessor'))


    def remove_dependency(self,
                          predecessor: str,
                          reevaluate: bool = True,
                          ) -> bool:
        '''
        Remove an existing dependency from a switch.

        @param predecessor
           Either an existing switch or the name of one that is to be removed as
           predecessor.

        @param reevaluate
           Recalculate state after removing dependency
        '''

        if isinstance(predecessor, Switch):
            predecessor = predecessor.name

        return self.board.remove_dependency(
            self.name,
            predecessor,
            reevaluate)

    def invoke_interceptor(self,
                           interceptor_name : str,
                           state : State|None = None,
                           ) -> Error|None:
        '''
        Manually invoke a specific interceptor, as if it were triggered by a
        switch changing its current state.  Primarily a diagnostic tool.

        @param interceptor_name
            Name of interceptor to invoke.

        @param state
            State to pass as input argument for the interceptor. If not
            provided, defaults to the current state of this switch.
        '''

        return self.board.invoke_interceptor(
            intereptor_name = interceptor_name,
            switch_name = self.name,
            state = state)

    def add_interceptor(self,
                        interceptor_name: str,
                        state_transitions: StateSet,
                        callback: InterceptorMethod,
                        phase: InterceptorPhase = InterceptorPhase.NORMAL,
                        asynchronous: bool = False,
                        rerun: bool = False,
                        immediate: bool = False,
                        future: bool = False,
                        on_cancel: ExceptionHandling = ExceptionHandling.ABORT,
                        on_error: ExceptionHandling = ExceptionHandling.FAIL,
                        ) -> bool:
        '''
        Add a new interceptor to be executed once the switch enters the
        specified state(s).

        @param interceptor_name
            Unique name/id for this interceptor.

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

        @param rerun
            Whether to invoke interceptor when (explicitly) re-entering one of
            the specified states, even if the switch is already in that state.

        @param on_cancel
            How to proceed if state change is cancelled. The default value
            `DEFAULT` is equivalent to `ABORT`.

        @param on_error
            How to proceed if the interceptor encounters an error. The default
            value `DEFAULT` is equivalent to `FAIL`.

        @param immediate
            If the interceptor's trigger states include this switch's current
            state OR the transitional state preceding it (for instance, if the
            switch is currently ACTIVE and the interceptor triggers on either
            ACTIVATING and ACTIVE), invoke it immediately. In this case, unless
            `asynchronous` flag is also True, the call blocks until the
            interceptor has completed.

        @returns
            True if the interceptor was added.
        '''

        return self.board.add_interceptor(
            switch_selection = [self.name],
            **self._inputs(locals()),
        )


    def remove_interceptor(self,
                           interceptor_name: str,
                           abandon_pending: bool = True,
                           ) -> bool:
        '''
        Remove an existing interceptor from a switch.

        @param interceptor_name
            Name of interceptor to remove

        @param abandon_pending
            Abandon any pending invocations of this interceptor.

        @returns
            True if the interceptor existed and was removed.
        '''

        return self.board.remove_interceptor(
            interceptor_name = interceptor_name,
            switch_selection = [self.name],
            abandon_pending = abandon_pending,
        )


    def set_target(self,
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
        Transition to the specified `target_state`.  If not provided, then
          * if `error` is given, the target state is inferred as `State.FAILED`;
          * otherwise, the target state is inferred based on the current state of the
            switch's dependencies.

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

        return self.board.set_target(
            switch_name = self.name,
            **self._inputs(locals()))


    def set_active(self,
                   active: bool,
                   attributes: PyValueMap|None = None,
                   clear_existing: bool = False,
                   invoke_interceptors: InvocationStyle = InvocationStyle.ALL,
                   cascade_descendants: CascadeStyle = CascadeStyle.ASYNC,
                   reenter: bool = False,
                   on_cancel: ExceptionHandling = ExceptionHandling.DEFAULT,
                   on_error: ExceptionHandling = ExceptionHandling.DEFAULT):
        '''
        Turn the switch off or on.  Equivalent to `set_target()` with
        `target_state` set to `ACTIVE` or `INACTIVE`.
        '''

        assert isinstance(active, bool), \
               "A boolean `active` flag is required"

        return self.set_target(
            target_state = (State.INACTIVE, State.ACTIVE)[bool(active)],
            **self._inputs(locals(), ignore='active'))


    def set_error(self,
                  error: Error|Exception|str,
                  attributes: PyValueMap|None = None,
                  clear_existing: bool = False,
                  invoke_interceptors: InvocationStyle = InvocationStyle.ALL,
                  cascade_descendants: CascadeStyle = CascadeStyle.ASYNC,
                  reenter: bool = True,
                  on_cancel: ExceptionHandling = ExceptionHandling.DEFAULT,
                  on_error: ExceptionHandling = ExceptionHandling.DEFAULT):
        '''
        Set the switch to FAILED state, with the provided error data.
        Equivalent to `set_target()` with an `error` argument.
        '''

        return self.set_target(
            State.FAILED,
            **self._inputs(locals()))


    def set_auto(self,
                 attributes: PyValueMap|None = None,
                 clear_existing: bool = False,
                 invoke_interceptors: InvocationStyle = InvocationStyle.ALL,
                 cascade_descendants: CascadeStyle = CascadeStyle.ASYNC,
                 reenter: bool = False,
                 on_cancel: ExceptionHandling = ExceptionHandling.DEFAULT,
                 on_error: ExceptionHandling = ExceptionHandling.DEFAULT):
        '''
        Set the switch state based on its dependencies, i.e. the value(s) of its
        immediate predecessors.  Equivalent to `set_target()` with the `target`
        argument set to `None`.

        '''
        return self.set_target(
            target_state = None,
            **self._inputs(locals()))


    def get_attributes(self,
                       inherit: bool = False) -> GetAttributesResponse:
        '''
        Obtain a key/value map of attributes assigned to this switch from
        server.

        @param inherit
            Also recursively merge in attributes from its ancestors
        '''

        return self.board.get_attributes(
            switch_name = self.name,
            inherit = inherit,
        )


    def set_attributes(self,
                       attributes: PyValueMap|None = None,
                       clear_existing: bool = False) -> bool:
        '''
        Assign arbitrary key/value pairs to this switch.
        See `set_target()` for details on the input arguments.
        '''

        return self.board.set_attributes(
            switch_name = self.name,
            **self._inputs(locals()))


    def get_dependency_statuses(self) -> DependencyStatusMap:
        '''
        Return a recursive tree of ancestors for this switch, with indication of
        each ancestor's current state and whether the dependency is satisifed.
        '''

        return self.board.get_dependency_statuses(
            switch_name = self.name,
        )


    def get_culprits(self, expected_position: bool = True) -> StatusMap:
        '''
        Obtain root causes for a switch not being in the expected positiion.

        @param expected_position
          Report culprits iff the current value of the switch is different from this.

        @returns
          Dictionary of conflicting upstream state names and their corresponding states.
        '''

        return self.board.get_culprits(
            switch_name = self.name,
        )


    def get_errors(self) -> ErrorMap:
        '''
        Obtain errors assigned to this switch and its ancestors.

        @returns
          Switch names mapped to corresponding errors.
        '''

        return self.board.get_errors(
            switch_name = self.name,
        )


    def has_error(self) -> bool:
        '''
        Indicate whether this switch is currently FAILING or FAILED.
        '''
        return self.error and (self.error.Level >= Level.ERROR)

    def is_active(self) -> bool:
        '''
        Indicate whether the switch is currently active
        '''
        return self.status.active

    def is_pending(self) -> bool:
        '''
        Indicate whether the switch is currently in a "pending" state
        (DEACTIVATING, ACTIVATING, FAILING).
        '''
        return self.status.pending

    def is_settled(self) -> bool:
        '''
        Indicate whether the switch is currently in a "settled" state
        (INACTIVE, ACTIVE, FAILED).
        '''
        return not self.status.pending

