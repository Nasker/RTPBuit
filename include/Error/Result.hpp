#pragma once

#include "Error.hpp"
#include <optional>

/**
 * @brief Result type for error handling without exceptions
 * 
 * This template provides a type-safe way to handle operations that can fail
 * without using exceptions, which is ideal for embedded systems.
 * 
 * @tparam T The type of value to return on success
 */
template<typename T>
class Result {
private:
    std::optional<T> _value;
    Error _error;

public:
    // Constructors
    Result(const T& value) : _value(value), _error(ErrorCode::None, ErrorSeverity::Info, "Success") {}
    Result(T&& value) : _value(std::move(value)), _error(ErrorCode::None, ErrorSeverity::Info, "Success") {}
    Result(const Error& error) : _value(std::nullopt), _error(error) {}

    // Static factory methods
    static Result Ok(const T& value) { return Result(value); }
    static Result Ok(T&& value) { return Result(std::move(value)); }
    static Result ErrorResult(const Error& error) { return Result(error); }
    static Result failure(ErrorCode code, ErrorSeverity severity, const String& message) {
        return Result(::Error(code, severity, message));
    }

    // Check result
    bool isOk() const { return _value.has_value() && _error.code == ErrorCode::None; }
    bool isError() const { return !isOk(); }

    // Access value (unsafe - check isOk() first)
    const T& getValue() const { 
        // In production, this should be handled more gracefully
        return _value.value(); 
    }
    
    T& getValue() { 
        return _value.value(); 
    }

    // Access value or default
    T getValueOr(const T& defaultValue) const {
        return isOk() ? _value.value() : defaultValue;
    }

    // Access error
    const ::Error& getError() const { return _error; }

    // Monadic operations
    template<typename F>
    auto map(F&& func) -> Result<decltype(func(_value.value()))> {
        using ReturnType = decltype(func(_value.value()));
        
        if (isOk()) {
            return Result<ReturnType>::Ok(func(_value.value()));
        } else {
            return Result<ReturnType>::Error(_error);
        }
    }

    template<typename F>
    auto flatMap(F&& func) -> decltype(func(_value.value())) {
        if (isOk()) {
            return func(_value.value());
        } else {
            return decltype(func(_value.value()))::Error(_error);
        }
    }

    // Chaining with error handling
    template<typename F>
    Result<T> onError(F&& func) {
        if (isError()) {
            func(_error);
        }
        return *this;
    }

    // Conversion operators
    operator bool() const { return isOk(); }

    // Dereference operators
    const T& operator*() const { return getValue(); }
    T& operator*() { return getValue(); }
    const T* operator->() const { return &_value.value(); }
    T* operator->() { return &_value.value(); }
};

// Specialization for void return type
template<>
class Result<void> {
private:
    Error _error;

public:
    Result() : _error(ErrorCode::None, ErrorSeverity::Info, "Success") {}
    Result(const Error& error) : _error(error) {}

    static Result Ok() { return Result(); }
    static Result ErrorResult(const Error& error) { return Result(error); }
    static Result failure(ErrorCode code, ErrorSeverity severity, const String& message) {
        return Result(::Error(code, severity, message));
    }

    bool isOk() const { return _error.code == ErrorCode::None; }
    bool isError() const { return !isOk(); }

    const ::Error& getError() const { return _error; }

    operator bool() const { return isOk(); }
};

// Convenience macros for error handling
#define RETURN_IF_ERROR(result) \
    do { \
        auto _temp_result = (result); \
        if (_temp_result.isError()) { \
            return _temp_result.getError(); \
        } \
    } while(0)

#define ASSIGN_OR_RETURN(var, result) \
    auto _temp_result_##var = (result); \
    if (_temp_result_##var.isError()) { \
        return _temp_result_##var.getError(); \
    } \
    auto var = _temp_result_##var.getValue()

#define TRY(result) \
    do { \
        auto _temp_result = (result); \
        if (_temp_result.isError()) { \
            return _temp_result.getError(); \
        } \
    } while(0)
