# time

## std::chrono::system_clock
非线性time，能被修改。是一个相较于January 1, 1970, 00:00:00 UTC的时间

## std::chrono::steady_clock
线性的time, 只增不减，很适合用于统计时间间隔
通常，从系统重启时，开始计算

## epoch/unix time
Epoch time, also known as Unix time, is a system for representing points in time as a numeric value. It represents the number of seconds that have elapsed since January 1, 1970, 00:00:00 UTC (Coordinated Universal Time), not counting leap seconds. This means that every moment in time corresponds to a unique epoch time value.
是一个时间点，用一串数字表示. 该时刻从UTC 1970/01/01 00:00:00开始计算

## Year 2038 problem
As a 32 signed `time_t` overflowed
https://en.wikipedia.org/wiki/Year_2038_problem
