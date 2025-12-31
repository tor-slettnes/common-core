#!/usr/bin/echo Do not invoke directly.
##===============================================================================n
## @file client.py
## @brief Client for REST API Services
## @author Tor Slettnes
#===============================================================================

### Modules within package
from ..http.client import HTTPClient

### Standard Python modules
import json
import asyncio
from typing import Union, Mapping, List


class RESTClient (HTTPClient):
    messaging_flavor = 'REST'

    # `service_name` should be overwritten by final subclass to look up settings
    # for this endpoint (e.g., host/port, ...).
    service_name = None

    async def get_json_async(self, rel_url, kwargs={}, headers={}) -> object:
        '''
        AsyncIO coroutine wrapper for `get_json()`

        @param rel_url:
            REST endpoint URL relative to the `base_url` passed to `__init__()`.

        @param kwargs:
            Request arguments, used to construct the HTTP query

        @param headers:
            HTTP request headers

        @returns
            Decoded JSON object, e.g. a Python dict, list, or primitive.
        '''
        return await asyncio.to_thread(self.get_json, rel_url, kwargs, headers)

    def get_json(self, rel_url, kwargs={}, headers={}):
        '''
        Request a JSON object from a REST-enabled web service.

        @param rel_url:
            REST endpoint URL relative to the `base_url` passed to `__init__()`.

        @param kwargs:
            Request arguments, used to construct the HTTP query

        @param headers:
            HTTP request headers

        @returns
            Decoded JSON object, e.g. a Python dict, list, or primitive.
        '''

        string_args = { key : json.dumps(value)
                        for (key, value) in kwargs.items() }

        response = self.get(rel_url, string_args, headers, 'application/json')
        return json.loads(response.read())

