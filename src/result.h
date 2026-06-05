#pragma once

#include <cstdint>
#include <functional>
#include <initializer_list>
#include <stdexcept>
#include <type_traits>
#include <utility>

template <typename T>
using ResultRemoveCvref = std::remove_cv_t<std::remove_reference_t<T>>;

enum class ResultErrorCode : uint8_t {
  ResultInSuccessState = 0,
  ResultInFailureState,
};

constexpr const char* to_string(ResultErrorCode code) noexcept {
  switch (code) {
    case ResultErrorCode::ResultInSuccessState:
      return "Result in success state, but requested error value";
    case ResultErrorCode::ResultInFailureState:
      return "Result in failure state, but requested success value";
    default:
      return "Unknown error";
  }
}

class ResultException : public std::exception {
  public:
    explicit ResultException(ResultErrorCode code)
      : code_(code) 
    {}

    ResultErrorCode code() const noexcept 
    {
      return code_;
    }

    const char* what() const noexcept override {
      return to_string(code_);
    }

  private:
    ResultErrorCode code_;
};
template <typename T, typename E> class Result {
private:
  struct SuccessTag {};
  struct FailureTag {};

public:
  static Result Success(T value) {
    return Result(SuccessTag{}, std::move(value));
  }

  static Result Failure(E error) {
    return Result(FailureTag{}, std::move(error));
  }

  template <typename... Args>
  static Result SuccessInPlace(Args &&...args) {
    return Result(SuccessTag{}, std::forward<Args>(args)...);
  }

  template <typename U, typename... Args>
  static Result SuccessInPlace(std::initializer_list<U> values,
                               Args &&...args) {
    return Result(SuccessTag{}, values, std::forward<Args>(args)...);
  }

  template <typename... Args>
  static Result FailureInPlace(Args &&...args) {
    return Result(FailureTag{}, std::forward<Args>(args)...);
  }

  template <typename U, typename... Args>
  static Result FailureInPlace(std::initializer_list<U> values,
                               Args &&...args) {
    return Result(FailureTag{}, values, std::forward<Args>(args)...);
  }

  bool isSuccess() const noexcept { return _isSuccess; }
  bool isFailure() const noexcept { return !_isSuccess; }
  explicit operator bool() const noexcept { return _isSuccess; }

  template <typename F>
  auto andThen(F &&func) & -> std::invoke_result_t<F, T &> {
    using ReturnType = std::invoke_result_t<F, T &>;
    if (_isSuccess) {
      return std::invoke(std::forward<F>(func), _storage._value);
    }
    return ReturnType::Failure(_storage._error);
  }

  template <typename F>
  auto andThen(F &&func) const & -> std::invoke_result_t<F, const T &> {
    using ReturnType = std::invoke_result_t<F, const T &>;
    if (_isSuccess) {
      return std::invoke(std::forward<F>(func), _storage._value);
    }
    return ReturnType::Failure(_storage._error);
  }

  template <typename F>
  auto andThen(F &&func) && -> std::invoke_result_t<F, T &&> {
    using ReturnType = std::invoke_result_t<F, T &&>;
    if (_isSuccess) {
      return std::invoke(std::forward<F>(func), std::move(_storage._value));
    }
    return ReturnType::Failure(std::move(_storage._error));
  }

  template <typename F>
  auto map(F &&func) &
      -> Result<ResultRemoveCvref<std::invoke_result_t<F, T &>>, E> {
    using ValueType = ResultRemoveCvref<std::invoke_result_t<F, T &>>;
    using ReturnType = Result<ValueType, E>;
    if (_isSuccess) {
      return ReturnType::Success(std::invoke(std::forward<F>(func), _storage._value));
    }
    return ReturnType::Failure(_storage._error);
  }

  template <typename F>
  auto map(F &&func) const &
      -> Result<ResultRemoveCvref<std::invoke_result_t<F, const T &>>, E> {
    using ValueType = ResultRemoveCvref<std::invoke_result_t<F, const T &>>;
    using ReturnType = Result<ValueType, E>;
    if (_isSuccess) {
      return ReturnType::Success(std::invoke(std::forward<F>(func), _storage._value));
    }
    return ReturnType::Failure(_storage._error);
  }

  template <typename F>
  auto map(F &&func) &&
      -> Result<ResultRemoveCvref<std::invoke_result_t<F, T &&>>, E> {
    using ValueType = ResultRemoveCvref<std::invoke_result_t<F, T &&>>;
    using ReturnType = Result<ValueType, E>;
    if (_isSuccess) {
      return ReturnType::Success(
          std::invoke(std::forward<F>(func), std::move(_storage._value)));
    }
    return ReturnType::Failure(std::move(_storage._error));
  }

  template <typename F>
  auto map_error(F &&func) &
      -> Result<T, ResultRemoveCvref<std::invoke_result_t<F, E &>>> {
    using ErrorType = ResultRemoveCvref<std::invoke_result_t<F, E &>>;
    using ReturnType = Result<T, ErrorType>;
    if (_isSuccess) {
      return ReturnType::Success(_storage._value);
    }
    return ReturnType::Failure(std::invoke(std::forward<F>(func), _storage._error));
  }

  template <typename F>
  auto map_error(F &&func) const &
      -> Result<T, ResultRemoveCvref<std::invoke_result_t<F, const E &>>> {
    using ErrorType = ResultRemoveCvref<std::invoke_result_t<F, const E &>>;
    using ReturnType = Result<T, ErrorType>;
    if (_isSuccess) {
      return ReturnType::Success(_storage._value);
    }
    return ReturnType::Failure(std::invoke(std::forward<F>(func), _storage._error));
  }

  template <typename F>
  auto map_error(F &&func) &&
      -> Result<T, ResultRemoveCvref<std::invoke_result_t<F, E &&>>> {
    using ErrorType = ResultRemoveCvref<std::invoke_result_t<F, E &&>>;
    using ReturnType = Result<T, ErrorType>;
    if (_isSuccess) {
      return ReturnType::Success(std::move(_storage._value));
    }
    return ReturnType::Failure(
        std::invoke(std::forward<F>(func), std::move(_storage._error)));
  }

  template <typename F>
  auto or_else(F &&func) & -> std::invoke_result_t<F, E &> {
    using ReturnType = std::invoke_result_t<F, E &>;
    if (_isSuccess) {
      return ReturnType::Success(_storage._value);
    }
    return std::invoke(std::forward<F>(func), _storage._error);
  }

  template <typename F>
  auto or_else(F &&func) const & -> std::invoke_result_t<F, const E &> {
    using ReturnType = std::invoke_result_t<F, const E &>;
    if (_isSuccess) {
      return ReturnType::Success(_storage._value);
    }
    return std::invoke(std::forward<F>(func), _storage._error);
  }

  template <typename F>
  auto or_else(F &&func) && -> std::invoke_result_t<F, E &&> {
    using ReturnType = std::invoke_result_t<F, E &&>;
    if (_isSuccess) {
      return ReturnType::Success(std::move(_storage._value));
    }
    return std::invoke(std::forward<F>(func), std::move(_storage._error));
  }

  // Safe accessors with state validation (ASIL-B: defensive programming)
  const T &Value() const {
    if (!_isSuccess) {
      throw ResultException(ResultErrorCode::ResultInFailureState);
    }
    return _storage._value;
  }

  const E &Error() const {
    if (_isSuccess) {
      throw ResultException(ResultErrorCode::ResultInSuccessState);
    }
    return _storage._error;
  }

  // Explicitly delete copy semantics (ASIL-B: explicit lifecycle control)
  Result(const Result &) = delete;
  Result &operator=(const Result &) = delete;

  // Allow move semantics (ASIL-B: predictable resource management)
  Result(Result &&other) noexcept : _isSuccess(other._isSuccess) {
    if (_isSuccess) {
      new (&_storage._value) T(std::move(other._storage._value));
    } else {
      new (&_storage._error) E(std::move(other._storage._error));
    }
  }

  Result &operator=(Result &&other) noexcept {
    if (this != &other) {
      // Destroy current state
      if (_isSuccess) {
        _storage._value.~T();
      } else {
        _storage._error.~E();
      }
      // Move new state
      _isSuccess = other._isSuccess;
      if (_isSuccess) {
        new (&_storage._value) T(std::move(other._storage._value));
      } else {
        new (&_storage._error) E(std::move(other._storage._error));
      }
    }
    return *this;
  }

  ~Result() {
    if (_isSuccess) {
      _storage._value.~T();
    } else {
      _storage._error.~E();
    }
  }

private:
  template <typename... Args>
  Result(SuccessTag, Args &&...args)
      : _isSuccess(true),
        _storage(SuccessTag{}, std::forward<Args>(args)...) {}

  template <typename U, typename... Args>
  Result(SuccessTag, std::initializer_list<U> values, Args &&...args)
      : _isSuccess(true),
        _storage(SuccessTag{}, values, std::forward<Args>(args)...) {}

  template <typename... Args>
  Result(FailureTag, Args &&...args)
      : _isSuccess(false),
        _storage(FailureTag{}, std::forward<Args>(args)...) {}

  template <typename U, typename... Args>
  Result(FailureTag, std::initializer_list<U> values, Args &&...args)
      : _isSuccess(false),
        _storage(FailureTag{}, values, std::forward<Args>(args)...) {}

  bool _isSuccess;
  union Storage {
    T _value;
    E _error;

    Storage() {}

    template <typename... Args>
    Storage(SuccessTag, Args &&...args)
        : _value(std::forward<Args>(args)...) {}

    template <typename U, typename... Args>
    Storage(SuccessTag, std::initializer_list<U> values, Args &&...args)
        : _value(values, std::forward<Args>(args)...) {}

    template <typename... Args>
    Storage(FailureTag, Args &&...args)
        : _error(std::forward<Args>(args)...) {}

    template <typename U, typename... Args>
    Storage(FailureTag, std::initializer_list<U> values, Args &&...args)
        : _error(values, std::forward<Args>(args)...) {}

    ~Storage() {}
  } _storage;
};
