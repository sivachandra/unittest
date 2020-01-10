//===------------------ Base class for libc unittests -----------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// This file should stricly not include any other file. Not even standard
// library headers.

namespace llvm_libc {

class RunContext;

class Test {
private:
  const char *Name;
  Test *Next = nullptr;

public:
  enum Condition {
    Cond_None,
    Cond_EQ,
    Cond_NE,
    Cond_LT,
    Cond_LE,
    Cond_GT,
    Cond_GE,
  };

  virtual ~Test() {}
  virtual void SetUp() {}
  virtual void TearDown() {}

  static int runTests();

protected:
  explicit Test(const char *N);

  static void addTest(Test *T);

  template <typename ValType>
  static bool test(RunContext &Ctx, Condition Cond, ValType LHS, ValType RHS,
                   const char *LHSStr, const char *RHSStr, const char *File,
                   unsigned long Line);

  static bool testStrEq(RunContext &Ctx, const char *LHS, const char *RHS,
                        const char *LHSStr, const char *RHSStr,
                        const char *File, unsigned long Line);

  static bool testStrNe(RunContext &Ctx, const char *LHS, const char *RHS,
                        const char *LHSStr, const char *RHSStr,
                        const char *File, unsigned long Line);

private:
  virtual void Run(RunContext &Ctx) = 0;

  static Test *Start;
  static Test *End;
};

} // namespace llvm_libc

#define TEST(SuiteName, TestName)                                              \
  class SuiteName##_##TestName : public llvm_libc::Test {                      \
  public:                                                                      \
    SuiteName##_##TestName() : Test(#SuiteName "." #TestName) {                \
      addTest(this);                                                           \
    }                                                                          \
    void Run(llvm_libc::RunContext &) override;                                \
  };                                                                           \
  SuiteName##_##TestName SuiteName##_##TestName##_Instance;                    \
  void SuiteName##_##TestName::Run(llvm_libc::RunContext &Ctx)

#define EXPECT_EQ(LHS, RHS)                                                    \
  llvm_libc::Test::test(Ctx, Cond_EQ, (LHS), (RHS), #LHS, #RHS, __FILE__,      \
                        __LINE__)
#define ASSERT_EQ(LHS, RHS)                                                    \
  if (!llvm_libc::Test::test(Ctx, Cond_EQ, (LHS), (RHS), #LHS, #RHS, __FILE__, \
                             __LINE__))                                        \
  return

#define EXPECT_STREQ(LHS, RHS)                                                 \
  llvm_libc::Test::testStrEq(Ctx, (LHS), (RHS), #LHS, #RHS, __FILE__, __LINE__)
#define ASSERT_STREQ(LHS, RHS)                                                 \
  if (!llvm_libc::Test::testStrEq(Ctx, (LHS), (RHS), #LHS, #RHS, __FILE__,     \
                                  __LINE__))                                   \
  return

#define EXPECT_NE(LHS, RHS)                                                    \
  llvm_libc::Test::test(Ctx, Cond_NE, (LHS), (RHS), #LHS, #RHS, __FILE__,      \
                        __LINE__)
#define ASSERT_NE(LHS, RHS)                                                    \
  if (!llvm_libc::Test::test(Ctx, Cond_NE, (LHS), (RHS), #LHS, #RHS, __FILE__, \
                             __LINE__))                                        \
  return
