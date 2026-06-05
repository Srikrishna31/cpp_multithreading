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

/**
 * @brief Converts a ResultErrorCode to a diagnostic message.
 *
 * @param code Error code to describe.
 * @return Null-terminated static string for the code.
 */
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
    /**
     * @brief Constructs an exception for an invalid Result state access.
     *
     * @param code Error code describing the invalid access.
     */
    explicit ResultException(ResultErrorCode code)
      : code_(code) 
    {}

    /**
     * @brief Returns the Result-specific error code.
     *
     * @return Error code passed at construction.
     */
    ResultErrorCode code() const noexcept 
    {
      return code_;
    }

    /**
     * @brief Returns a human-readable description of the error.
     *
     * @return Null-terminated static diagnostic string.
     */
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
  /**
   * @brief Constructs a successful Result from an already-created value.
   *
   * @param value Success value to move into the Result.
   * @return Result in the success state.
   */
  static Result Success(T value) {
    return Result(SuccessTag{}, std::move(value));
  }

  /**
   * @brief Constructs a failed Result from an already-created error.
   *
   * @param error Error value to move into the Result.
   * @return Result in the failure state.
   */
  static Result Failure(E error) {
    return Result(FailureTag{}, std::move(error));
  }

  /**
   * @brief Constructs the success value directly in place.
   *
   * @tparam Args Constructor argument types for T.
   * @param args Arguments forwarded to T's constructor.
   * @return Result in the success state.
   */
  template <typename... Args>
  static Result SuccessInPlace(Args &&...args) {
    return Result(SuccessTag{}, std::forward<Args>(args)...);
  }

  /**
   * @brief Constructs the success value directly in place from an initializer list.
   *
   * @tparam U Initializer-list element type.
   * @tparam Args Additional constructor argument types for T.
   * @param values Initializer list forwarded to T's constructor.
   * @param args Additional arguments forwarded to T's constructor.
   * @return Result in the success state.
   */
  template <typename U, typename... Args>
  static Result SuccessInPlace(std::initializer_list<U> values,
                               Args &&...args) {
    return Result(SuccessTag{}, values, std::forward<Args>(args)...);
  }

  /**
   * @brief Constructs the error value directly in place.
   *
   * @tparam Args Constructor argument types for E.
   * @param args Arguments forwarded to E's constructor.
   * @return Result in the failure state.
   */
  template <typename... Args>
  static Result FailureInPlace(Args &&...args) {
    return Result(FailureTag{}, std::forward<Args>(args)...);
  }

  /**
   * @brief Constructs the error value directly in place from an initializer list.
   *
   * @tparam U Initializer-list element type.
   * @tparam Args Additional constructor argument types for E.
   * @param values Initializer list forwarded to E's constructor.
   * @param args Additional arguments forwarded to E's constructor.
   * @return Result in the failure state.
   */
  template <typename U, typename... Args>
  static Result FailureInPlace(std::initializer_list<U> values,
                               Args &&...args) {
    return Result(FailureTag{}, values, std::forward<Args>(args)...);
  }

  /**
   * @brief Checks whether the Result contains a success value.
   *
   * @return true when the Result is in the success state.
   */
  bool isSuccess() const noexcept { return _isSuccess; }

  /**
   * @brief Checks whether the Result contains an error value.
   *
   * @return true when the Result is in the failure state.
   */
  bool isFailure() const noexcept { return !_isSuccess; }

  /**
   * @brief Checks whether the Result contains a success value.
   *
   * @return true when the Result is in the success state.
   */
  explicit operator bool() const noexcept { return _isSuccess; }

  /**
   * @brief Chains a success value into another Result-producing operation.
   *
   * Invokes func with T& when this Result is successful. If this Result is a
   * failure, propagates the current error into the returned Result type.
   *
   * @tparam F Callable type accepting T& and returning a Result-like type.
   * @param func Callable invoked only in the success state.
   * @return Result returned by func, or a failure carrying the current error.
   */
  template <typename F>
  auto andThen(F &&func) & -> std::invoke_result_t<F, T &> {
    using ReturnType = std::invoke_result_t<F, T &>;
    if (_isSuccess) {
      return std::invoke(std::forward<F>(func), _storage._value);
    }
    return ReturnType::Failure(_storage._error);
  }

  /**
   * @brief Chains a const success value into another Result-producing operation.
   *
   * Invokes func with const T& when this Result is successful. If this Result is
   * a failure, propagates the current error into the returned Result type.
   *
   * @tparam F Callable type accepting const T& and returning a Result-like type.
   * @param func Callable invoked only in the success state.
   * @return Result returned by func, or a failure carrying the current error.
   */
  template <typename F>
  auto andThen(F &&func) const & -> std::invoke_result_t<F, const T &> {
    using ReturnType = std::invoke_result_t<F, const T &>;
    if (_isSuccess) {
      return std::invoke(std::forward<F>(func), _storage._value);
    }
    return ReturnType::Failure(_storage._error);
  }

  /**
   * @brief Moves a success value into another Result-producing operation.
   *
   * Invokes func with T&& when this Result is successful. If this Result is a
   * failure, moves the current error into the returned Result type.
   *
   * @tparam F Callable type accepting T&& and returning a Result-like type.
   * @param func Callable invoked only in the success state.
   * @return Result returned by func, or a failure carrying the moved error.
   */
  template <typename F>
  auto andThen(F &&func) && -> std::invoke_result_t<F, T &&> {
    using ReturnType = std::invoke_result_t<F, T &&>;
    if (_isSuccess) {
      return std::invoke(std::forward<F>(func), std::move(_storage._value));
    }
    return ReturnType::Failure(std::move(_storage._error));
  }

  /**
   * @brief Transforms a success value while preserving the error type.
   *
   * Invokes func with T& when this Result is successful. If this Result is a
   * failure, propagates the current error unchanged.
   *
   * @tparam F Callable type accepting T&.
   * @param func Callable invoked only in the success state.
   * @return Result containing func's return value, or the current error.
   */
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

  /**
   * @brief Transforms a const success value while preserving the error type.
   *
   * Invokes func with const T& when this Result is successful. If this Result is
   * a failure, propagates the current error unchanged.
   *
   * @tparam F Callable type accepting const T&.
   * @param func Callable invoked only in the success state.
   * @return Result containing func's return value, or the current error.
   */
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

  /**
   * @brief Moves and transforms a success value while preserving the error type.
   *
   * Invokes func with T&& when this Result is successful. If this Result is a
   * failure, moves the current error into the returned Result.
   *
   * @tparam F Callable type accepting T&&.
   * @param func Callable invoked only in the success state.
   * @return Result containing func's return value, or the moved error.
   */
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

  /**
   * @brief Transforms an error value while preserving the success type.
   *
   * Invokes func with E& when this Result is a failure. If this Result is
   * successful, propagates the current value unchanged.
   *
   * @tparam F Callable type accepting E&.
   * @param func Callable invoked only in the failure state.
   * @return Result containing the current value, or func's returned error.
   */
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

  /**
   * @brief Transforms a const error value while preserving the success type.
   *
   * Invokes func with const E& when this Result is a failure. If this Result is
   * successful, propagates the current value unchanged.
   *
   * @tparam F Callable type accepting const E&.
   * @param func Callable invoked only in the failure state.
   * @return Result containing the current value, or func's returned error.
   */
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

  /**
   * @brief Moves and transforms an error value while preserving the success type.
   *
   * Invokes func with E&& when this Result is a failure. If this Result is
   * successful, moves the current value into the returned Result.
   *
   * @tparam F Callable type accepting E&&.
   * @param func Callable invoked only in the failure state.
   * @return Result containing the moved value, or func's returned error.
   */
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

  /**
   * @brief Recovers from an error using another Result-producing operation.
   *
   * If this Result is successful, propagates the current value into the returned
   * Result type. Otherwise invokes func with E&.
   *
   * @tparam F Callable type accepting E& and returning a Result-like type.
   * @param func Callable invoked only in the failure state.
   * @return Successful propagation of the current value, or func's Result.
   */
  template <typename F>
  auto or_else(F &&func) & -> std::invoke_result_t<F, E &> {
    using ReturnType = std::invoke_result_t<F, E &>;
    if (_isSuccess) {
      return ReturnType::Success(_storage._value);
    }
    return std::invoke(std::forward<F>(func), _storage._error);
  }

  /**
   * @brief Recovers from a const error using another Result-producing operation.
   *
   * If this Result is successful, propagates the current value into the returned
   * Result type. Otherwise invokes func with const E&.
   *
   * @tparam F Callable type accepting const E& and returning a Result-like type.
   * @param func Callable invoked only in the failure state.
   * @return Successful propagation of the current value, or func's Result.
   */
  template <typename F>
  auto or_else(F &&func) const & -> std::invoke_result_t<F, const E &> {
    using ReturnType = std::invoke_result_t<F, const E &>;
    if (_isSuccess) {
      return ReturnType::Success(_storage._value);
    }
    return std::invoke(std::forward<F>(func), _storage._error);
  }

  /**
   * @brief Moves an error into another Result-producing recovery operation.
   *
   * If this Result is successful, moves the current value into the returned
   * Result type. Otherwise invokes func with E&&.
   *
   * @tparam F Callable type accepting E&& and returning a Result-like type.
   * @param func Callable invoked only in the failure state.
   * @return Successful propagation of the moved value, or func's Result.
   */
  template <typename F>
  auto or_else(F &&func) && -> std::invoke_result_t<F, E &&> {
    using ReturnType = std::invoke_result_t<F, E &&>;
    if (_isSuccess) {
      return ReturnType::Success(std::move(_storage._value));
    }
    return std::invoke(std::forward<F>(func), std::move(_storage._error));
  }

  /**
   * @brief Returns the contained success value.
   *
   * @return Const reference to the success value.
   * @throws ResultException when the Result is in the failure state.
   */
  const T &Value() const {
    if (!_isSuccess) {
      throw ResultException(ResultErrorCode::ResultInFailureState);
    }
    return _storage._value;
  }

  /**
   * @brief Returns the contained error value.
   *
   * @return Const reference to the error value.
   * @throws ResultException when the Result is in the success state.
   */
  const E &Error() const {
    if (_isSuccess) {
      throw ResultException(ResultErrorCode::ResultInSuccessState);
    }
    return _storage._error;
  }

  /**
   * @brief Copy construction is disabled.
   *
   * Result owns exactly one active union member and supports explicit move-only
   * lifecycle management.
   */
  Result(const Result &) = delete;

  /**
   * @brief Copy assignment is disabled.
   *
   * Result owns exactly one active union member and supports explicit move-only
   * lifecycle management.
   */
  Result &operator=(const Result &) = delete;

  /**
   * @brief Move-constructs a Result from another Result.
   *
   * Constructs the same active state as other by moving its contained value or
   * error.
   *
   * @param other Result to move from.
   */
  Result(Result &&other) noexcept : _isSuccess(other._isSuccess) {
    if (_isSuccess) {
      new (&_storage._value) T(std::move(other._storage._value));
    } else {
      new (&_storage._error) E(std::move(other._storage._error));
    }
  }

  /**
   * @brief Move-assigns a Result from another Result.
   *
   * Destroys the current active state, then constructs the same active state as
   * other by moving its contained value or error.
   *
   * @param other Result to move from.
   * @return Reference to this Result.
   */
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

  /**
   * @brief Destroys the active success or error object.
   */
  ~Result() {
    if (_isSuccess) {
      _storage._value.~T();
    } else {
      _storage._error.~E();
    }
  }

private:
  /**
   * @brief Constructs a Result whose active member is the success value.
   *
   * @tparam Args Constructor argument types for T.
   * @param args Arguments forwarded to the active storage member.
   */
  template <typename... Args>
  Result(SuccessTag, Args &&...args)
      : _isSuccess(true),
        _storage(SuccessTag{}, std::forward<Args>(args)...) {}

  /**
   * @brief Constructs a Result whose active success value uses an initializer list.
   *
   * @tparam U Initializer-list element type.
   * @tparam Args Additional constructor argument types for T.
   * @param values Initializer list forwarded to the active storage member.
   * @param args Additional arguments forwarded to the active storage member.
   */
  template <typename U, typename... Args>
  Result(SuccessTag, std::initializer_list<U> values, Args &&...args)
      : _isSuccess(true),
        _storage(SuccessTag{}, values, std::forward<Args>(args)...) {}

  /**
   * @brief Constructs a Result whose active member is the error value.
   *
   * @tparam Args Constructor argument types for E.
   * @param args Arguments forwarded to the active storage member.
   */
  template <typename... Args>
  Result(FailureTag, Args &&...args)
      : _isSuccess(false),
        _storage(FailureTag{}, std::forward<Args>(args)...) {}

  /**
   * @brief Constructs a Result whose active error value uses an initializer list.
   *
   * @tparam U Initializer-list element type.
   * @tparam Args Additional constructor argument types for E.
   * @param values Initializer list forwarded to the active storage member.
   * @param args Additional arguments forwarded to the active storage member.
   */
  template <typename U, typename... Args>
  Result(FailureTag, std::initializer_list<U> values, Args &&...args)
      : _isSuccess(false),
        _storage(FailureTag{}, values, std::forward<Args>(args)...) {}

  bool _isSuccess;
  union Storage {
    T _value;
    E _error;

    /**
     * @brief Leaves storage uninitialized for placement construction.
     */
    Storage() {}

    /**
     * @brief Constructs the success storage member in place.
     *
     * @tparam Args Constructor argument types for T.
     * @param args Arguments forwarded to T's constructor.
     */
    template <typename... Args>
    Storage(SuccessTag, Args &&...args)
        : _value(std::forward<Args>(args)...) {}

    /**
     * @brief Constructs the success storage member from an initializer list.
     *
     * @tparam U Initializer-list element type.
     * @tparam Args Additional constructor argument types for T.
     * @param values Initializer list forwarded to T's constructor.
     * @param args Additional arguments forwarded to T's constructor.
     */
    template <typename U, typename... Args>
    Storage(SuccessTag, std::initializer_list<U> values, Args &&...args)
        : _value(values, std::forward<Args>(args)...) {}

    /**
     * @brief Constructs the error storage member in place.
     *
     * @tparam Args Constructor argument types for E.
     * @param args Arguments forwarded to E's constructor.
     */
    template <typename... Args>
    Storage(FailureTag, Args &&...args)
        : _error(std::forward<Args>(args)...) {}

    /**
     * @brief Constructs the error storage member from an initializer list.
     *
     * @tparam U Initializer-list element type.
     * @tparam Args Additional constructor argument types for E.
     * @param values Initializer list forwarded to E's constructor.
     * @param args Additional arguments forwarded to E's constructor.
     */
    template <typename U, typename... Args>
    Storage(FailureTag, std::initializer_list<U> values, Args &&...args)
        : _error(values, std::forward<Args>(args)...) {}

    /**
     * @brief Does not destroy storage members; Result destroys the active member.
     */
    ~Storage() {}
  } _storage;
};
