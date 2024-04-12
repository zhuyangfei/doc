# RAII

## when to use
1. resource shall be acquired before use.
2. a **lifetime** resource

## when it's can't be used
1. RAII is not suitable for all paired-call, such as syscom's `registerCPID` and `registerCPID`. though the `registerCPID` and `registerCPID` should be pair called. but the resource `CPID` is not owned by the wrapper object, it's actually owned by `syscom`, so a RAII wrapper is not suitable.
**A RAII object must own the resource and responsible to acquire and release resource.**

## note:
**RAII does not apply to the management of the resources that are not acquired before use**: CPU time, cores, and cache capacity, entropy pool capacity, network bandwidth, electric power consumption, stack memory.

### scenarios of dtor can't be called
1. premature exits, such as std::terminate, std::abort
[scenarios of dtor can't be called](https://stackoverflow.com/questions/3179494/under-what-circumstances-are-c-destructors-not-going-to-be-called)

## reference
[RAII](https://en.cppreference.com/w/cpp/language/raii)