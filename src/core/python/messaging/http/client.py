#!/usr/bin/echo Do not invoke directly.
##===============================================================================n
## @file client.py
## @brief HTTP Client boilerplate
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules relative to install folder
from .base import HTTPBase, HTTPClientError, HTTPServerError

### Standard Python modules
from typing import Mapping, Any
import http.client
import urllib.parse
import logging
import threading
import os.path

class HTTPClient (HTTPBase):
    endpoint_type = 'client'

    ConnectionMap = {
        'http'  : http.client.HTTPConnection,
        'https' : http.client.HTTPSConnection
        }

    StatusMap = dict([(status.value, status) for status in http.HTTPStatus])


    def __init__(self,
                 base_url: str,
                 service_name: str|None = None,
                 project_name: str|None = None,
                 product_name: str|None = None,
                 ):

        HTTPBase.__init__(self,
                          service_name = service_name,
                          project_name = project_name,
                          product_name = product_name)
        self.base_url = self.get_target_url(base_url)

    @property
    def connection(self):
        thread_data = threading.local()
        try:
            connection = thread_data.connection
        except AttributeError:
            connection = thread_data.connection = self.create_connection(self.base_url)

        return connection


    def create_connection(self, url):
        parts = urllib.parse.urlparse(url)
        try:
            connection_type = HTTPClient.ConnectionMap[parts.scheme]
        except KeyError:
            raise TypeError("Unsupported connection scheme: %s"%(parts.scheme,),
                            parts.scheme)

        return connection_type(parts.netloc)


    async def get_async(self,
            path    : str,
            kwargs  : Mapping[str, Any] = {},
            headers : Mapping[str, str] = {},
            expected_content_type: str = None):

        return await asyncio.to_thread(self.get,
                                       path,
                                       kwargs,
                                       headers,
                                       expected_content_type)


    def get(self,
            path    : str,
            kwargs  : Mapping[str, Any] = {},
            headers : Mapping[str, str] = {},
            expected_content_type: str = None):

        parts = self.combined_parts(self.base_url, path, kwargs)
        location = urllib.parse.urlunsplit(('', '', parts.path, parts.query, ''))

        try:
            response = self.get_raw(location, headers)
        except ConnectionError as e:
            raise type(e)(e, urllib.parse.urlunsplit(parts)) from None
        else:
            return self.check_response(response, location, expected_content_type)


    def get_raw(self,
                path : str,
                headers={}):

        connection = self.connection
        request = connection.request("GET", path, headers)
        return connection.getresponse()

    def check_response(self,
                       response: http.client.HTTPResponse,
                       location: str,
                       expected_content_type: str = None) -> http.client.HTTPResponse:

        if 400 <= response.status < 500:
            error_type = HTTPClientError
        elif 500 <= response.status < 600:
            error_type = HTTPServerError
        else:
            error_type = None

        if error_type:
            try:
                status = self.StatusMap[response.status]
            except KeyError:
                error_symbol = 'Unknown'
            else:
                error_symbol = status.name

            raise error_type("Recieved HTTP error code %d (%s), location=%s"%
                             (response.status, error_symbol, location))

        if expected_content_type:
            received_content_type = response.getheader('content-type')
            if not received_content_type.startswith(expected_content_type):
                raise TypeError("Exected content type %r, received %r"%
                                (expected_content_type, received_content_type))

        return response

