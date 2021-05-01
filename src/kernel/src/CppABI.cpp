namespace CppABI {
    extern "C" void __cxa_pure_virtual() {
        // Called when a virtual function happens to be pure.
        // TODO: Kernel panic
    }

    // The ABI requires a 64-bit type for the thread-safe static variable initialization guard.
    __extension__ typedef int __guard __attribute__((mode(__DI__)));

    extern "C" int __cxa_guard_acquire(__guard * g) {
        return !*reinterpret_cast<char *>(g);
    }

    extern "C" void __cxa_guard_release(__guard * g) {
        *reinterpret_cast<char *>(g) = 1;
    }

    extern "C" void __cxa_guard_abort(__guard * ) {

    }
}