#include <gtest/gtest.h>

#include "src/result.h"

#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace {

using IntStringResult = Result<int, std::string>;

TEST(ResultShould, ConstructSuccessAndExposeState) {
  auto result = IntStringResult::Success(42);

  EXPECT_TRUE(result.isSuccess());
  EXPECT_FALSE(result.isFailure());
  EXPECT_TRUE(static_cast<bool>(result));
  EXPECT_EQ(result.Value(), 42);
}

TEST(ResultShould, ConstructFailureAndExposeState) {
  auto result = IntStringResult::Failure("error");

  EXPECT_FALSE(result.isSuccess());
  EXPECT_TRUE(result.isFailure());
  EXPECT_FALSE(static_cast<bool>(result));
  EXPECT_EQ(result.Error(), "error");
}

TEST(ResultShould, ThrowWhenAccessingValueFromFailure) {
  auto result = IntStringResult::Failure("error");

  try {
    (void)result.Value();
    FAIL() << "Expected ResultException";
  } catch (const ResultException &exception) {
    EXPECT_EQ(exception.code(), ResultErrorCode::ResultInFailureState);
    EXPECT_STREQ(exception.what(),
                 "Result in failure state, but requested success value");
  }
}

TEST(ResultShould, ThrowWhenAccessingErrorFromSuccess) {
  auto result = IntStringResult::Success(42);

  try {
    (void)result.Error();
    FAIL() << "Expected ResultException";
  } catch (const ResultException &exception) {
    EXPECT_EQ(exception.code(), ResultErrorCode::ResultInSuccessState);
    EXPECT_STREQ(exception.what(),
                 "Result in success state, but requested error value");
  }
}

TEST(ResultShould, ConvertUnknownErrorCodeToString) {
  const auto unknown_code = static_cast<ResultErrorCode>(99);

  EXPECT_STREQ(to_string(unknown_code), "Unknown error");
}

TEST(ResultShould, ConstructSuccessInPlaceFromArguments) {
  auto result = Result<std::string, int>::SuccessInPlace(3, 'x');

  EXPECT_TRUE(result.isSuccess());
  EXPECT_EQ(result.Value(), "xxx");
}

TEST(ResultShould, ConstructFailureInPlaceFromArguments) {
  auto result = Result<int, std::string>::FailureInPlace(2, 'e');

  EXPECT_TRUE(result.isFailure());
  EXPECT_EQ(result.Error(), "ee");
}

TEST(ResultShould, ConstructSuccessInPlaceFromInitializerList) {
  auto result =
      Result<std::vector<int>, std::string>::SuccessInPlace({1, 2, 3});

  EXPECT_TRUE(result.isSuccess());
  EXPECT_EQ(result.Value(), std::vector<int>({1, 2, 3}));
}

TEST(ResultShould, ConstructFailureInPlaceFromInitializerList) {
  auto result = Result<int, std::vector<int>>::FailureInPlace({4, 5, 6});

  EXPECT_TRUE(result.isFailure());
  EXPECT_EQ(result.Error(), std::vector<int>({4, 5, 6}));
}

TEST(ResultShould, SupportSameSuccessAndErrorType) {
  auto success = Result<std::string, std::string>::Success("ok");
  auto failure = Result<std::string, std::string>::Failure("bad");

  EXPECT_EQ(success.Value(), "ok");
  EXPECT_EQ(failure.Error(), "bad");
}

TEST(ResultShould, MoveConstructSuccess) {
  auto source = Result<std::unique_ptr<int>, std::string>::Success(
      std::make_unique<int>(7));
  auto moved = std::move(source);

  ASSERT_TRUE(moved.isSuccess());
  ASSERT_NE(moved.Value(), nullptr);
  EXPECT_EQ(*moved.Value(), 7);
}

TEST(ResultShould, MoveConstructFailure) {
  auto source = Result<int, std::unique_ptr<std::string>>::Failure(
      std::make_unique<std::string>("bad"));
  auto moved = std::move(source);

  ASSERT_TRUE(moved.isFailure());
  ASSERT_NE(moved.Error(), nullptr);
  EXPECT_EQ(*moved.Error(), "bad");
}

TEST(ResultShould, MoveAssignSuccessOverFailure) {
  auto target = IntStringResult::Failure("old");
  auto source = IntStringResult::Success(9);

  target = std::move(source);

  EXPECT_TRUE(target.isSuccess());
  EXPECT_EQ(target.Value(), 9);
}

TEST(ResultShould, MoveAssignFailureOverSuccess) {
  auto target = IntStringResult::Success(9);
  auto source = IntStringResult::Failure("new");

  target = std::move(source);

  EXPECT_TRUE(target.isFailure());
  EXPECT_EQ(target.Error(), "new");
}

TEST(ResultShould, SupportDeletedCopyOperations) {
  EXPECT_FALSE(std::is_copy_constructible<IntStringResult>::value);
  EXPECT_FALSE(std::is_copy_assignable<IntStringResult>::value);
  EXPECT_TRUE(std::is_move_constructible<IntStringResult>::value);
  EXPECT_TRUE(std::is_move_assignable<IntStringResult>::value);
}

TEST(ResultShould, AndThenOnLvalueSuccessInvokesCallable) {
  auto result = IntStringResult::Success(2);

  auto chained = result.andThen([](int &value) {
    value += 1;
    return Result<std::string, std::string>::Success(std::to_string(value));
  });

  EXPECT_EQ(result.Value(), 3);
  EXPECT_EQ(chained.Value(), "3");
}

TEST(ResultShould, AndThenOnConstLvalueSuccessInvokesCallable) {
  const auto result = IntStringResult::Success(4);

  auto chained = result.andThen([](const int &value) {
    return Result<std::string, std::string>::Success(std::to_string(value));
  });

  EXPECT_EQ(chained.Value(), "4");
}

TEST(ResultShould, AndThenOnRvalueSuccessMovesValue) {
  auto result = Result<std::unique_ptr<int>, std::string>::Success(
      std::make_unique<int>(5));

  auto chained = std::move(result).andThen([](std::unique_ptr<int> value) {
    return IntStringResult::Success(*value);
  });

  EXPECT_EQ(chained.Value(), 5);
}

TEST(ResultShould, AndThenPropagatesFailure) {
  auto result = IntStringResult::Failure("bad");

  auto chained = result.andThen([](int &) {
    return Result<std::string, std::string>::Success("unused");
  });

  EXPECT_TRUE(chained.isFailure());
  EXPECT_EQ(chained.Error(), "bad");
}

TEST(ResultShould, AndThenMovesFailureFromRvalue) {
  auto result = Result<int, std::unique_ptr<std::string>>::Failure(
      std::make_unique<std::string>("bad"));

  auto chained = std::move(result).andThen([](int) {
    return Result<std::string, std::unique_ptr<std::string>>::Success("unused");
  });

  ASSERT_TRUE(chained.isFailure());
  ASSERT_NE(chained.Error(), nullptr);
  EXPECT_EQ(*chained.Error(), "bad");
}

TEST(ResultShould, MapOnLvalueSuccessTransformsValue) {
  auto result = IntStringResult::Success(2);

  auto mapped = result.map([](int &value) {
    value += 3;
    return std::to_string(value);
  });

  EXPECT_EQ(result.Value(), 5);
  EXPECT_EQ(mapped.Value(), "5");
}

TEST(ResultShould, MapOnConstLvalueSuccessTransformsValue) {
  const auto result = IntStringResult::Success(6);

  auto mapped = result.map([](const int &value) {
    return std::to_string(value);
  });

  EXPECT_EQ(mapped.Value(), "6");
}

TEST(ResultShould, MapOnRvalueSuccessMovesValue) {
  auto result = Result<std::unique_ptr<int>, std::string>::Success(
      std::make_unique<int>(8));

  auto mapped = std::move(result).map([](std::unique_ptr<int> value) {
    return *value;
  });

  EXPECT_EQ(mapped.Value(), 8);
}

TEST(ResultShould, MapPropagatesFailure) {
  auto result = IntStringResult::Failure("bad");

  auto mapped = result.map([](int) { return std::string("unused"); });

  EXPECT_TRUE(mapped.isFailure());
  EXPECT_EQ(mapped.Error(), "bad");
}

TEST(ResultShould, MapMovesFailureFromRvalue) {
  auto result = Result<int, std::unique_ptr<std::string>>::Failure(
      std::make_unique<std::string>("bad"));

  auto mapped = std::move(result).map([](int) { return std::string("unused"); });

  ASSERT_TRUE(mapped.isFailure());
  ASSERT_NE(mapped.Error(), nullptr);
  EXPECT_EQ(*mapped.Error(), "bad");
}

TEST(ResultShould, MapErrorOnLvalueFailureTransformsError) {
  auto result = IntStringResult::Failure("bad");

  auto mapped = result.map_error([](std::string &error) {
    error += "!";
    return error.size();
  });

  EXPECT_EQ(result.Error(), "bad!");
  EXPECT_EQ(mapped.Error(), 4U);
}

TEST(ResultShould, MapErrorOnConstLvalueFailureTransformsError) {
  const auto result = IntStringResult::Failure("bad");

  auto mapped = result.map_error([](const std::string &error) {
    return error.size();
  });

  EXPECT_EQ(mapped.Error(), 3U);
}

TEST(ResultShould, MapErrorOnRvalueFailureMovesError) {
  auto result = Result<int, std::unique_ptr<std::string>>::Failure(
      std::make_unique<std::string>("bad"));

  auto mapped = std::move(result).map_error(
      [](std::unique_ptr<std::string> error) { return error->size(); });

  EXPECT_EQ(mapped.Error(), 3U);
}

TEST(ResultShould, MapErrorPropagatesSuccess) {
  auto result = IntStringResult::Success(11);

  auto mapped = result.map_error([](std::string &) { return 0U; });

  EXPECT_TRUE(mapped.isSuccess());
  EXPECT_EQ(mapped.Value(), 11);
}

TEST(ResultShould, MapErrorMovesSuccessFromRvalue) {
  auto result = Result<std::unique_ptr<int>, std::string>::Success(
      std::make_unique<int>(12));

  auto mapped = std::move(result).map_error(
      [](std::string &&) { return std::string("unused"); });

  ASSERT_TRUE(mapped.isSuccess());
  ASSERT_NE(mapped.Value(), nullptr);
  EXPECT_EQ(*mapped.Value(), 12);
}

TEST(ResultShould, OrElseOnLvalueFailureInvokesCallable) {
  auto result = IntStringResult::Failure("bad");

  auto recovered = result.or_else([](std::string &error) {
    error += "!";
    return IntStringResult::Success(static_cast<int>(error.size()));
  });

  EXPECT_EQ(result.Error(), "bad!");
  EXPECT_EQ(recovered.Value(), 4);
}

TEST(ResultShould, OrElseOnConstLvalueFailureInvokesCallable) {
  const auto result = IntStringResult::Failure("bad");

  auto recovered = result.or_else([](const std::string &error) {
    return IntStringResult::Success(static_cast<int>(error.size()));
  });

  EXPECT_EQ(recovered.Value(), 3);
}

TEST(ResultShould, OrElseOnRvalueFailureMovesError) {
  auto result = Result<int, std::unique_ptr<std::string>>::Failure(
      std::make_unique<std::string>("bad"));

  auto recovered = std::move(result).or_else(
      [](std::unique_ptr<std::string> error) {
        return IntStringResult::Success(static_cast<int>(error->size()));
      });

  EXPECT_EQ(recovered.Value(), 3);
}

TEST(ResultShould, OrElsePropagatesSuccess) {
  auto result = IntStringResult::Success(13);

  auto recovered = result.or_else([](std::string &) {
    return IntStringResult::Failure("unused");
  });

  EXPECT_TRUE(recovered.isSuccess());
  EXPECT_EQ(recovered.Value(), 13);
}

TEST(ResultShould, OrElseMovesSuccessFromRvalue) {
  auto result = Result<std::unique_ptr<int>, std::string>::Success(
      std::make_unique<int>(14));

  auto recovered = std::move(result).or_else([](std::string &&) {
    return Result<std::unique_ptr<int>, std::string>::Failure("unused");
  });

  ASSERT_TRUE(recovered.isSuccess());
  ASSERT_NE(recovered.Value(), nullptr);
  EXPECT_EQ(*recovered.Value(), 14);
}

}  // namespace

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
