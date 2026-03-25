//
// Created by coolk on 24-03-2026.
//

#ifndef LOGGER_LOGGER_H
#define LOGGER_LOGGER_H

#include <atomic>
#include <memory>
#include <cstring>


namespace aeva::safe::logger {

enum class LogLevel : uint8_t {
  kLOG_LEVEL_INFO = 0,
  kLOG_LEVEL_WARN = 1,
  kLOG_LEVEL_DEBUG = 2,
  kLOG_LEVEL_ERROR = 3
};

class LogQueue;

enum class ArgType : uint8_t {
  U32,
  I32,
  F32,
};

union ArgValue {
  uint32_t u32;
  int32_t i32;
  float f32;
};
struct LogArg {
  ArgType type;
  ArgValue value;
};
template <std::size_t N>
struct LogPayload {
  LogArg args[N];
  uint8_t count;
};

template <typename T>
constexpr LogArg encode_one(T value) {
  if constexpr (std::is_same_v<T, uint32_t>) {
    return {ArgType::U32, {.u32 = value}};
  } else if constexpr (std::is_same_v<T, int32_t>) {
    return {ArgType::I32, {.i32 = value}};
  } else if constexpr (std::is_same_v<T, float>) {
    return {ArgType::F32, {.f32 = value}};
  } else {
    static_assert(sizeof(T) == 0, "Unsupported type");
    return {ArgType::F32, {.f32 = value}};
  }
}

template <typename... Args>
constexpr auto make_payload(Args&&... args) {
  constexpr std::size_t N = sizeof...(args);

  LogPayload<N> payload{};
  payload.count = static_cast<uint8_t>(N);

  LogArg tmp[] = {encode_one(std::forward<Args>(args))...};

  for (std::size_t i = 0; i < N; ++i) {
    payload.args[i] = tmp[i];
  }

  return payload;
}

constexpr std::size_t MAX_ARGS = 8;

struct LogRecord {
  ArgType types[MAX_ARGS];
  uint32_t data[MAX_ARGS];  // raw bits (floats included)
  uint8_t count;
};

template <std::size_t N>
LogRecord to_record(uint32_t event_id, const LogPayload<N>& p) {
  static_assert(N <= MAX_ARGS, "A maximum of 8 arguments are supported");
  LogRecord r{};
  r.count = p.count;

  for (std::size_t i = 0; i < p.count; ++i) {
    r.types[i] = p.args[i].type;
    switch (p.args[i].type) {
      case ArgType::U32:
        r.data[i] = p.args[i].value.u32;
        break;
      case ArgType::I32:
        r.data[i] = p.args[i].value.i32;
        break;
      case ArgType::F32: {
        uint32_t bits;
        std::memcpy(&bits, &p.args[i].value.f32, sizeof(float));
        r.data[i] = bits;
        break;
      }
      default: ;
    }
  }

  return r;
}

class Logger {
 public:
  static void init(const char* logger_name) noexcept;

  static Logger& instance();

  void log(LogLevel level, const char* message);

  void info(const char* message);
  void debug(const char* message);
  void warn(const char* message);
  void error(const char* message);

  static inline uint32_t read_timestamp() {
    static std::atomic<uint32_t> counter{0};

    return counter.fetch_add(1U);
  }

  static void shutdown();

  ~Logger();

 private:
  Logger();

  std::unique_ptr<LogQueue> queue;
  static std::unique_ptr<Logger> instance_;
  const char* logger_name;
};

} // namespace aeva::safe::logger



#endif //LOGGER_LOGGER_H