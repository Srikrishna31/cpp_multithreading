//
// Created by coolk on 24-03-2026.
//
// Copyright Aeva 2026

#include "logger.h"

namespace aeva::safe::logger {

    class LogQueue {

    };

    void Logger::init(const char* logger_name) noexcept {
        instance_ = std::unique_ptr<Logger>(new Logger());
    }

    Logger::Logger() = default;

    Logger::~Logger()
    {

    }

    Logger& Logger::instance() {
        return *instance_;
    }

    void Logger::log(LogLevel level, const char* message) {
        //TODO: Implement the log function which pushes the message to queue.

    }

    void Logger::info(const char* message) {
        log(LogLevel::kLOG_LEVEL_INFO, message);
    }

    void Logger::debug(const char* message) {
        log(LogLevel::kLOG_LEVEL_DEBUG, message);
    }

    void Logger::warn(const char* message) {
        log(LogLevel::kLOG_LEVEL_WARN, message);
    }

    void Logger::error(const char* message) {
        log(LogLevel::kLOG_LEVEL_ERROR, message);
    }

    void Logger::shutdown() {
        // TODO: Cleanup, by flushing existing log messages,
        // and then return.
    }

} // namespace aeva::safe::logger


