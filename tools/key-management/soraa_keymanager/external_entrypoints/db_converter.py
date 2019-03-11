from argparse import ArgumentParser, RawTextHelpFormatter

import sys

from soraa_keymanager.core_utils.config_loader import ConfigLoader
from soraa_keymanager.external_utils.atmel_dongles_controller import AtmelDonglesController
from soraa_keymanager.storage.db_storage import DBStorage
from soraa_keymanager.storage.keys_tinydb_storage import KeysTinyDBStorage
from soraa_keymanager.storage.tl_version_tinydb_storage import TLVersionTinyDBStorage
from soraa_keymanager.ui import UI
from soraa_keymanager.storage.tinydb_storage_extensions import SignedByteStorage, CryptoByteStorage
import os


class DbConverter(object):

    def __init__(self):
        self._args = None
        self.__config = self.__load_configs()
        self.__ui = UI()
        self._emulator = None
        self._emalator_mode = None
        self._atmel_util_path = None
        self.__atmel = AtmelDonglesController(self.__atmel_util_path, self.__emulator_mode)
        self._db_folder = None
        self._converted_folder = None
        self._upper_level_keys_db = None
        self.__signed_db_list = list()
        self.__crypted_db_list = list()

    def __get_db_lists(self):
        file_list = os.listdir(self.__db_folder)
        for filename in file_list:
            if os.path.isfile(os.path.join(self.__db_folder, filename)):
                if filename.endswith(".db"):
                    if "Private" in filename:
                        self.__crypted_db_list.append(filename.replace(".db", ""))
                    else:
                        self.__signed_db_list.append(filename.replace(".db", ""))

    def __read_from_gdm(self, db_name):
        db = DBStorage(os.path.join(self.__db_folder, db_name))
        data = db.get_all_data()
        return data

    def __save_to_tiny(self, data, db_name, db_type="signed"):
        db_types = {
            "crypted": CryptoByteStorage,
            "signed": SignedByteStorage
        }
        if db_name != "TrustListVersions":
            db = KeysTinyDBStorage(
                os.path.join(self.__converted_folder, db_name),
                storage_type=db_types[db_type],
                storage_kwargs={
                    "atmel": self.__atmel,
                    "ui": self.__ui,
                    "upper_level_keys_db": self.__upper_level_keys_db
                }
            )
        else:
            db = TLVersionTinyDBStorage(
                os.path.join(self.__converted_folder, db_name),
                storage_type=db_types[db_type],
                storage_kwargs={
                    "atmel": self.__atmel,
                    "ui": self.__ui,
                    "upper_level_keys_db": self.__upper_level_keys_db
                }
            )
        for key in data.keys():
            db.save(key, data[key])

    def __convert(self):
        self.__ui.print_message("Start convert db's...")
        for db_name in self.__signed_db_list:
            self.__ui.print_message("Converting {}...".format(db_name))
            data = self.__read_from_gdm(db_name)
            self.__save_to_tiny(data, db_name)
            self.__ui.print_message("Convert finished.")

        for db_name in self.__crypted_db_list:
            self.__ui.print_message("Converting {}...".format(db_name))
            data = self.__read_from_gdm(db_name)
            self.__save_to_tiny(data, db_name, db_type="crypted")
            self.__ui.print_message("Convert finished.")
        self.__ui.print_message("All convert finished.")

    def run(self):
        self.__get_db_lists()
        self.__convert()

    @property
    def __args(self):
        if not self._args:
            arguments = ArgumentParser(
                description='Database converter for keymanagement tools',
                formatter_class=RawTextHelpFormatter
            )
            arguments.add_argument("input_folder", nargs=1, type=str,
                                   help="Folder contains plain Databases")
            arguments.add_argument("output_folder", nargs=1, type=str,
                                   help="Folder to store converted Databases")
            arguments.add_argument('-c', "--config", metavar="CONFIG_PATH", type=str, help="custom configuration file")
            arguments.add_argument('-e', "--emulator", action="store_true",
                                   help="emulator usage")
            arguments.add_argument('-d', '--development', action='store_true', help="development mode")
            self._args = vars(arguments.parse_args())
        return self._args

    def __load_configs(self):
        # Try load config from argument variable, if arg not set try from default path
        try:
            if self.__args['config']:
                config_path = os.path.abspath(self.__args['config'])
            else:
                if sys.platform == "win32":
                    config_path = os.path.join(
                        os.environ(os.getenv('LOCALAPPDATA'), "keymanager\\keymanager.conf"))
                else:
                    home_config_path = os.path.join(os.getenv("HOME"), ".keymanager", "keymanager.conf")
                    if os.path.exists(home_config_path):
                        config_path = home_config_path
                    else:
                        config_path = "/etc/keymanager/keymanager.conf"

            config = ConfigLoader(config_path).get_config()

            if config:
                self.__check_configs(config)
                return config
            else:
                sys.exit("[FATAL]: Configuration file is empty! Please setup config at {}".format(config_path))
        except IOError as error:
            sys.exit(error)

    def __check_configs(self, config):
        sections = ["MAIN"]
        important_keys_main = ["dongles_cli_path", "dongles_cli_emulator_path"]
        for section in sections:
            if section not in config.keys():
                sys.exit("[FATAL]: Missing section {} in config".format(section))

        for important_key in important_keys_main:
            if important_key not in config["MAIN"].keys():
                sys.exit("Missing config parameter {} in section MAIN".format(important_key))

    @property
    def __atmel_util_path(self):
        if not self._atmel_util_path:
            if self.__emulator:
                if "dongles_cli_emulator_path" in self.__config["MAIN"].keys():
                    self._atmel_util_path = self.__config["MAIN"]["dongles_cli_emulator_path"]
            else:
                if "dongles_cli_path" in self.__config["MAIN"].keys():
                    self._atmel_util_path = self.__config["MAIN"]["dongles_cli_path"]

            if not self._atmel_util_path:
                self._atmel_util_path = os.path.join(
                    os.path.dirname(sys.modules["soraa_keymanager"].__file__),
                    "external_utils",
                    "util",
                    "emulator" if self.__emulator else "origin",
                    "soraa-dongles-cli"
                )
        return self._atmel_util_path

    @property
    def __db_folder(self):
        if not self._db_folder:
            self._db_folder = self.__args["input_folder"][0]
        return self._db_folder

    @property
    def __converted_folder(self):
        if not self._converted_folder:
            self._converted_folder = self.__args["output_folder"][0]
        return self._converted_folder

    @property
    def __emulator(self):
        if not self._emulator:
            self._emulator = "emulator" in self.__args.keys() and self.__args["emulator"]
        return self._emulator

    @property
    def __emulator_mode(self):
        if not self._emalator_mode:
            if self.__emulator:
                self._emalator_mode = "dev" if "development" in self.__args and self.__args["development"] else "main"
        return self._emalator_mode

    @property
    def __upper_level_keys_db(self):
        if not self._upper_level_keys_db:
            if "UpperLevelKeys" in self.__signed_db_list:
                self._upper_level_keys_db = DBStorage(os.path.join(self.__db_folder, "UpperLevelKeys"))
            else:
                sys.exit("[FATAL]: Can't find UpperLevelKeys db")
        return self._upper_level_keys_db