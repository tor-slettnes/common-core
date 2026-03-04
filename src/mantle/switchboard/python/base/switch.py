'''
Abstract representation of a Switchboard node ("Switch")
'''

__docformat__ = 'javadoc en'
__author__ = 'Tor Slettnes'

### Standard ProtoBuf modules
from typing import Optional, Mapping, Sequence, Set, Callable
from abc import abstractmethod
from weakref import ref

### Core modules
from cc.core.decorators import virtual
from cc.core.docbase import DocBase
from cc.core.maputils import recursive_merge
from cc.core.invocation import safe_invoke
from cc.protobuf.status import Error, encodeError
from cc.protobuf.variant import (
    PyValueMap, encodeKeyValueMap, decodeKeyValueMap,
)

### Swithboard modules
from ..protobuf import (
    Specification, Status, State, StateSet, encodeStateSet,
    Dependency, DependencyMap, DependencyPolarity,
    Localization, LocalizationMap, encodeLocalization, encodeLocalizationMap,
    InterceptorSpec, InterceptorInvocation, InterceptorMethod,
    InterceptorPhase, ExceptionHandling, CascadeStyle,
    LanguageCode, LanguageChoice, LocalizationsInput,
)

InterceptorName = str
SwitchUpdateSubscriber = Callable[['Switch'], None]


class Switch (DocBase):
    '''
    Representation of a binary runtime conditition. (e.g. "Door is open",
    "Temperature control subsystem is ready", "Connected to the Internet").
    '''

    DEFAULT_LANGUAGE = "en"

    def __init__(self,
                 name: str,
                 board: 'SwitchboardBase',
                 ):
        self.name = name
        self.board = ref(board)
        self.specification = Specification()
        self.status = Status()
        self.subscriptions = {}

    def __repr__ (self):
        return "Switch(%r, %s)"%(self.name, self.current_state.name)

    def update_specification(self, specification: Specification):
        self.specification.CopyFrom(specification)
        return self.publish_update()

    def update_status(self, status: Status):
        self.status.CopyFrom(status)
        return self.publish_update()

    def publish_update(self):
        for (handle, callback) in self.subscriptions.items():
            self.publish_update_to(callback)

    def publish_update_to(self, callback: SwitchUpdateSubscriber):
        safe_invoke(callback, args = (self,))

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


    @abstractmethod
    def set_specification(self,
                          specification: Specification,
                          replace_aliases: bool = False,
                          replace_localizations: bool = False,
                          replace_dependencies: bool = False,
                          replace_interceptors: bool = False,
                          active: Optional[bool] = None,
                          update_state: Optional[bool] = None):
        '''
        Set or update the specification of this switch.
        '''

    @property
    def localizations(self) -> Mapping[str, Localization]:
        '''
        Return language codes mapped to text strings for this switch,
        including its description, texts explaining each target action
        (True/False), and texts explaining each state (INITIAL, DEACTICATING,
        INACTIVE, ACTIVATING, ACTIVE, FAILING, FAILED).
        '''
        return self.specification.localizations.map


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

        return self.set_specification(spec,
                                      replace_localizations=replace)


    def get_localization(self,
                         language_choices: LanguageChoice = DEFAULT_LANGUAGE,
                         ) -> Localization:
        '''
        Return language-specific text strings for this switch, including its
        description, texts explaining each target action (True/False), and texts
        explaining each state (INITIAL, DEACTICATING, INACTIVE, ACTIVATING,
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
            raise KeyError("No such localization(s) exist(s)", candidates)


    def set_localization(self,
                         language_code: LanguageCode = DEFAULT_LANGUAGE,
                         localization: Optional[Localization] = None,
                         *,
                         description: Optional[str] = None,
                         activate_text: Optional[str] = None,
                         deactivate_text: Optional[str] = None,
                         state_texts: Optional[Mapping[State, str]]  = None
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
            deactivate_txt = deactivate_text,
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
        return self.get_localization(language_choices).description


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

        return self.get_localization(language_choices).activate_text

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

        return self.get_localization(language_choices).deactivate_text

    def get_target_text(self,
                        language_choices: LanguageChoice = DEFAULT_LANGUAGE,
                        target: Optional[bool] = None
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

        return (self.get_activate_text(language_choices) if target
                else self.get_deactivate_text(language_choices))

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

        return self.get_localization(language_choices).state_texts

    def get_state_text(self,
                       language_choices: LanguageChoice = DEFAULT_LANGUAGE,
                       ) -> Optional[str]:
        '''
        Obtain human readable text describing the current state. If no text is
        available, returns `None`.

        @returns
            Text string describing the current state, if defined.
        '''

        return self.get_state_texts(language_choices).get(self.status.current_state)

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


    @property
    def is_primary(self) -> bool:
        '''
        Indicate whether switch is `is_primary`, i.e. whether to stop
        descending further into its dependencies when looking for culprits
        '''
        return self.specification.is_primary


    @property
    def aliases(self) -> set[str]:
        '''
        Return a list of aliaes for this string.
        '''
        return set(self.specification.aliases)


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


    @property
    def dependencies(self) -> Mapping[str, Dependency]:
        '''
        Return a map of dependencies for this switch. @sa add_dependency().
        '''
        return self.specification.dependencies.map

    @property
    def successors(self) -> Mapping[str, 'Switch']:
        '''
        Return a map of immediate successors to this switch, i.e., switches
        with a direct dependency on this one.
        '''
        successors = {}
        if board := self.board():
            for candidate in board.switches.values():
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
        if board := self.board():
            for predecessor_name, dep in self.dependencies.items():
                if predecessor := board.get_switch(predecessor_name):
                    predecessors[predecessor_name] = predecessor
        return predecessors

    @property
    def all_ancestors(self) -> Mapping[str, 'Switch']:
        '''
        Return a map of all direct and indirect ancestors to this switch, as
        determined by following its dependencies recursively until a candidate
        either has no further dependencies or its `is_primary` flag is True.
        '''

        ancestors = {}
        if board := self.board():
            for predecessor_name, dep in self.dependencies.items():
                if predecessor := board.get_switch(predecessor_name):
                    ancestors[predecessor_name] = predecessor
                    ancestors.update(predecessor.all_ancestors)
        return ancestors


    @property
    def ultimate_ancestors(self) -> Mapping[str, 'Switch']:
        '''
        Return a map of ultimate ancestors to this switch, as determined by
        following its dependencies recursively until a candidate either has no
        further dependencies or its `is_primary` flag is True.
        '''

        ancestors = {}
        if board := self.board():
            for predecessor_name, dep in self.dependencies.items():
                if predecessor := board.get_switch(predecessor_name):
                    if predecessor.is_primary or not predecessor.dependencies:
                        ancestors[predecessor.name] = predecessor
                    else:
                        ancestors.update(predecessor.ultimate_ancestors)
        return ancestors


    @abstractmethod
    def add_dependency(self,
                       predecessor: str,
                       trigger_states: StateSet = State.SETTLED,
                       polarity: DependencyPolarity = DependencyPolarity.POSITIVE,
                       hard: bool = False,
                       sufficient: bool = False,
                       allow_update: Optional[bool] = None,
                       reevaluate: Optional[bool] = None,
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


    @abstractmethod
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


    @property
    def interceptors(self) -> Mapping[str, InterceptorSpec]:
        '''
        Return a list of interceptors associated with this switch.

        @see add_interceptor()
        '''

        return self.specification.interceptors.map


    @abstractmethod
    def add_interceptor(self,
                        interceptor_name: str,
                        state_transitions: StateSet,
                        callback: InterceptorMethod,
                        phase: InterceptorPhase = InterceptorPhase.NORMAL,
                        asynchronous: bool = False,
                        rerun: bool = False,
                        on_cancel: ExceptionHandling = ExceptionHandling.ABORT,
                        on_error: ExceptionHandling = ExceptionHandling.FAIL,
                        immediate: bool = False,
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

        if board := self.board():
            return board.register_interceptor(
                switch_name = self.name,
                interceptor_name = interceptor_name,
                registration = InterceptorRegistration(
                    spec = InterceptorSpec(
                        state_transitions = encodeStateSet(state_transitions),
                        asynchronous = asynchronous,
                        phase = phase,
                        rerun = rerun,
                        on_cancel = on_cancel,
                        on_error = on_error,
                    ),
                    immediate = immediate,
                ),
                method = callback,
            )
        else:
            return False


    @abstractmethod
    def remove_interceptor(self,
                           interceptor_name: str,
                           ) -> bool:
        '''
        Remove an existing interceptor from a switch.

        @param interceptor_name
            Name of interceptor to remove

        @returns
            True if the interceptor existed and was removed.
        '''

        if board := self.board():
            return board.deregister_interceptor(
                switch_name = self.name,
                interceptor_name = interceptor_name,
            )
        else:
            return False


    @abstractmethod
    def invoke_interceptor(self,
                           interceptor_name : str,
                           state : Optional[State] = None,
                           ) -> Optional[Error]:
        '''
        Manually invoke a specific interceptor, as if it were triggered by a
        switch changing its current state.  Primarily a diagnostic tool.

        @param interceptor_name
            Name of interceptor to invoke.

        @param state
            State to pass as input argument for the interceptor. If not
            provided, defaults to the current state of this switch.
        '''


    @abstractmethod
    def on_intercept(self,
                     interceptor_name : str,
                     state : State):
        '''
        Callback handler for intercept invocations from service.
        '''


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
    def target(self) -> Optional[bool]:
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
    def current_state(self) -> State:
        '''
        Return the current state of this switch
        '''
        return State(self.status.current_state)

    @property
    def settled_state(self) -> State:
        '''
        Return the settled state of this switch. This will be one of
        INITIAL, ACTIVE, INACTIVE, FAILED.
        '''
        return State(self.status.settled_state)

    @property
    def error(self) -> Optional[Error]:
        '''
        Return any error currently associated with switch
        '''
        if self.status.error.level:
            return self.status.error
        else:
            return None


    @abstractmethod
    def set_target(self,
                   target_state: Optional[State],
                   error: Error|Exception|str|None = None,
                   attributes: Optional[PyValueMap] = None,
                   clear_existing: bool = False,
                   invoke_interceptors: bool = True,
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
            if successful, those for ACTIVE).

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


    def set_active(self,
                   active: bool,
                   attributes: Optional[PyValueMap] = None,
                   clear_existing: bool = False,
                   invoke_interceptors: bool = True,
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
            attributes = attributes,
            clear_existing = clear_existing,
            invoke_interceptors = invoke_interceptors,
            cascade_descendants = cascade_descendants,
            reenter = reenter,
            on_cancel = on_cancel,
            on_error = on_error)



    def set_error(self,
                  error: Error|Exception|str,
                  attributes: Optional[PyValueMap] = None,
                  clear_existing: bool = False,
                  invoke_interceptors: bool = True,
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
            error = encodeError(error),
            attributes = attributes,
            clear_existing = clear_existing,
            invoke_interceptors = invoke_interceptors,
            cascade_descendants = cascade_descendants,
            reenter = reenter,
            on_cancel = on_cancel,
            on_error = on_error)

    def set_auto(self,
                 attributes: Optional[PyValueMap] = None,
                 clear_existing: bool = False,
                 invoke_interceptors: bool = True,
                 cascade_descendants: CascadeStyle = CascadeStyle.ASYNC,
                 reenter: bool = False,
                 on_cancel: ExceptionHandling = ExceptionHandling.DEFAULT,
                 on_error: ExceptionHandling = ExceptionHandling.DEFAULT):
        '''
        Set the switch state based on its dependencies, i.e. the value(s) of its
        immediate predecessors.  Equivalent to `set_target()` with the `target`
        argument set to `None`.

        '''
        self.set_target(
            target_state = None,
            attributes = attributes,
            clear_existing = clear_existing,
            invoke_interceptors = invoke_interceptors,
            cascade_descendants = cascade_descendants,
            reenter = reenter,
            on_cancel = on_cancel,
            on_error = on_error)

    @property
    def attributes(self) -> PyValueMap:
        '''
        Return a dictinoary of arbitrary key/value pairs currently associated
        with this switch.
        '''

        return decodeKeyValueMap(self.status.attributes)


    @abstractmethod
    def get_attributes(self,
                       inherit: bool = False) -> PyValueMap:
        '''
        Obtain a key/value map of attributes assigned to this switch

        @param inherit
            Also recursively merge in attributes from its ancestors
        '''


    @abstractmethod
    def set_attributes(self,
                       attributes: Optional[PyValueMap] = None,
                       clear_existing: bool = False):
        '''
        Assign arbitrary key/value pairs to this switch.
        See `set_target()` for details on the input arguments.
        '''

    @property
    def cascaded_attributes(self):
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


    @abstractmethod
    def get_status(self) -> Status:
        '''
        Retrieve status of this switch from the server
        '''

    @abstractmethod
    def get_culprits(self, expected_position: bool = True) -> Mapping[str, 'Switch']:
        '''
        Obtain root causes for a switch not being in the expected positiion.

        @returns
          Dictionary of conflicting upstream state names and their corresponding states.
        '''

    @abstractmethod
    def get_errors(self) -> Mapping[str, Error]:
        '''
        Obtain errors assigned to this switch and its ancestors.

        @returns
          Dictionary of errors assigned to this switch and its ancestors
        '''

