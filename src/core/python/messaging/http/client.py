#!/usr/bin/echo Do not invoke directly.
##===============================================================================n
## @file client.py
## @brief HTTP Client boilerplate
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules relative to install folder
from .base import HTTPBase, HTTPClientError, HTTPServerError
from cc.core.invocation import check_type

### Standard Python modules
from typing import Mapping, Any
import http, http.client, urllib.parse
import os.path, logging

class HTTPClient (HTTPBase):
    endpoint_type = 'client'

    ConnectionMap = {
        'http'  : http.client.HTTPConnection,
        'https' : http.client.HTTPSConnection
        }

    StatusMap = dict([(status.value, status) for status in http.HTTPStatus])



    def __init__(self, base_url, service_name=None):
        HTTPBase.__init__(self, service_name)
        self.target = self.get_target(base_url)
        self.connection = self.create_connection(self.target)

    def create_connection(self, target):
        try:
            connection_type = HTTPClient.ConnectionMap[target.scheme]
        except KeyError:
            raise TypeError("Unsupported connection scheeme: %s"%(target.scheme,),
                            target.scheme)

        return connection_type(target.host, target.port)

    def get_raw(self,
                rel_path : str,
                kwargs  : Mapping[str, Any] = {},
                headers={}):

        path = self.full_path(rel_path, kwargs)
        try:
            request = self.connection.request("GET", path, headers)
            return self.connection.getresponse()
        except ConnectionError as e:
            scheme, host, port, _ = self.target
            url = self.join_url(scheme, host, port, path)
            raise type(e)("%s (URL=%s)"%(e, url), url) from None

    def get(self,
            rel_url,
            kwargs={},
            headers={},
            expected_content_type: str = None):

        response = self.get_raw(rel_url, kwargs, headers)

        if 400 <= response.status < 500:
            error_type = HTTPClientError
        elif 500 <= response.status < 600:
            error_type = HTTPServerError
        else:
            error_type = None

        if error_type:
            try:
                status = HTTPClient.StatusMap[response.status]
            except KeyError:
                raise "Received unknown HTTP error code %d"%(response.status)
            else:
                raise error_type("Recieve HTTP error code %d: [%s] %s"%
                                 (status.value,
                                  status.name,
                                  status.description))

        if expected_content_type:
            received_content_type = response.getheader('content-type')
            if not received_content_type.startswith(expected_content_type):
                raise TypeError("Exected content type %r, received %r"%
                                (expected_content_type, received_content_type))

        return response


    def full_path(self, rel_path, kwargs={}):
        check_type(rel_path, str)

        if self.target.path.endswith('/') and rel_path.startswith('/'):
            path = self.target.path[:-1] + rel_path
        elif self.target.path or rel_path:
            path = self.target.path + rel_path
        else:
            path = '/'

        query = "&".join([
            "=".join((key, urllib.parse.quote_plus(arg)))
            for (key, arg) in kwargs.items()
            ])

        return "?".join([part for part in (path, query) if part])
