//
// Created by coolk on 24-03-2026.
//

#ifndef LOGGER_LOG_H
#define LOGGER_LOG_H

// Copyright Aeva 2026

#include "Logger.h"

constexpr auto BUILD_LOG_LEVEL =
    aeva::safe::logger::LogLevel::kLOG_LEVEL_ERROR;

#define SAFE_LOG_ENABLED(LEVEL) (BUILD_LOG_LEVEL <= LEVEL)

#define SAFE_LOG_EMIT(FMT, ...) \
  aeva::safe::logger::Logger::instance().log(__FILE__, __LINE__, FMT, ##__VA_ARGS__);

#define SAFE_LOG_POLICY_ALWAYS(BODY) BODY

#define LOG_POLICY_IF(COND, BODY) \
  do {                            \
    if (COND) {                   \
      BODY                        \
    }                             \
  } while (0);

#define LOG_POLICY_EVERY_N(N, BODY)         \
  do {                                      \
    static std::atomic<uint32_t> _cnt = 0U; \
    if (_cnt == 0U) {                       \
      BODY                                  \
    }                                       \
    _cnt++;                                 \
    if (_cnt >= (N)) {                      \
      _cnt = 0U;                            \
    }                                       \
  } while (0)

#define LOG_POLICY_IF_EVERY_N(COND, N, BODY) \
  do {                                       \
    static std::atomic<uint32_t> _cnt = 0U;  \
    if (COND) {                              \
      if (_cnt == 0U) {                      \
        BODY                                 \
      }                                      \
      _cnt++;                                \
      if (_cnt >= (N)) {                     \
        _cnt = 0U;                           \
      }                                      \
    }                                        \
  } while (0)

#define LOG_POLICY_FIRST_N(N, BODY)         \
  do {                                      \
    static std::atomic<uint32_t> _cnt = 0U; \
    if (_cnt < (N)) {                       \
      BODY _cnt++;                          \
    }                                       \
  } while (0)

#define LOG_INTERNAL(LEVEL, POLICY)     \
  do {                                  \
    if constexpr (LOG_ENABLED(LEVEL)) { \
      POLICY                            \
    }                                   \
  } while (0)

#define SAFE_LOG_DEBUG(FMT, ...) \
  LOG_INTERNAL(aeva::safe::logger::LogLevel::kLOG_LEVEL_DEBUG, FMT, __VA_ARGS__)
#define SAFE_LOG_INFO(FMT, ...) \
  LOG_INTERNAL(aeva::safe::logger::LogLevel::kLOG_LEVEL_INFO, FMT, __VA_ARGS__)
#define SAFE_LOG_WARN(FMT, ...) \
  LOG_INTERNAL(aeva::safe::logger::LogLevel::kLOG_LEVEL_INFO, FMT, __VA_ARGS__)
#define SAFE_LOG_ERROR(FMT, ...) \
  LOG_INTERNAL(aeva::safe::logger::LogLevel::kLOG_LEVEL_ERROR, FMT, __VA_ARGS__)

#define SAFE_LOG_DEBUG_IF(COND, FMT, ...)                      \
  LOG_INTERNAL(aeva::safe::logger::LogLevel::kLOG_LEVEL_DEBUG, \
               LOG_POLICY_IF(COND, SAFE_LOG_EMIT(FMT, __VA_ARGS__)))

#define SAFE_LOG_WARN_IF(COND, FMT, ...)                      \
  LOG_INTERNAL(aeva::safe::logger::LogLevel::kLOG_LEVEL_WARN, \
               LOG_POLICY_IF(COND, SAFE_LOG_EMIT(FMT, __VA_ARGS__)))

#define SAFE_LOG_INFO_IF(COND, FMT, ...)                      \
  LOG_INTERNAL(aeva::safe::logger::LogLevel::kLOG_LEVEL_INFO, \
               LOG_POLICY_IF(COND, SAFE_LOG_EMIT(FMT, __VA_ARGS__)))

#define SAFE_LOG_ERROR_IF(COND, FMT, ...)                      \
  LOG_INTERNAL(aeva::safe::logger::LogLevel::kLOG_LEVEL_ERROR, \
               LOG_POLICY_IF(COND, SAFE_LOG_EMIT(FMT, __VA_ARGS__)))

#define SAFE_LOG_DEBUG_EVERY_N(N, FMT, ...)                            \
  LOG_INTERNAL(aeva::safe::logger::LogLevel::kLOG_LEVEL_DEBUG, MODULE, \
               LOG_POLICY_EVERY_N(N, SAFE_LOG_EMIT(FMT, __VA_ARGS__)))

#define SAFE_LOG_INFO_EVERY_N(N, FMT, ...)                            \
  LOG_INTERNAL(aeva::safe::logger::LogLevel::kLOG_LEVEL_INFO, MODULE, \
               LOG_POLICY_EVERY_N(N, SAFE_LOG_EMIT(FMT, __VA_ARGS__)))

#define SAFE_LOG_WARN_EVERY_N(N, FMT, ...)                            \
  LOG_INTERNAL(aeva::safe::logger::LogLevel::kLOG_LEVEL_WARN, MODULE, \
               LOG_POLICY_EVERY_N(N, SAFE_LOG_EMIT(FMT, __VA_ARGS__)))

#define SAFE_LOG_ERROR_EVERY_N(N, FMT, ...)                            \
  LOG_INTERNAL(aeva::safe::logger::LogLevel::kLOG_LEVEL_ERROR, MODULE, \
               LOG_POLICY_EVERY_N(N, SAFE_LOG_EMIT(FMT, __VA_ARGS__)))

#define SAFE_LOG_DEBUG_FIRST_N(N, FMT, ...)                    \
  LOG_INTERNAL(aeva::safe::logger::LogLevel::kLOG_LEVEL_DEBUG, \
               LOG_POLICY_FIRST_N(N, SAFE_LOG_EMIT(FMT, __VA_ARGS__)))

#define SAFE_LOG_INFO_FIRST_N(N, FMT, ...)                    \
  LOG_INTERNAL(aeva::safe::logger::LogLevel::kLOG_LEVEL_INFO, \
               LOG_POLICY_FIRST_N(N, SAFE_LOG_EMIT(FMT, __VA_ARGS__)))

#define SAFE_LOG_WARN_FIRST_N(N, FMT, ...)                    \
  LOG_INTERNAL(aeva::safe::logger::LogLevel::kLOG_LEVEL_WARN, \
               LOG_POLICY_FIRST_N(N, SAFE_LOG_EMIT(FMT, __VA_ARGS__)))

#define SAFE_LOG_ERROR_FIRST_N(N, FMT, ...)                    \
  LOG_INTERNAL(aeva::safe::logger::LogLevel::kLOG_LEVEL_ERROR, \
               LOG_POLICY_FIRST_N(N, SAFE_LOG_EMIT(FMT, __VA_ARGS__)))

#define SAFE_LOG_DEBUG_IF_EVERY_N(COND, N, FMT, ...)                   \
  LOG_INTERNAL(aeva::safe::logger::LogLevel::kLOG_LEVEL_DEBUG, MODULE, \
               LOG_POLICY_IF_EVERY_N(COND, N, SAFE_LOG_EMIT(FMT, __VA_ARGS__)))

#define SAFE_LOG_WARN_IF_EVERY_N(COND, N, FMT, ...)                   \
  LOG_INTERNAL(aeva::safe::logger::LogLevel::kLOG_LEVEL_WARN, MODULE, \
               LOG_POLICY_IF_EVERY_N(COND, N, SAFE_LOG_EMIT(FMT, __VA_ARGS__)))

#define SAFE_LOG_ERROR_IF_EVERY_N(COND, N, FMT, ...)                   \
  LOG_INTERNAL(aeva::safe::logger::LogLevel::kLOG_LEVEL_ERROR, MODULE, \
               LOG_POLICY_IF_EVERY_N(COND, N, SAFE_LOG_EMIT(FMT, __VA_ARGS__)))

#define SAFE_LOG_INFO_IF_EVERY_N(COND, N, FMT, ...)                   \
  LOG_INTERNAL(aeva::safe::logger::LogLevel::kLOG_LEVEL_INFO, MODULE, \
               LOG_POLICY_IF_EVERY_N(COND, N, SAFE_LOG_EMIT(FMT, __VA_ARGS__)))

#endif //LOGGER_LOG_H