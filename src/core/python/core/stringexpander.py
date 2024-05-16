#!/usr/bin/python3
#===============================================================================
## @file stringexpander.py
## @brief String substitutions
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

import re
import typing
import sys
import importlib

TOKENS = (BACKSLASH, SUBEXPRESSION, SUB_NESTED, SUB_END) = \
         ('space', 'subexpression', 'sub_nested', 'sub_end')

_tokens     = {
    BACKSLASH     : r'\\',
    SUBEXPRESSION : r'\$[\{\[\(]',
    SUB_NESTED    : r'[\{\[\(]',
    SUB_END       : r'[\}\]\)]'
}

_end_tokens = {
    '${' : '}',
    '$[' : ']',
    '$(' : ')',
    '{'  : '}',
    '['  : ']',
    '('  : ')',
}

def _tokenSearch (*tokens):
    groups = [r'(?P<%s>%s)'%(t, _tokens[t]) for t in tokens]
    return re.compile("|".join(groups))


class ParseError (Exception):
    def __init__ (self, expression, pos, reason):
        self.expression    = expression
        self.pos           = pos
        self.reason        = reason

    def __str__ (self):
        return '%s <-- %s'%(self.expression[:self.pos].rstrip(), self.reason)


class VariableSyntax (ParseError):
    def __init__ (self, expression, pos=None, reason='Invalid variable syntax'):
        super().__init__(expression, pos, reason)


class PythonError (ParseError):
    def __init__ (self, name, expression, reason):
        self.name = name
        super().__init__(expression, None, reason)

    def __str__ (self):
        return ('[%s] %s <-- %s'%
                (self.name, self.expression.rstrip(), self.reason))


class Expander (object):

    _wholeX = _tokenSearch(BACKSLASH, SUBEXPRESSION, SUB_NESTED)

    def substitute(self,
                   input: typing.Union[str, typing.TextIO],
                   substitutions: typing.Mapping[str, object],
                   /,
                   **kwargs) -> str:
        '''
        Expand the provided template string by substituting values from `substitutions`
        '''

        if isinstance(input, str):
            template, instream = input, None
        elif input:
            template, instream = self.readLine(input), input
        else:
            template, instream = "", None

        kwargs.update(substitutions)
        argmap = dict(substitutions = kwargs)

        arg, text, pos, end = self._expandText(template,
                                               0,
                                               instream,
                                               self._wholeX,
                                               argmap=argmap)

        return arg


    def _expandText (self, text, pos, instream, regexp, argmap={}, skip=(), end=()):
        parts    = []
        endtoken = None

        while pos < len(text):
            match   = regexp.search(text, pos)
            if not match:
                parts.append(text[pos:])
                pos      = len(text)
                endtoken = None
                break


            index = match.lastindex
            group = match.lastgroup
            token = match.group(index)
            start = match.start(index)
            intro = text[pos:start]
            pos   = match.end(index)

            if intro:
                parts.append(intro)

            elif not parts and group in skip:
                continue

            if group in end:
                endtoken = token
                #parts.append('')
                break

            else:
                method = self.matchMethods[group]
                arg, text, pos = method(self, token, text, pos, instream, argmap)
                if arg is not None:
                    parts.append(arg)


        if parts:
            arg = ''.join(map(str, parts))
        else:
            arg = ''

        return arg, text, pos, endtoken




    #===========================================================================
    ### Processing of the backslash ('\') character

    _eolX = re.compile(r'\s*(?:\n|$)')

    def _process_backslash (self, start, text, pos, instream, argmap):
        eolMatch = self._eolX.match(text, pos)
        if eolMatch:
            pos = eolMatch.end()

            if len(text) == pos:
                try:
                    text += self.readLine(instream)
                except EOFError:
                    raise ParseError(text, None,
                                     'End of input while reading extended line')

            return '', text, pos
        else:
            return self._unescape(text, pos)


    #===========================================================================
    ### Processing of "$..." subexpressions

    _subX = _tokenSearch(BACKSLASH, SUBEXPRESSION, SUB_NESTED, SUB_END)

    def _process_subexpression (self, start, text, pos, instream, argmap):
        try:
            method = self.evalMethods[start]
        except NotImplementedError:
            return start, text, pos


        arg, text, pos, end = self._expandText(text, pos, instream,
                                               self._subX,
                                               argmap=argmap,
                                               end=(SUB_END,))

        self._check_end(text, pos, start, end)

        try:
            raw = method(self, arg, **argmap)

        except ParseError as e:
            raise
        except Exception as e:
            raw = arg.join((start, end))
#            raise
            raise ParseError(text, pos, str(e))
        else:
            if raw is None:
                arg = ''
            else:
                arg = str(raw)

        return arg, text, pos


    _nestedX = _tokenSearch(BACKSLASH, SUBEXPRESSION, SUB_NESTED, SUB_END)

    def _process_sub_nested (self, start, text, pos, instream, argmap):
        arg, text, pos, end = self._expandText(text, pos, instream,
                                               self._nestedX,
                                               argmap=argmap,
                                               end=(SUB_END,))

        arg = ''.join(filter(None, (start, arg, end)))
        return arg, text, pos


    def _process_sub_end (self, start, text, pos, instream, argmap):
        for c_start, c_end in _end_tokens.items():
            if c_end == start:
                raise ParseError(text, pos, 'Mismatched %r'%(start,))

        return start, text, pos


    def _check_end (self, text, pos, start, end):
        if not end:
            raise ParseError(text, pos, 'Missing %r at end of line'%(_end_tokens[start]))

        elif end != _end_tokens[start]:
            raise ParseError(text, pos, 'Expected %r, not %r'%(_end_tokens[start], end))

    _unescapeX = re.compile(r'[0-7]{3}|x[0-9a-fA-F]{2}|[0abnfrtv]')
    _unescapeU = re.compile(r'u[0-9a-fA-F]{4}|U[0-9a-fA-F]{8}')

    def _unescape (self, text, pos):
        match = self._unescapeX.match(text, pos)
        if match:
            return eval('"\\%s"'%match.group()), text, match.end()

        match = self._unescapeU.match(text, pos)
        if match:
            code = int(match.group()[1:], 16)
            return unichr(code).encode('utf8'), text, match.end()

        return text[pos], text, pos+1


    def to_boolean (self, value: str) -> bool:
        if isinstance(value, bool):
            return value

        elif isinstance(value, str):
            if value.lower() in ('true','yes', 'on'):
                return True
            elif value.lower() in ('false', 'no', 'off', ''):
                return False
            else:
                return bool(self.to_number(value, True))

        else:
            return bool(value)

    def to_number (self,
                   value: str,
                   default: object = None) -> typing.Union[int, float]:
        try:
            return int(value)
        except ValueError:
            try:
                return int(value, 0)
            except ValueError:
                try:
                    return float(value)
                except ValueError:
                    if default is not None:
                        return default
                    else:
                        raise


    def readLine (self, instream):
        try:
            text = instream.readline()
        except AttributeError:
            raise EOFError
        except SocketError:
            raise EOFError
        else:
            if not text:
                raise EOFError

        return text




    _escapeChars   = re.compile('(\\n)|(\\r)|(\\t)|(\')|(")|(\\$)|(\\\\)|(\x00-x1F\x7F-xA0)')
    _escapes       = (None, '\\n', '\\r', '\\t', '\\\'', '\\"', '\\$', '\\\\', None)


    def _escape (self, text, pos=0, keep=(), quote=''):
        match = self._escapeChars.search(text, pos)
        parts = [ quote ]

        while match:
            parts.append(text[pos:match.start()])
            char   = match.group()

            if char in keep:
                esc = char

            else:
                esc = self._escapes[match.lastindex] or r'\x%02X'%ord(char)

            parts.append(esc)
            pos    = match.end()
            match  = self._escapeChars.search(text, pos)

        parts.append(text[pos:])
        parts.append(quote)
        return ''.join(parts)




    matchMethods = {
        BACKSLASH     : _process_backslash,
        SUBEXPRESSION : _process_subexpression,
        SUB_NESTED    : _process_sub_nested,
        SUB_END       : _process_sub_end,
    }



    _rx_var = re.compile(r'^(#)?\s*(?:'
                         r'(\d+|@)|(\w[\w-]*)\s*(?:\[(\w*)\])?)\s*'
                         r'(?:/(/?)([^/]+)(?:/(.*))?|'
                         r':((?:\s+[\+\-]|\s*)?\d+)\s*:?\s*([+-]?\d*)\s*:?\s*([+-]?\d*)\s*|'
                         r':([+-])(.*)|'
                         r'\?([+-])(.*)?|'
                         r'(\?)([^:]*):?(.*)'
                         r')?$')

    def _getVariable (self,
                      identifier    : str,
                      substitutions : typing.Mapping[str, object]):

        portion   = None
        handling  = None
        search    = None

        match = self._rx_var.match(identifier)
        if not match:
            raise self.VariableSyntax(expression=identifier.strip())

        (hashmark,
         argindex, key, subkey,
         isregex, search, replace,
         offset, length, step,
         check, alternate,
         vassert, vtext,
         ifelse, iftext, elsetext) = match.groups()

        raw    = substitutions.get(key)
        string = raw is not None and str(raw) or ''

        if search:
            if isregex:
                string = sub(search, replace or '', string)
            else:
                string = string.replace(search, replace or '')

        elif offset:
            start, end, step = int(offset), None, None
            if length:
                end = start + int(length)
            if step:
                step = int(step)

            string = string[start:end:step]

        if hashmark is not None:
            raw = len(string)
            string = str(raw)

        if vassert:
            string = ("", vtext)[self.to_boolean(raw) == (vassert == '+')]

        elif ifelse:
            string = (elsetext, iftext)[self.to_boolean(raw)]

        elif alternate and (bool(string) == (check == '+')):
            string = alternate

        return string


    def _getCommandOutput(self,
                          text          : str,
                          substitutions : typing.Mapping[str, object]):
        raise NotImplementedError

    def _getEvaluationResult(self,
                             expression    : str,
                             substitutions : typing.Mapping[str, object],
                             imports       : typing.Sequence[str] = ('math', 're')):

        localmap = {}

        for modulename in imports:
            m = importlib.import_module(modulename)
            localmap.update(m.__dict__)

        localmap.update(substitutions)

        try:
            return eval(expression, globals(), localmap)

        except Exception as e:
            raise PythonError(type(e).__name__,
                              expression = expression.strip(),
                              reason = str(e).strip())


    evalMethods = {
        '$(' : _getCommandOutput,
        '${' : _getVariable,
        '$[' : _getEvaluationResult
    }


