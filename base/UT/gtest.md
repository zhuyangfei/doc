# gtest

## Death test
EXPECT_CALL要放到Death test语句的里面，这样才不会出现"EXPECT_CALL没有被调用的异常”。如
```cpp
TEST_F(DummyTest, xxxx)
{
EXPECT_EXIT(
    EXPECT_CALL(mock, func());
    funcWithAbort(),
    KilledBySignal(SIGABRT), ".*Log.*");
}
// EXPECT_CALL(mock, func());  // setup before call funcWithAbort()
// funcWithAbort(), expression will cause abort
```

## gtest ASSERT_ vs EXPECT_EQ
ASSERT_, 失败后，当前function返回，等效为return，ASSERT_之后的代码不能被执行，不影响其他test case。
EXPECT_，失败后，当前function继续执行。
```cpp
TEST_F(CTest, AssertUsage)
{
    Abc abc;
    MockA mockA(1);
    MockB mockB(mockA);
    C c(mockB);
    EXPECT_CALL(mockB, getValue()).WillOnce(Return(200));
    ASSERT_EQ(100, c.getValue());
    EXPECT_EQ(200, 100);
    std::cout << "end AssertUsage\n";
}


TEST_F(CTest, AssertUsage2)
{
    MockA mockA(1);
    MockB mockB(mockA);
    C c(mockB);
    EXPECT_CALL(mockB, getValue()).WillOnce(Return(200));
    EXPECT_EQ(100, c.getValue());
    EXPECT_EQ(200, 100);
    std::cout << "end AssertUsage2\n";
}

[ RUN      ] CTest.AssertUsage
construct
A construct
../tst/UT_C.cpp:56: Failure
Expected equality of these values:
  100
  c.getValue()
    Which is: 200
A deconstruct
deconstruct
[  FAILED  ] CTest.AssertUsage (0 ms)
[ RUN      ] CTest.AssertUsage2
construct
../tst/UT_C.cpp:67: Failure
Expected equality of these values:
  100
  c.getValue()
    Which is: 200
../tst/UT_C.cpp:68: Failure
Expected equality of these values:
  200
  100
end AssertUsage2
deconstruct
[  FAILED  ] CTest.AssertUsage2 (0 ms)

```
