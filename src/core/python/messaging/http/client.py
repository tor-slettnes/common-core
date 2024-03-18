#!/usr/bin/echo Do not invoke directly.
##===============================================================================n
## @file client.py
## @brief HTTP Client boilerplate
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules relative to install folder
from .base import HTTPBase

### Standard Python modules
from typing import Mapping, Any
import http.client
import urllib.parse
import logging
import os.path

class HTTPClient (HTTPBase):
    endpoint_type = 'client'

    ConnectionMap = {
        'http'  : http.client.HTTPConnection,
        'https' : http.client.HTTPSConnection
        }

    StatusMap = dict([(status.value, status) for status in http.HTTPStatus])


    def __init__(self, base_url, service_name=None):
        HTTPBase.__init__(self, service_name)
        self.base_url = self.get_target_url(base_url)
        self.connection = self.create_connection(self.base_url)


    def create_connection(self, url):
        parts = urllib.parse.urlparse(url)
        try:
            connection_type = HTTPClient.ConnectionMap[parts.scheme]
        except KeyError:
            raise TypeError("Unsupported connection scheeme: %s"%(parts.scheme,),
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
        response = self.get_raw(location, headers)
        return self.check_response(response, expected_content_type)


    def get_raw(self,
                path : str,
                headers={}):

        try:
            request = self.connection.request("GET", path, headers)
            return self.connection.getresponse()
        except ConnectionError as e:
            scheme, host, port, _ = self.target
            url = self.join_url(scheme, host, port, path)
            raise type(e)(e, url) from None


    def check_response(self,
                       response: http.client.HTTPResponse,
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
                raise error_type("Received unknown HTTP error code %d, location=%s"%
                                (response.status, location))
            else:
                raise error_type("Recieve HTTP error code %d (%s), location=%s"%
                                 (status.value, status.name, location))

        if expected_content_type:
            received_content_type = response.getheader('content-type')
            if not received_content_type.startswith(expected_content_type):
                raise TypeError("Exected content type %r, received %r"%
                                (expected_content_type, received_content_type))

        return response

