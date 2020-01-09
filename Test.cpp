namespace llvm_libc {

Test *Test::Start = nullptr;
Test *Test::End = nullptr;

void Test::AddTest(Test *T) {
  if (End == nullptr) {
    Start = T;
    End = T;
    return;
  }

  End->Next = T;
  End = T;
}

int Test::RunTests() {
  Test *T = Start;
  while (T != nullptr) {
    T->Run();
    T = T->Next;
  }
}

void Test::SetUp() {}
void Test::TearDown() {}

} // namespace llvm_libc

int main() {
  return llvm_libc::Test::RunTests();
}
