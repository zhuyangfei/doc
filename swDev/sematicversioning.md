# semantic versioning

## why use sematic versioning
software version sematic is used to manage packages, subsystem or component dependency. The sematic versioning convey the meaning what software updated, so API user can know whether need to recompile their code or use a new package with compatible change directly.

## usage example
with automake compiler syscom, for example
require the `example` at least version 1.2.0, then print message.
```c
PKG_CHECK_MODULES([EXAMPLE], [example], [1.2.0], [AC_MSG_NOTICE([Found example package])], [AC_MSG_ERROR([Required example package not found])])
PKG_CHECK_MODULES([LIBGENAPI], [libgenapi >= 1.3.0])
```

## semantic versioning
For one thing, **it need to declare a public API.This may consist of documentation or be enforced by the code itself.**

The version with format of Major.minor.patch, i.e. X.Y.Z
Major increase, when incompatible API change happens
Minor increase, when compatible change happens, such as add new APIs or refactor private implementations.
Patch increase, when make backward compatible a bug.

A high level change, then reset low level number to zero, such as, if Major increase, then both minor and patch reset to zero, if Only minor increase, then reset patch to zero.

a pre-version release can be marked with a hyphen followed by x.y.z, for example 1.0.0-alpha, 1.0.0-0.2.1

metadata can be marked with plus followed by x.y.z, such as 1.0.0+abc03xs, but metadata should be excluded when compare the version.

### version compare
exclude metadata, the big version value is the newer version.

## good practice
1. a initial project
a initial project should begin with 0.1.0, then increase minor for each subsequent release.
2. when to set version 1.0.0?
   - if your software used in production
   - if you have a stable API on which users have come to depend
3. link https://semver.org/ to README

## note:
introduce compiler analysis to an API don't take as an incompatible API change, so don't need to increase Major number.

## reference
https://semver.org/
