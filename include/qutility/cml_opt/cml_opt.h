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
            po::options_description desc("Allowed options");
            try
            {
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
                std::cout << desc << "\n";
                exit(-1);
            }
        }

        template <typename OptionsDerivedT, typename = typename std::enable_if_t<std::is_base_of_v<OptionsBase, OptionsDerivedT>>>
        void obtain_explicit_cml_options(int argc, const char *const *argv, OptionsDerivedT &opts)
        {
            namespace po = boost::program_options;
            po::options_description desc("Allowed options");
            try
            {
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
                opts.get_explicit_only(vm);
            }
            catch (po::error &e)
            {
                std::cout << e.what() << std::endl;
                std::cout << desc << "\n";
                exit(-1);
            }
        }

        template <typename ResultT>
        ResultT get_json_value(const boost::json::object &obj, const char *obj_tag)
        {
            if constexpr (std::is_signed<ResultT>::value && std::is_integral<ResultT>::value)
            {
                return obj.at(obj_tag).get_int64();
            }
            else if constexpr (std::is_unsigned<ResultT>::value && std::is_integral<ResultT>::value)
            {
                return obj.at(obj_tag).get_uint64();
            }
            else if constexpr (std::is_floating_point<ResultT>::value)
            {
                return obj.at(obj_tag).get_double();
            }
            else if constexpr (std::is_constructible<boost::json::string, ResultT>::value || std::is_convertible<boost::json::string, ResultT>::value)
            {
                return ResultT(obj.at(obj_tag).get_string());
            }
            else if constexpr (std::is_same<bool, ResultT>::value)
            {
                return obj.at(obj_tag).get_bool();
            }
            else
            {
                static_assert(std::is_convertible<boost::json::object, ResultT>::value, "No conversion method available. Note that object or array are not allowed");
                return obj.at(obj_tag);
            }
        }
    }
}

#ifndef QUTILITY_REGISTER_OPTION

#define QUTILITY_REGISTER_OPTION_IMPL_VALUE_AND_COMMON_FUNCTIONS(type, name, tag, cml_token) \
public:                                                                                      \
    type name;                                                                               \
                                                                                             \
protected:                                                                                   \
    constexpr static const char *name##_tag = cml_token;                                     \
    void name##_get_func(const vm_T &vm, bool if_override)                                   \
    {                                                                                        \
        auto val = vm[name##_tag].as<type>();                                                \
        if (if_override || (!vm[name##_tag].defaulted()))                                    \
        {                                                                                    \
            name = val;                                                                      \
        }                                                                                    \
    }                                                                                        \
    std::pair<std::string, std::string> name##_str_func() const                              \
    {                                                                                        \
        return {name##_tag, to_string(name)};                                                \
    }                                                                                        \
    void name##_json_insert_func(jsobject_T &obj) const                                      \
    {                                                                                        \
        obj[name##_tag] = name;                                                              \
    }                                                                                        \
    void name##_json_obtain_func(const jsobject_T &obj)                                      \
    {                                                                                        \
        if (obj.find(name##_tag) != obj.end())                                               \
        {                                                                                    \
            name = qutility::cml_opt::get_json_value<type>(obj, name##_tag);                 \
        }                                                                                    \
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
        &class_name::name##_json_insert_func,                \
        &class_name::name##_json_obtain_func);

#define QUTILITY_REGISTER_OPTION_WITH_DEFAULT(class_name, type, name, tag, cml_token, default, desc)   \
    QUTILITY_REGISTER_OPTION_IMPL_VALUE_AND_COMMON_FUNCTIONS(type, name, tag, cml_token)               \
    QUTILITY_REGISTER_OPTION_IMPL_REGISTER_FUNCTION_WITH_DEFAULT(type, name, cml_token, default, desc) \
    QUTILITY_REGISTER_OPTION_IMPL_STUB(class_name, name)

#define QUTILITY_REGISTER_OPTION_REQUIRED(class_name, type, name, tag, cml_token, desc)   \
    QUTILITY_REGISTER_OPTION_IMPL_VALUE_AND_COMMON_FUNCTIONS(type, name, tag, cml_token)  \
    QUTILITY_REGISTER_OPTION_IMPL_REGISTER_FUNCTION_REQUIRED(type, name, cml_token, desc) \
    QUTILITY_REGISTER_OPTION_IMPL_STUB(class_name, name)

#define QUTILITY_REGISTER_OPTION_INITIALIZE(class_name)                        \
private:                                                                       \
    void sample_get_func(const vm_T &, bool);                                  \
    void sample_register_func(osd_T &) const;                                  \
    std::pair<std::string, std::string> sample_str_func() const;               \
    void sample_json_insert_func(jsobject_T &) const;                          \
    void sample_json_obtain_func(const jsobject_T &);                          \
    using get_func_T = decltype(&class_name::sample_get_func);                 \
    using register_func_T = decltype(&class_name::sample_register_func);       \
    using str_func_T = decltype(&class_name::sample_str_func);                 \
    using json_insert_func_T = decltype(&class_name::sample_json_insert_func); \
    using json_obtain_func_T = decltype(&class_name::sample_json_obtain_func); \
    std::vector<get_func_T> get_funcs_;                                        \
    std::vector<register_func_T> register_funcs_;                              \
    std::vector<str_func_T> str_funcs_;                                        \
    std::vector<json_insert_func_T> json_insert_funcs_;                        \
    std::vector<json_obtain_func_T> json_obtain_funcs_;                        \
    bool make_stub(                                                            \
        get_func_T get_func,                                                   \
        register_func_T register_func,                                         \
        str_func_T str_func,                                                   \
        json_insert_func_T json_insert_func,                                   \
        json_obtain_func_T json_obtain_func)                                   \
    {                                                                          \
        get_funcs_.push_back(get_func);                                        \
        register_funcs_.push_back(register_func);                              \
        str_funcs_.push_back(str_func);                                        \
        json_insert_funcs_.push_back(json_insert_func);                        \
        json_obtain_funcs_.push_back(json_obtain_func);                        \
        return true;                                                           \
    }                                                                          \
                                                                               \
public:                                                                        \
    void get_all(vm_T &vm)                                                     \
    {                                                                          \
        for (const auto &ele : get_funcs_)                                     \
        {                                                                      \
            std::invoke(ele, *this, vm, true);                                 \
        }                                                                      \
    }                                                                          \
    void get_explicit_only(vm_T &vm)                                           \
    {                                                                          \
        for (const auto &ele : get_funcs_)                                     \
        {                                                                      \
            std::invoke(ele, *this, vm, false);                                \
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
    void print_all_cml_style(std::ostream &os)                                 \
    {                                                                          \
        for (const auto &ele : str_funcs_)                                     \
        {                                                                      \
            auto ans = std::invoke(ele, *this);                                \
            os << "--" << ans.first << "=" << ans.second << " ";               \
        }                                                                      \
        os << std::endl;                                                       \
    }                                                                          \
    void json_insert_all(jsobject_T &obj)                                      \
    {                                                                          \
        for (const auto &ele : json_insert_funcs_)                             \
        {                                                                      \
            std::invoke(ele, *this, obj);                                      \
        }                                                                      \
    }                                                                          \
    void json_obtain_all(const jsobject_T &obj)                                \
    {                                                                          \
        for (const auto &ele : json_obtain_funcs_)                             \
        {                                                                      \
            std::invoke(ele, *this, obj);                                      \
        }                                                                      \
    }

#endif

#endif