#include "result.h"

#include <memory>
#include <string>
#include <utility>

int main() {
  auto mapped = Result<int, std::string>::Success(2).map(
      [](int value) { return value + 1; });
  if (mapped.Value() != 3) {
    return 1;
  }

  auto chained = std::move(mapped).andThen([](int value) {
    return Result<std::string, std::string>::Success(std::to_string(value));
  });
  if (chained.Value() != "3") {
    return 2;
  }

  auto mapped_error = Result<int, std::string>::Failure("bad").map_error(
      [](const std::string &error) { return error.size(); });
  if (mapped_error.Error() != 3U) {
    return 3;
  }

  auto recovered = std::move(mapped_error).or_else([](std::size_t error) {
    return Result<int, std::size_t>::Success(static_cast<int>(error));
  });
  if (recovered.Value() != 3) {
    return 4;
  }

  auto move_only = Result<std::unique_ptr<int>, std::string>::Success(
                       std::make_unique<int>(4))
                       .map([](std::unique_ptr<int> value) {
                         return *value + 1;
                       });
  if (move_only.Value() != 5) {
    return 5;
  }

  return 0;
}
