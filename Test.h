namespace llvm_libc {

class Test {
 private:
  const char *Name;
  Test *Next = nullptr;

 public:
  virtual void SetUp();
  virtual void TearDown();

  static int RunTests();

 protected:
  explicit Test(const char *N) : Name(N) {}

  template <typename ValType>
  static bool TestEq(ValType LHS, ValType RHS, const char *File, unsigned long Line);

  static bool TestStrEq(const char *LHS, const char *RHS,
                        const char *File, unsigned long Line);

 private:
  virtual void Run() = 0;

  static void AddTest(Test *T);
  static void ReportEqError(const char *);

  static Test *Start;
  static Test *End;
};

#define TEST(SuiteName, TestName) \
    class SuiteName##_##TestName : public Test { \
     public: \
       SuiteName##_##TestName() : Test(#SuiteName "_" #TestName) { \
         AddTest(this); \
       } \
       virtual void Run() override; \
    }; \
    SuiteName##_##TestName SuiteName##_##TestName##_Instance; \
    void SuiteName##_##TestName::Run()

#define EXPECT_EQ(LHS, RHS) Test::TestEq((LHS), (RHS), __FILE__, __LINE__)
#define EXPECT_STREQ(LHS, RHS) Test::TestStrEq((LHS), (RHS), __FILE__, __LINE__)

#define ASSERT_EQ(LHS, RHS) \
    if (!Test::TestEq((LHS), (RHS), __FILE__, __LINE__)) return
#define ASSERT_STREQ(LHS, RHS) \
    if (!Test::TestStrEq((LHS), (RHS), __FILE__, __LINE__)) return

} // namespace llvm_libc
