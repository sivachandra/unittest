//===--------- Implementation of the base class for libc unittests --------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "Test.h"

#include "llvm/ADT/StringRef.h"
#include "llvm/Support/raw_ostream.h"

#include <string>

namespace llvm_libc {

class RunContext {
public:
  enum RunStatus { STATUS_PASS = 1, STATUS_FAIL = 2 };

  RunStatus status() const { return Status; }

  void markFail() { Status = STATUS_FAIL; }

private:
  RunStatus Status = STATUS_PASS;
};

} // namespace llvm_libc

template <typename ValType>
static bool test(llvm_libc::RunContext &Ctx, llvm_libc::Test::Condition Cond,
                 ValType LHS, ValType RHS, const char *LHSStr,
                 const char *RHSStr, const char *File, unsigned long Line) {
  if (Cond == llvm_libc::Test::Cond_EQ) {
    if (LHS == RHS)
      return true;

    Ctx.markFail();
    llvm::outs() << File << ":" << Line << ": FAILURE\n"
                 << "      Expected: " << LHSStr << '\n'
                 << "      Which is: " << LHS << '\n'
                 << "To be equal to: " << RHSStr << '\n'
                 << "      Which is: " << RHS << '\n';

    return false;
  }

  if (Cond == llvm_libc::Test::Cond_NE) {
    if (LHS != RHS)
      return true;

    Ctx.markFail();
    llvm::outs() << File << ":" << Line << ": FAILURE\n"
                 << "          Expected: " << LHSStr << '\n'
                 << "          Which is: " << LHS << '\n'
                 << "To be not equal to: " << RHSStr << '\n'
                 << "          Which is: " << RHS << '\n';
    return false;
  }

  if (Cond == llvm_libc::Test::Cond_LT) {
    if (LHS < RHS)
      return true;

    Ctx.markFail();
    llvm::outs() << File << ":" << Line << ": FAILURE\n"
                 << "       Expected: " << LHSStr << '\n'
                 << "       Which is: " << LHS << '\n'
                 << "To be less than: " << RHSStr << '\n'
                 << "       Which is: " << RHS << '\n';
    return false;
  }

  if (Cond == llvm_libc::Test::Cond_LE) {
    if (LHS <= RHS)
      return true;

    Ctx.markFail();
    llvm::outs() << File << ":" << Line << ": FAILURE\n"
                 << "                   Expected: " << LHSStr << '\n'
                 << "                   Which is: " << LHS << '\n'
                 << "To be less than or equal to: " << RHSStr << '\n'
                 << "                   Which is: " << RHS << '\n';
    return false;
  }

  if (Cond == llvm_libc::Test::Cond_GT) {
    if (LHS > RHS)
      return true;

    Ctx.markFail();
    llvm::outs() << File << ":" << Line << ": FAILURE\n"
                 << "          Expected: " << LHSStr << '\n'
                 << "          Which is: " << LHS << '\n'
                 << "To be greater than: " << RHSStr << '\n'
                 << "          Which is: " << RHS << '\n';
    return false;
  }

  if (Cond == llvm_libc::Test::Cond_GE) {
    if (LHS >= RHS)
      return true;

    Ctx.markFail();
    llvm::outs() << File << ":" << Line << ": FAILURE\n"
                 << "                      Expected: " << LHSStr << '\n'
                 << "                      Which is: " << LHS << '\n'
                 << "To be greater than or equal to: " << RHSStr << '\n'
                 << "                      Which is: " << RHS << '\n';
    return false;
  }

  Ctx.markFail();
  llvm::outs() << "Unexpected test condition.\n";
  return false;
}

namespace llvm_libc {

Test::Test(const char *N) : Name(N) {}

Test *Test::Start = nullptr;
Test *Test::End = nullptr;

void Test::addTest(Test *T) {
  if (End == nullptr) {
    Start = T;
    End = T;
    return;
  }

  End->Next = T;
  End = T;
}

int Test::runTests() {
  Test *T = Start;
  if (T == nullptr)
    return 0;

  int TestCount = 0;
  int FailCount = 0;
  do {
    ++TestCount;
    llvm::outs() << "[ RUN      ] " << T->Name << '\n';
    RunContext Ctx;
    T->SetUp();
    T->Run(Ctx);
    T->TearDown();
    auto Status = Ctx.status();
    if (Status == RunContext::STATUS_FAIL) {
      llvm::outs() << "[  FAILED  ] " << T->Name << '\n';
      ++FailCount;
    } else if (Status == RunContext::STATUS_PASS) {
      llvm::outs() << "[       OK ] " << T->Name << '\n';
    } else {
      llvm::outs() << "Unexpected status after running " << T->Name << '\n';
    }
    T = T->Next;
  } while (T != nullptr);

  llvm::outs() << "Ran " << TestCount << " tests. "
               << " PASS: " << TestCount - FailCount << ' '
               << " FAIL: " << FailCount << '\n';

  return FailCount > 0 ? 1 : 0;
}

template <>
bool Test::test<char>(RunContext &Ctx, Test::Condition Cond, char LHS, char RHS,
                      const char *LHSStr, const char *RHSStr, const char *File,
                      unsigned long Line) {
  return ::test(Ctx, Cond, LHS, RHS, LHSStr, RHSStr, File, Line);
}

template <>
bool Test::test<short>(RunContext &Ctx, Test::Condition Cond, short LHS,
                       short RHS, const char *LHSStr, const char *RHSStr,
                       const char *File, unsigned long Line) {
  return ::test(Ctx, Cond, LHS, RHS, LHSStr, RHSStr, File, Line);
}

template <>
bool Test::test<int>(RunContext &Ctx, Test::Condition Cond, int LHS, int RHS,
                     const char *LHSStr, const char *RHSStr, const char *File,
                     unsigned long Line) {
  return ::test(Ctx, Cond, LHS, RHS, LHSStr, RHSStr, File, Line);
}

template <>
bool Test::test<long>(RunContext &Ctx, Test::Condition Cond, long LHS, long RHS,
                      const char *LHSStr, const char *RHSStr, const char *File,
                      unsigned long Line) {
  return ::test(Ctx, Cond, LHS, RHS, LHSStr, RHSStr, File, Line);
}

template <>
bool Test::test<long long>(RunContext &Ctx, Test::Condition Cond, long long LHS,
                           long long RHS, const char *LHSStr,
                           const char *RHSStr, const char *File,
                           unsigned long Line) {
  return ::test(Ctx, Cond, LHS, RHS, LHSStr, RHSStr, File, Line);
}

template <>
bool Test::test<unsigned char>(RunContext &Ctx, Test::Condition Cond,
                               unsigned char LHS, unsigned char RHS,
                               const char *LHSStr, const char *RHSStr,
                               const char *File, unsigned long Line) {
  return ::test(Ctx, Cond, LHS, RHS, LHSStr, RHSStr, File, Line);
}

template <>
bool Test::test<unsigned short>(RunContext &Ctx, Test::Condition Cond,
                                unsigned short LHS, unsigned short RHS,
                                const char *LHSStr, const char *RHSStr,
                                const char *File, unsigned long Line) {
  return ::test(Ctx, Cond, LHS, RHS, LHSStr, RHSStr, File, Line);
}

template <>
bool Test::test<unsigned int>(RunContext &Ctx, Test::Condition Cond,
                              unsigned int LHS, unsigned int RHS,
                              const char *LHSStr, const char *RHSStr,
                              const char *File, unsigned long Line) {
  return ::test(Ctx, Cond, LHS, RHS, LHSStr, RHSStr, File, Line);
}

template <>
bool Test::test<unsigned long>(RunContext &Ctx, Test::Condition Cond,
                               unsigned long LHS, unsigned long RHS,
                               const char *LHSStr, const char *RHSStr,
                               const char *File, unsigned long Line) {
  return ::test(Ctx, Cond, LHS, RHS, LHSStr, RHSStr, File, Line);
}

template <>
bool Test::test<unsigned long long>(RunContext &Ctx, Test::Condition Cond,
                                    unsigned long long LHS,
                                    unsigned long long RHS, const char *LHSStr,
                                    const char *RHSStr, const char *File,
                                    unsigned long Line) {
  return ::test(Ctx, Cond, LHS, RHS, LHSStr, RHSStr, File, Line);
}

template <>
bool Test::test<void *>(RunContext &Ctx, Test::Condition Cond, void *LHS,
                        void *RHS, const char *LHSStr, const char *RHSStr,
                        const char *File, unsigned long Line) {
  return ::test(Ctx, Cond, LHS, RHS, LHSStr, RHSStr, File, Line);
}

bool Test::testStrEq(RunContext &Ctx, const char *LHS, const char *RHS,
                     const char *LHSStr, const char *RHSStr, const char *File,
                     unsigned long Line) {
  return ::test(Ctx, Cond_EQ, llvm::StringRef(LHS), llvm::StringRef(RHS),
                LHSStr, RHSStr, File, Line);
}

bool Test::testStrNe(RunContext &Ctx, const char *LHS, const char *RHS,
                     const char *LHSStr, const char *RHSStr, const char *File,
                     unsigned long Line) {
  return ::test(Ctx, Cond_NE, llvm::StringRef(LHS), llvm::StringRef(RHS),
                LHSStr, RHSStr, File, Line);
}

} // namespace llvm_libc

int main() { return llvm_libc::Test::runTests(); }
