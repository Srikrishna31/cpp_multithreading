#pragma once

#include <stdexcept>
#include <utility>

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
public:
  static Result Success(T value) { return Result(std::move(value)); }
  static Result Failure(E error) { return Result(std::move(error)); }

  bool isSuccess() const noexcept { return _isSuccess; }
  bool isFailure() const noexcept { return !_isSuccess; }
  explicit operator bool() const noexcept { return _isSuccess; }

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
  explicit Result(T value) : _isSuccess(true), _storage(value) {}
  explicit Result(E error) : _isSuccess(false), _storage(error) {}

  bool _isSuccess;
  union Storage {
    T _value;
    E _error;

    Storage() {}
    Storage(T value) : _value(value) {}
    Storage(E error) : _error(error) {}
    ~Storage() {}
  } _storage;
};
