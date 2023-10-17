from pathlib import Path

from conan import ConanFile
from conan.tools.files import copy

class HelloRecipe(ConanFile):
    name = "msd-cpp-utils"
    version = "0.0.1"

    description = "utilities for c++ to deal with endianess and byte decoding"
    author = "github:msd"
    topics = ("endianess", "conversion", "bytes")
    homepage = "https://github.com/msd/cpp-utils"
    url = "https://github.com/msd/cpp-utils"
    exports_sources = "*.hpp"

    package_type = "header-library"
    no_copy_source = True

    def package(self):
        pkg_dir = Path(self.package_folder)
        copy(self, "*.hpp", self.source_folder, str(pkg_dir / "include" / "msd_utils"))

    def package_info(self):
        pkg_dir = Path(self.package_folder)
        self.cpp_info.includedirs = [str(pkg_dir / "include")]
        # For header-only packages, libdirs and bindirs are not used
        # so it's necessary to set those as empty.
        self.cpp_info.bindirs = []
        self.cpp_info.libdirs = []
