#pragma once

#ifndef QUTILITY_CML_OPT_DISABLE_CML_OPT

#include <type_traits>
#include <iostream>
#include <iomanip>
#include <memory>
#include <vector>
#include <functional>

#include "boost/program_options.hpp"
#include "boost/json.hpp"

namespace qutility
{
    namespace cml_opt
    {
        class OptionsBase
        {
        public:
            using vm_T = boost::program_options::variables_map;
            using osd_T = boost::program_options::options_description;
            using od_T = boost::program_options::option_description;
            using jsobject_T = boost::json::object;

        protected:
            template <typename val_t>
            static std::string to_string(const val_t &val) { return std::to_string(val); }
            static std::string to_string(const std::string &str) { return str; }
        };

        template <typename OptionsDerivedT, typename = typename std::enable_if_t<std::is_base_of_v<OptionsBase, OptionsDerivedT>>>
        void obtain_cml_options(int argc, const char *const *argv, OptionsDerivedT &opts)
        {
            namespace po = boost::program_options;
            try
            {
                po::options_description desc("Allowed options");
                desc.add_options()("help", "generate help information");
                opts.register_all(desc);
                po::variables_map vm;
                po::store(po::parse_command_line(argc, argv, desc), vm);
                po::notify(vm);
                if (vm.count("help"))
                {
                    std::cout << desc << "\n";
                    exit(0);
                }
                opts.get_all(vm);
            }
            catch (po::error &e)
            {
                std::cout << e.what() << std::endl;
                exit(-1);
            }
        }
    }
}

#ifndef QUTILITY_REGISTER_OPTION

#define QUTILITY_REGISTER_OPTION_IMPL_VALUE_AND_COMMON_FUNCTIONS(type, name, tag) \
public:                                                                           \
    type name;                                                                    \
                                                                                  \
protected:                                                                        \
    constexpr static const char *name##_tag = tag;                                \
    void name##_get_func(const vm_T &vm)                                          \
    {                                                                             \
        name = vm[name##_tag].as<type>();                                         \
    }                                                                             \
    std::pair<std::string, std::string> name##_str_func() const                   \
    {                                                                             \
        return {name##_tag, to_string(name)};                                     \
    }                                                                             \
    void name##_json_insert_func(jsobject_T &obj) const                           \
    {                                                                             \
        obj[name##_tag] = name;                                                   \
    }

#define QUTILITY_REGISTER_OPTION_IMPL_REGISTER_FUNCTION_WITH_DEFAULT(type, name, cml_token, default, desc) \
    void name##_register_func(osd_T &osd) const                                                            \
    {                                                                                                      \
        boost::shared_ptr<od_T> opt(new od_T(                                                              \
            cml_token,                                                                                     \
            boost::program_options::value<type>()->default_value(default),                                 \
            desc));                                                                                        \
        osd.add(opt);                                                                                      \
    }

#define QUTILITY_REGISTER_OPTION_IMPL_REGISTER_FUNCTION_REQUIRED(type, name, cml_token, desc) \
    void name##_register_func(osd_T &osd) const                                               \
    {                                                                                         \
        boost::shared_ptr<od_T> opt(new od_T(                                                 \
            cml_token,                                                                        \
            boost::program_options::value<type>()->required(),                                \
            desc));                                                                           \
        osd.add(opt);                                                                         \
    }

#define QUTILITY_REGISTER_OPTION_IMPL_STUB(class_name, name) \
    const bool name##_stub = make_stub(                      \
        &class_name::name##_get_func,                        \
        &class_name::name##_register_func,                   \
        &class_name::name##_str_func,                        \
        &class_name::name##_json_insert_func);

#define QUTILITY_REGISTER_OPTION_WITH_DEFAULT(class_name, type, name, tag, cml_token, default, desc)   \
    QUTILITY_REGISTER_OPTION_IMPL_VALUE_AND_COMMON_FUNCTIONS(type, name, tag)                          \
    QUTILITY_REGISTER_OPTION_IMPL_REGISTER_FUNCTION_WITH_DEFAULT(type, name, cml_token, default, desc) \
    QUTILITY_REGISTER_OPTION_IMPL_STUB(class_name, name)

#define QUTILITY_REGISTER_OPTION_REQUIRED(class_name, type, name, tag, cml_token, desc)   \
    QUTILITY_REGISTER_OPTION_IMPL_VALUE_AND_COMMON_FUNCTIONS(type, name, tag)             \
    QUTILITY_REGISTER_OPTION_IMPL_REGISTER_FUNCTION_REQUIRED(type, name, cml_token, desc) \
    QUTILITY_REGISTER_OPTION_IMPL_STUB(class_name, name)

#define QUTILITY_REGISTER_OPTION_INITIALIZE(class_name)                        \
private:                                                                       \
    void sample_get_func(const vm_T &);                                        \
    void sample_register_func(osd_T &) const;                                  \
    std::pair<std::string, std::string> sample_str_func() const;               \
    void sample_json_insert_func(jsobject_T &) const;                          \
    using get_func_T = decltype(&class_name::sample_get_func);                 \
    using register_func_T = decltype(&class_name::sample_register_func);       \
    using str_func_T = decltype(&class_name::sample_str_func);                 \
    using json_insert_func_T = decltype(&class_name::sample_json_insert_func); \
    std::vector<get_func_T> get_funcs_;                                        \
    std::vector<register_func_T> register_funcs_;                              \
    std::vector<str_func_T> str_funcs_;                                        \
    std::vector<json_insert_func_T> json_insert_funcs_;                        \
    bool make_stub(                                                            \
        get_func_T get_func,                                                   \
        register_func_T register_func,                                         \
        str_func_T str_func,                                                   \
        json_insert_func_T json_func)                                          \
    {                                                                          \
        get_funcs_.push_back(get_func);                                        \
        register_funcs_.push_back(register_func);                              \
        str_funcs_.push_back(str_func);                                        \
        json_insert_funcs_.push_back(json_func);                               \
        return true;                                                           \
    }                                                                          \
                                                                               \
public:                                                                        \
    void get_all(vm_T &vm)                                                     \
    {                                                                          \
        for (const auto &ele : get_funcs_)                                     \
        {                                                                      \
            std::invoke(ele, *this, vm);                                       \
        }                                                                      \
    }                                                                          \
    void register_all(osd_T &od)                                               \
    {                                                                          \
        for (const auto &ele : register_funcs_)                                \
        {                                                                      \
            std::invoke(ele, *this, od);                                       \
        }                                                                      \
    }                                                                          \
    void print_all(std::ostream &os)                                           \
    {                                                                          \
        for (const auto &ele : str_funcs_)                                     \
        {                                                                      \
            auto ans = std::invoke(ele, *this);                                \
            os << ans.first << " = " << ans.second << ", ";                    \
        }                                                                      \
        os << std::endl;                                                       \
    }                                                                          \
    void json_insert_all(jsobject_T &obj)                                      \
    {                                                                          \
        for (const auto &ele : json_insert_funcs_)                             \
        {                                                                      \
            std::invoke(ele, *this, obj);                                      \
        }                                                                      \
    }

#endif

#endif