from conan import ConanFile

class JustQueueItConan(ConanFile):
    name = "just_queue_it"
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"
    extension_properties = {"compatibility_cppstd": False}

    options = {
        "with_tests": [True, False],
        "with_backend": [True, False]
    }
    default_options = {
        "with_tests": False,
        "with_backend": False
    }

    def requirements(self):
        if self.options.with_backend:
            self.requires("boost/1.86.0")
            self.requires("reflect-cpp/0.16.0")

    def build_requirements(self):
        if self.options.with_tests:
            self.test_requires("doctest/2.4.11")
            self.test_requires("trompeloeil/49")
