# inline variable

## usage
```cpp
inline constexpr int MAX_COUNT = 16;
```

## motivation to introduce inline variable
1. make sure initializing have done before use
2. head file lib is available
3. variable has same address in each translation unit.
