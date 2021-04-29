#pragma once

namespace std {

    template<typename>
    struct return_type_helper;

    template<typename F, typename R, typename... Args>
    struct return_type_helper<R (F::*)(Args...) const> {
        using type = R;
    };

    template<typename F, typename R, typename... Args>
    struct return_type_helper<R (F::*)(Args...)> {
        using type = R;
    };

    template<typename C>
    struct invocation_result {
        using type = typename std::return_type_helper<decltype(&C::operator())>::type;
    };

    template<typename R, typename... Args>
    struct function {
    public:
        virtual R operator()(Args...) = 0;

        virtual R operator()(Args...) const = 0;
    };

    template<typename C, typename... Args>
    class callable_wrapper : public function<typename std::invocation_result<C>::type, Args...> {
        C callable;

    public:
        callable_wrapper(const C &obj) : callable(obj) {}

        using R = typename std::invocation_result<C>::type;

        virtual R operator()(Args... a) {
            return callable(a...);
        }

        virtual R operator()(Args... a) const {
            return callable(a...);
        }
    };

    template<typename>
    struct mk_fun;

    template<typename F, typename R, typename... Args>
    struct mk_fun<R (F::*)(Args...) const> {

        template<typename C>
        std::callable_wrapper<C, Args...> make(C obj) {
            return std::callable_wrapper<C, Args...>(obj);
        }
    };

    template<typename F, typename R, typename... Args>
    struct mk_fun<R (F::*)(Args...)> {

        template<typename C>
        std::callable_wrapper<C, Args...> make(C obj) {
            return std::callable_wrapper<C, Args...>(obj);
        }
    };

    template<typename C>
    decltype(auto) make_function(C obj) {
        auto ff = mk_fun<decltype(&C::operator())>();
        return ff.make(obj);
    }

}

