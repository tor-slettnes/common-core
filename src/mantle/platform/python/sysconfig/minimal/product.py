'''
Minimal Python-native lookups for SysConfig information.
This does not use the SysConfig gRPC service.
'''

__author__ = 'Tor Slettnes'
__docformat__ = 'javadoc en'

### Common Core modules
from cc.core.settingsstore import SettingsStore
from cc.protobuf.version import version_to_string

### SysConfig modules
from ..protobuf import ProductInfo


class Product:
    '''
    Minimal in-process lookup of product information.
    '''

    RELEASE_SETTINGS_FILE = "release.json"
    RELEASE_SETTING_PRODUCT = "product"
    RELEASE_SETTING_DESCRIPTION = "description"
    RELEASE_SETTING_VERSION = "version"

    PRODUCT_SETTINGS_FILE = "product.json"
    PRODUCT_SETTING_SERIAL = "serial number"
    PRODUCT_SETTING_MODEL = "hardware model"


    @classmethod
    def get_product_info(cls) -> ProductInfo:
        '''
        Get information about this product:
        model, serial number, versions, subsystems
        '''
        release_settings = SettingsStore(cls.RELEASE_SETTINGS_FILE)
        product_settings = SettingsStore(cls.PRODUCT_SETTINGS_FILE)

        return ProductInfo(
            product_name = release_settings.get(cls.RELEASE_SETTING_PRODUCT),
            product_description = release_settings.get(cls.RELEASE_SETTING_PRODUCT),
            release_version = product_settings.get(cls.RELEASE_SETTING_VERSION),
            serial_number = product_settings.get(cls.PRODUCT_SETTING_SERIAL),
            model_name = product_settings.get(cls.PRODUCT_SETTING_MODEL),
        )

    def get_printable_version(cls) -> str:
        '''
        Get version number as a printable string.

        @param[in] product_info
            Previously-obtained response from `get_product_info()`, if any.

        '''
        product_info = cls.get_product_info()
        version_to_string(product_info.release_version)

