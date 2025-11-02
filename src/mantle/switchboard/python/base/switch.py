'''
Abstract representation of a Switchboard node ("Switch")
'''

__all__ = ['Switch']
__docformat__ = 'javadoc en'
__author__ = 'Tor Slettnes'


### Modules within package
from ..protobuf import Specification, Status, State, \
    DependencyMap, DependencyPolarity, InterceptorPhase, ExceptionHandling, \
    Localization, LocalizationMap, encodeLocalization, encodeLocalizationMap, \
    LanguageCode, LanguageChoice, LocalizationsInput

from cc.protobuf.status import Error
from cc.protobuf.variant import PyValueDict, encodeKeyValueMap
from cc.core.invocation import safe_invoke

from typing import Optional
from collections.abc import Sequence, Mapping, Callable
from abc import abstractmethod

class Switch:
    '''
    Representation of a binary runtime conditition. (e.g. "Door is open",
    "Temperature control subsystem is ready", "Connected to the Internet").
    '''

    DEFAULT_LANGUAGE = "en"

    def __init__(self, name: str):
        self.name = name
        self.specification = Specification()
        self.status = Status()
        self.callbacks = {}

    def __repr__ (self):
        return "Switch(%r)"%(self.name,)

    def _update_specification(self, specification: Specification):
        self.specification.CopyFrom(specification)
        self._invoke_callbacks("specification")

    def _update_status(self, status: Status):
        self.status.CopyFrom(status)
        self._invoke_callbacks("status")

    def _invoke_callbacks(self, notification_type: str):
        for (handle, callback) in self.callbacks.items():
            safe_invoke(callback,
                        args = (self,),
                        description = ('Switch %s notification callback %r'%
                                       (notification_type, handle,)))

    def subscribe_updates(self,
                          handle   : str,
                          callback : Callable):
        '''
        Register a callback to be invoked whenever this switch changes specification or status

        @param handle:
            Unique handle for this callback. If a callback is already
            registered with the same handle, it will be replaced.

        @param callback:
            Method to be invoked whenever this switch changes specification or state.
            Must take this switch instance as its sole argument.
        '''

        self.callbacks[handle] = callback


    def unsubscribe_updates(self,
                            handle: str) -> bool:
        '''
        Unregiser a callback handler

        @param handle:
            Handle for the registered callback.

        @returns
            True if the specified handle was found and removed, False otherwise.
        '''

        return self.callbacks.pop(handle, None) is not None


    @abstractmethod
    def set_specification(self,
                          specification: Specification,
                          replace_localizations: bool = False,
                          replace_dependencies: bool = False,
                          replace_interceptors: bool = False,
                          active: Optional[bool] = None,
                          update_state: Optional[bool] = None):
        '''
        Set or update the specification of this switch.
        '''

    @property
    def localizations(self) -> LocalizationMap:
        '''
        Return language codes mapped to text strings for this switch,
        including its description, texts explaining each target action
        (True/False), and texts explaining each state (INITIAL, DEACTICATING,
        INACTIVE, ACTIVATING, ACTIVE, FAILING, FAILED).
        '''
        return self.specification.localizations


    def set_localizations(self,
                          localizations: LocalizationsInput,
                          replace: bool = False,
                          ) -> bool:
        '''
        Update localization map for this switch.

        @param localization:
            Either of the following: a preconstructed ProtoBuf `LocalizationMap`
            message, or language codes mapped to ProtoBuf `Localization` types,
            or language codes mapped to dictionaries comprising `description`,
            `target_texts`, and/or `state_texts` keys.  In the latter case,
            `target_texts` is a mapping from boolean to string, and `state_texts`
            a mapping from `cc.protobuf.switchboard.State` to string.
        '''

        spec = Specification(
            switch_name = self.name,
            localizations = encodeLocalizationMap(localizations),
            replace_localizations = replace)

        return self.set_specification(spec)


    def get_localization(self,
                         language_choices: LanguageChoice = DEFAULT_LANGUAGE,
                         ) -> Localization:
        '''
        Return language-specific text strings for this switch, including its
        description, texts explaining each target action (True/False), and texts
        explaining each state (INITIAL, DEACTICATING, INACTIVE, ACTIVATING,
        ACTIVE, FAILING, _FAILED).

        @param language_choices:
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
                         target_texts: Optional[Mapping[bool, str]] = None,
                         state_texts: Optional[Mapping[State, str]]  = None
                         ) -> bool:
        '''
        Change or add localization for this switch.

        @param language_code:
            ISO language code, optionally followed by underscore and country code
            (e.g. "en" or "en_US")

        @param localization:
            Preconstructed ProtoBuf `Localization` message.
            Alternatively this can be constructed from the following arguments.

        @param description:
            Description of this switch

        @param target_texts:
            Update texts describing each boolean target transition (False, True).

        @param state_texts:
            Description of each possible state.

        Mapped strings for `target_texts` and `state_texts` should be suitable
        for building error messages about dependency conflicts, such as
        "cannot {target_text} because {state_text}".

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
              target_text = {
                  True: "lock the door",
                  False: "unlock the door"}

          door_locked.set_target(True)
          ```

        Possible failure:
          ```
          >>> RuntimeError: Cannot lock the door because the door is open
          ```
        '''

        update = encodeLocalization(
            description = description,
            target_texts = target_texts,
            state_texts = state_texts)

        return self.set_localizations({language_code: update})

    def get_description(self,
                        language_choices: LanguageChoice = DEFAULT_LANGUAGE,
                        ) -> str:
        '''
        Return human readable description of this switch, e.g. `"door lock"`

        @param language_choices:
            Either a single language code or a sequence of language codes,
            which will be used in order to look up the switch description.
            See `get_localization()` for details.
        '''
        return self.get_localization(language_choices).description


    def get_target_texts(self,
                         language_choices: LanguageChoice = DEFAULT_LANGUAGE,
                         ) -> Mapping[bool, str]:
        '''
        Obtain human readable text describing each target position (False, True).

        @param language_choices:
            Either a single language code or a sequence of language codes,
            which will be used in order to look up the target texts.
            For details, @see get_localization().

        @returns
            Each boolean target (False, True) mapped to explanation of the
            correponding operation; @see set_localization().
        '''

        return self.get_localization(language_choices).target_texts

    def get_target_text(self,
                        language_choices: LanguageChoice = DEFAULT_LANGUAGE,
                        ) -> str:
        '''
        If the switch is currently in the ACTIVATING or DEACTIVATING state,
        return a human readable text describing the corresponding target
        possition; otherwise None.

        @param language_choices:
            Either a single language code or a sequence of language codes,
            which will be used in order to look up the target texts.
            For details, @see get_localization().

        @returns
            A human readable text describing the current target position
            iff the switch is currently in the ACTIVATING or DEACTIVATING
            state; None otherwise.

        @see set_localization().
        '''

        return self.get_localization(language_choices).target_texts

    def set_target_texts(self,
                         texts: Mapping[bool, str],
                         language_code: LanguageCode = DEFAULT_LANGUAGE,
                         ) -> bool:
        '''
        Update texts describing each boolean target transition (False, True)
        in a format that can be included as '{target_text}' in a statement such
        as 'cannot {target_text} because {state_text}'.

        @see set_localization()
        '''
        return self.set_localization(
            language_code,
            target_texts = texts)

    def get_state_texts(self,
                        language_choices: LanguageChoice = DEFAULT_LANGUAGE,
                        ) -> Mapping[State, str]:
        '''
        Obtain human readable text describing each available state.

        @param language_choices:
            Either a single language code or a sequence of language codes,
            which will be used in order to look up the target texts.
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
        self._update_spec(state_texts=texts)


    @property
    def is_primary(self) -> bool:
        '''
        Indicate whether switch is `is_primary`, i.e. whether to stop
        descending further into its dependencies when looking for culprits
        '''
        return self.specification.is_primary


    def set_primary(self, primary: bool):
        '''
        Set the `is_primary` flag of this switch.

        A primary switch is considered a _root cause_ when searching for
        culprits for its own or descendants' failure to activate. That is, do
        not descend further into this switch's ancestors; "the buck stops here".
        '''

        spec = Specification(
            switch_name = self.name,
            is_parimary = primary)

        return self.set_specification(spec)


    @property
    def dependencies(self) -> DependencyMap:
        '''
        Return a list of dependencies for this switch. @sa add_dependency().
        '''
        return self.specification.dependencies


    @abstractmethod
    def add_dependency(self,
                       predecessor_name: str,
                       trigger_states: int = State.SETTLED,
                       polarity: DependencyPolarity = DependencyPolarity.POSITIVE,
                       hard: bool = False,
                       sufficient: bool = False,
                       allow_update: Optional[bool] = None,
                       reevaluate: Optional[bool] = None,
                       ) -> bool:
        '''
        Add a new upstream dependency (direct ancestor) to this switch.

        @param predecessor_name:
            Name of the switch on which we are adding a dependency.

        @param trigger_states:
            A bitmask representing which of the predecessor's state transitions
            that will automatically trigger a reevaluation of this switch's
            state, based on this and its other dependencies. A value of zero
            means that this switch does not automatically update its state in
            response to a change in this predecessor. See also `set_auto()`.
            The default value, `State.SETTLED`, is equivalent to `State.ACTIVE |
            State.INACTIVE | State.FAILED`.

        @param polarity:
            Whether this is a normal/positive dependency, a negative/conflicting
            dependency, or this switch toggles/flip-flops in response to the
            predecessor's value changing.

        @param hard:
            Hard dependency: This switch cannot be set unless this dependency is
            satisfied

        @param sufficient:
            Whether or not this dependency alone is sufficient to activate this
            switch, irrespective of other dependencies. This implies a logical
            OR instead of AND conditition, and as a side effect, this dependency
            becomes redundant if the successor's other dependencies are
            satisfied.

        @param allow_update:
            Allow existing dependency to be updated

        @param reevaluate:
            Recalculate this switch's state after adding the dependency.
        '''


    @abstractmethod
    def remove_dependency(self,
                          predecessor_name: str,
                          reevaluate: bool = True,
                          ) -> bool:
        '''
        Remove an existing dependency from a switch.

        @param predecessor_name:
           Name of predecessor that is being removed
        @param reevaluate:
           Recalculate state after removing dependency
        '''


    @property
    def interceptors(self) -> list:
        '''
        Return a list of interceptors associated with this switch.

        @see add_interceptor()
        '''

        return self.specification.interceptors.map


    @abstractmethod
    def add_interceptor(self,
                        interceptor_name: str,
                        state_transitions: int,
                        asynchronous: bool = False,
                        rerun: bool = False,
                        on_cancel: ExceptionHandling = ExceptionHandling.DEFAULT,
                        on_error: ExceptionHandling = ExceptionHandling.DEFAULT,
                        immediate: bool = False,
                        ) -> bool:
        '''
        Add a new interceptor to be executed once the switch enters the
        specified state(s).

        @param interceptor_name:
            Unique name/id for this interceptor.

        @param state_transitions:
            A bitmask representing states for which the inerceptor is invoked.
            Often just a single transitional state, i.e., `ACTIVATING`,
            `DEACTIVATING` or `FAILING`.

        @param asynchronous:
            Allow state to transition to the next state (normally `ACITVE`,
            `INACTIVE` or `FAILED`) even as this interceptor continues to run in
            the background.

        @param rerun:
            Whether to invoke interceptor when (explicitly) re-entering one of
            the specified states, even if the switch is already in that state.

        @param on_cancel:
            How to proceed if state change is cancelled. The default value
            `DEFAULT` is equivalent to `ABORT`.

        @param on_error:
            How to proceed if the interceptor encounters an error. The default
            value `DEFAULT` is equivalent to `FAIL`.

        @param immediate:
            If True, and if the `states` mask include the current state of this
            switch, invoke the interceptor immediately. In this case, unless
            `asynchronous` flag is also True, the call blocks until the
            interceptor has completed.

        @returns
            True if the interceptor was added.
        '''

    @abstractmethod
    def remove_interceptor(self,
                           interceptor_name: str,
                           ) -> bool:
        '''
        Remove an existing interceptor from a switch.

        @param interceptor_name:
            Name of interceptor to remove

        @returns
            True if the interceptor existed and was removed.
        '''

    @abstractmethod
    def invoke_interceptor(self,
                           interceptor_name : str,
                           state : Optional[int] = None,
                           ) -> Optional[Error]:
        '''
        Manually invoke a specific interceptor, as if it were triggered by a
        switch changing its current state.  Primarily a diagnostic tool.

        @param interceptor_name:
            Name of interceptor to invoke.

        @param state:
            State to pass as input argument for the interceptor. If not
            provided, defaults to the current state of this switch.
        '''

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
        ### TODO: Check boolean value of this if no error is returned.
        return self.status.error


    @abstractmethod
    def set_target(self,
                   target_state: Optional[State],
                   error: Optional[Error] = None,
                   attributes: Optional[PyValueDict] = None,
                   clear_existing: bool = False,
                   with_interceptors: bool = True,
                   trigger_descendants: bool = True,
                   reevaluate: bool = False,
                   on_cancel: ExceptionHandling = ExceptionHandling.DEFAULT,
                   on_error: ExceptionHandling = ExceptionHandling.DEFAULT,
                   ) -> bool:
        '''
        Transition to the specified `target_state`.  If not provided, then
          * if `error` is given, the target state is inferred as `State.FAILED`;
          * otherwise, the target state is inferred based on the current state of the
            switch's dependencies.

        @param target_state:
            Desired target state. Normally this is one of the "settled" states
            (ACTIVE, INACTIVE, or FAILED), in which case the switch will first
            change to the corresponding pending state (ACTIVATING, DEACTIVATING,
            FAILING), triggering any associated descendant updates and
            interceptor invocations on the way.

        @param error:
            Any error data associated with this switch. Ignored if the target is
            specified but is not one of `FAILING` or `FAILED`.

        @param attributes:
            Arbitrary key/value pairs assigned to the switch. These may be
            cleared in a future state change.

        @param clear_existing:
            Clear all existing attributes before setting those provided here.

        @param with_interceptors:
            Run interceptors associated with each state transition (e.g.  if
            `target_state` is ACTIVE, first run interceptors for ACTIVATING, and
            if successful, those for ACTIVE).

        @param trigger_descendants:
            Propagate the update to the switch's descendants, starting with its
            immediate successors. Only switches with dependencies that include
            the corresponding state transition(s) of this switch are
            reevaluated.

        @param reevaluate:
            Make the transition (via the applicable pending state) even if the
            switch is already in the desired target state, invoking any relevant
            interceptors on the way.

        @param on_cancel:
            What to do if the state transition is cancelled, e.g. if pre-empted
            by another target setting while executing interceptrs.  If omitted,
            the highest-numbered `on_cancel` attribute amongst the associated
            interceptors is used.

        @param on_error:
            What to do if the interceptors associated with the state transition
            encounters errors.  If omitted, the highest-numbered `on_error`
            attribute amongst the associated interceptors is used.

        @returns
            True iff the target state was modified
        '''


    def set_active(self,
                   active: bool,
                   attributes: Optional[PyValueDict] = None,
                   clear_existing: bool = False,
                   with_interceptors: bool = True,
                   trigger_descendants: bool = True,
                   reevaluate: bool = False,
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
            with_interceptors = with_interceptors,
            trigger_descendants = trigger_descendants,
            reevaluate = reevaluate,
            on_cancel = on_cancel,
            on_error = on_error)



    def set_error(self,
                  error: Error,
                  attributes: Optional[PyValueDict] = None,
                  clear_existing: bool = False,
                  with_interceptors: bool = True,
                  trigger_descendants: bool = True,
                  reevaluate: bool = True,
                  on_cancel: ExceptionHandling = ExceptionHandling.DEFAULT,
                  on_error: ExceptionHandling = ExceptionHandling.DEFAULT):
        '''
        Set the switch to FAILED state, with the provided error data.
        Equivalent to `set_target()` with an `error` argument.
        '''

        return self.set_target(
            State.FAILED,
            error = error,
            attributes = attributes,
            clear_existing = clear_existing,
            with_interceptors = with_interceptors,
            trigger_descendants = trigger_descendants,
            reevaluate = reevaluate,
            on_cancel = on_cancel,
            on_error = on_error)

    def set_auto(self,
                 attributes: Optional[PyValueDict] = None,
                 clear_existing: bool = False,
                 with_interceptors: bool = True,
                 trigger_descendants: bool = True,
                 reevaluate: bool = False,
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
            with_interceptors = with_interceptors,
            trigger_descendants = trigger_descendants,
            reevaluate = reevaluate,
            on_cancel = on_cancel,
            on_error = on_error)


    @abstractmethod
    def set_attributes(self,
                       attributes: Optional[PyValueDict] = None,
                       clear_existing: bool = False):
        '''
        Assign arbitrary key/value pairs to this switch.
        See `set_target()` for details on the input arguments.
        '''

    @property
    def attributes(self) -> PyValueDict:
        '''
        Return a dictinoary of arbitrary key/value pairs currently associated
        with this switch.
        '''

        return decodeKeyValueMap(self.status.attributes)


    @abstractmethod
    def get_culprits(self, expected_position: bool = True) -> Mapping[str, Status]:
        '''
        Obtain root causes for a switch not being in the expected positiion.

        @returns
          Dictionary of conflicting upstream state names and their corresponding states.
        '''
