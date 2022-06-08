#!/usr/bin/env python
# -*- coding: utf-8 -*-

from conan.tools.cmake import CMake, cmake_layout, CMakeDeps, CMakeToolchain
from conans import ConanFile
from conans.errors import ConanInvalidConfiguration
from conans.tools import Version
from os import path, remove


required_conan_version = ">=1.48.0"

class CommonLibSSE(ConanFile):
    name = "commonlibsse-ng"
    version = "3.5.0"
    license = "MIT"
    url = "https://github.com/CharmedBaryon/CommonLibSSE"
    description = "A fork of CommonLibSSE with advanced features for modern SKSE development."
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "testing": [True, False],
        "with_ae": [True, False],
        "with_se": [True, False],
        "with_vr": [True, False],
        "with_xbyak": [True, False]
    }
    default_options = {
        "testing": False,
        "with_ae": True,
        "with_se": True,
        "with_vr": True,
        "with_xbyak": False,
        "fmt:header_only": True,
        "spdlog:header_only": True
    }
    exports_sources = "CMakeLists.txt", "CMakePresets.json", "cmake/**", "include/**", "src/**", "tests/**", \
                      ".clang-format", "CommonLibSSE.natvis"

    def requirements(self):
        if self.options.testing:
            self.requires("catch2/2.13.9")
        self.requires("fmt/8.1.1")
        self.requires("rapidcsv/8.62")
        self.requires("spdlog/1.10.0")

    def layout(self):
        cmake_layout(self, generator="Ninja")

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self, generator="Ninja")
        tc.generate()

        # Project requires using existing CMake presets, remove the generated user presets file.
        user_presets = path.join(self.source_folder, "CMakeUserPresets.json")
        if path.exists(user_presets):
            remove(user_presets)

    def configure(self):
        if self.settings.os != "Windows":
            raise ConanInvalidConfiguration("Windows is the only supported OS")
        if self.settings.compiler != "Visual Studio":
            raise ConanInvalidConfiguration("Unsupported compiler provided, only Visual Studio is supported")
        if Version(self.settings.compiler.version) < "16":
            raise ConanInvalidConfiguration("Visual Studio 2019 (MSVC 16) or newer is required")
        if self.settings.arch != "x86_64":
            raise ConanInvalidConfiguration("x86_64 is the only supported architecture")

    def build(self):
        defs = {"ENABLE_SKYRIM_AE": "ON" if self.options.with_ae else "OFF",
                "ENABLE_SKYRIM_SE": "ON" if self.options.with_se else "OFF",
                "ENABLE_SKYRIM_VR": "ON" if self.options.with_vr else "OFF",
                "BUILD_TESTS": "ON" if self.options.testing else "OFF",
                "SKSE_SUPPORT_XBYAK": "ON" if self.options.with_xbyak else "OFF"}
        def_args = " ".join(['-D{}="{}"'.format(k, v) for k, v in defs.items()])
        build_type = str(self.settings.build_type).lower()

        if self.options.with_ae:
            if self.options.with_se:
                runtimes = "all" if self.options.with_vr else "flatrim"
            else:
                runtimes = "current" if self.options.with_vr else "ae"
        elif self.options.with_se:
            runtimes = "pre-ae" if self.options.with_vr else "se"
        elif self.options.with_vr:
            runtimes = "vr"
        else:
            raise ConanInvalidConfiguration("At least one Skyrim runtime must be enabled")

        self.run(f"cmake --preset=build-{build_type}-msvc-conan-{runtimes} {def_args}", cwd=self.source_folder)
        self.run(f"cmake --build --preset={build_type}-msvc-conan-{runtimes}", cwd=self.source_folder)

    def package(self):
        build_type = str(self.settings.build_type).lower()
        self.run(f'cmake --install "{path.join(self.build_folder, f"{build_type}-msvc-conan-all")}" ' +
                 f'--prefix "{self.package_folder}"', cwd=self.source_folder)
        self.copy("LICENSE*", dst="licenses",  ignore_case=True, keep_path=False)

    def package_info(self):
        self.cpp_info.libs = ["commonlibsse-ng"]
