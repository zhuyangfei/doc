# TDD practice in project And good UT practice

## a component requirement example
### requirement
A PV(persistent volume) management for trace log File.
1. A configurable space in PV to save trace log file
2. get a trace log file from PV, if the space not enough, then override the oldest trace log file.
3. only can override the files with the extension of `.tmp`, if all files are locked, i.e can not be deleted, then get a trace log file failure.

### Scenarios need to be verified
you can think of some scenarios need be tested before writing code.
1. get a trace log file success, if the space is enough, and will not override any trace log files.
2. should override the oldest trace log file, when space is not enough
3. should only override the `.tmp` file
4. if space is not enough and all files are locked, then fail to get a trace log

All scenarios should be tested and pass, no matter how the code implementations.

## TDD commit
```bash
f20e582 (HEAD -> pvmgrTdd) Add UT CreateFileFailedWhenAllFileLockedBySnapshotAndNoSpaceLeft
f4e9406 Add UT case ShouldOverrideTheOldestFilesIfPVIsFull
f9203c2 Add UT cases of CreateFileSuccessIfPVIsNotFull
f786224 TraceFilePVMgrShouldNoCopyableAndMoveable
```

## first TDD cases
[0001-TraceFilePVMgrShouldNoCopyableAndMoveable.patch](./0001-TraceFilePVMgrShouldNoCopyableAndMoveable.patch)
[0002-Add-UT-cases-of-CreateFileSuccessIfPVIsNotFull.patch](./0002-Add-UT-cases-of-CreateFileSuccessIfPVIsNotFull.patch)

### my experience
The first function UT case is always tough, but when you come cross it, the follows will be easy.
   1. As you need think clearly about how to test and what to test beforehand. You need to consider most of dependency components.
   2. You need have a vision of implementation beforehand. It's really a good practice for your imagination.
   3. you need time to get used to it.

## second and other TDD cases
[0003-Add-UT-case-ShouldOverrideTheOldestFilesIfPVIsFull.patch](./0003-Add-UT-case-ShouldOverrideTheOldestFilesIfPVIsFull.patch)
[0004-Add-UT-CreateFileFailedWhenAllFileLockedBySnapshotAn.patch](./0004-Add-UT-CreateFileFailedWhenAllFileLockedBySnapshotAn.patch)

## good UT practice
Writing UT according to the requirement or what you want to test, not according to the source code or the implementation. **always force on functions**.
you can practice this, even you have written source code. example:
### first commit
```cpp
bool TraceLogFile::writeBufferUntilError(const unsigned char* buffer, size_t bufferSize) noexcept
{
    ssize_t ret{0};
    size_t leftNumber = bufferSize;
    while(leftNumber)
    {
        ret = system.write(traceFileFd, buffer+ret, leftNumber);
        if(-1 == ret)
        {
            return false;
        }
        if ((size_t)ret == bufferSize)
        {
            return true;
        }
        leftNumber = bufferSize - ret;
    }

    return true;
}
```
https://gerrite1.ext.net.nokia.com/c/scm_rcp/RCPComMsgMCU/+/1496755/1..2/Services/MsgTrace/TraceLogFile.cpp

### commit after UT
```cpp
bool TraceLogFile::writeBufferUntilError(const unsigned char* buffer, size_t bufferSize) noexcept
{
    ssize_t ret{0};
    size_t leftNumber = bufferSize;
    size_t index = 0u;
    while(leftNumber)
    {
        ret = system.write(traceFileFd, buffer+index, leftNumber);
        if(fileOperationFailed == ret)
        {
            return false;
        }
        if ((size_t)ret == leftNumber)
        {
            break;
        }
        leftNumber -= ret;
        index += ret;
    }

    return true;
}
```
```cpp
TEST(TraceLogFileTest, getMessageLengthShouldEqualWriteBufferSizeWhenWrittenPartialDataOneTime)
{
    // the writeNum can't assign to 2, as cppUtest expectation checking order
    // it will first match write in expectWriteMsgLengthBeforeClose, then
    // in expectWriteBufferPartiallyOneTimeSuccess, which will case failed
    // it's a limitation
    ssize_t writeNum = 3;
    expectOpenAndCloseFile();
    expectInsertHeaderSuccess(true);
    expectWriteBufferPartiallyOneTimeSuccess(buffer.c_str(), buffer.length(), writeNum);
    expectWriteMsgLengthBeforeClose();

    const std::string msgWithEvenLength = "123456";
    expectWriteBufferPartiallyOneTimeSuccess(msgWithEvenLength.c_str(), msgWithEvenLength.length(), writeNum);

    TraceLogFile traceLog (systemMock, fileName, cbSwVersion);
    CHECK(traceLog.write((const unsigned char*)(buffer.c_str()), buffer.length()));
    CHECK(traceLog.write((const unsigned char*)(msgWithEvenLength.c_str()), msgWithEvenLength.length()));
    CHECK_EQUAL(traceLog.getMessageTotalLength(), buffer.length() + msgWithEvenLength.length());
}
```
This UT case failed, then i find a bug in the source code.

https://gerrite1.ext.net.nokia.com/c/scm_rcp/RCPComMsgMCU/+/1496755/2..6/Services/MsgTrace/TraceLogFile.cpp

### what's benefit
1. you can force on testing functions instead of implementation. some classic hiccups will never trouble you again, such as how to test private functions.
   As you know that when functions are correct, then those private functions will be definitely correct, and also be covered or tested.
   This is the meaning of `test via public API`.
2. It may help you to find some bugs.
3. You can map UT case to `ATC` more easy than before.
4. Written more readability and maintainability UT cases.
   - As you write UT cases force on functions, you will name a UT case and expectations naturally with better readability. such as
   ```cpp
   TEST(TraceLogFileTest, getMessageLengthShouldEqualsWriteBufferSizeWhenWriteSuccess)
   {
       expectOpenAndCloseFile();
       expectInsertHeaderSuccess(true);
       expectWriteBufferSuccess(true);
       expectWriteMsgLengthBeforeClose();
       TraceLogFile traceLog (systemMock, fileName, cbSwVersion);
       CHECK(traceLog.write((const unsigned char*)(buffer.c_str()), buffer.length()));
       CHECK_EQUAL(traceLog.getMessageTotalLength(), buffer.length());
   }
   ```
   - As the expectations are written in a high level, so those should not be affected by implement, such as when you refactor the code.
5. you are confident with your code.
   Take me as an example, I'm confident with my code, not as my programming skill, but as the code has already been tested.

I really enjoy the description of motivation written by Jan, It says that
- **See the code run. See it right now!**
- **Make sure that the code does what I want it to do**
- Make code look beautiful in an instant
- Create stepping stone for new things to be added
- Come back to code after year and be able to change it in an instant
For more information you can see, https://confluence.ext.net.nokia.com/display/RCP/UT+motivation.
