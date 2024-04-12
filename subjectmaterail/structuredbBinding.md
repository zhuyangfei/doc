# structured bindings

## example
### before
``` cpp
struct MyProfile
{
    int age;
    std::string name;
};

MyProfile getMyProfile();

{
    // example 1:
    MyProfile myProfile = getMyProfile();
    cout << myProfile.age << myProfile.name;

    // example 2:
    std::map<string, string> teamMembers {{"john","shanghai"}, {"joey", "hangzhou"}};
    for (const auto& teamMember : teamMembers)
    {
        cout << teamMember.first << teamMember.second <<'\n';
    }

}
```
### after
```cpp
{
    // example 1:
    auto[age, name] = getMyProfile();
    cout << age << name;

    // example 2:
    std::map<string, string> teamMembers {{"john","shanghai"}, {"joey", "hangzhou"}};
    for (const auto& [name, city] : teamMembers)
    {
        cout << name << city <<'\n';
    }
}
```

## benefit/why
1. To make a readable code
2. To use structure more convenient.

## how about performance?
code performance is the same.
```cpp
#include <iostream>
#include <map>
static void structuredNormal(benchmark::State& state) {
  // Code inside this loop is measured repeatedly
  std::map<std::string, std::string> teamMembers {{"john","shanghai"}, {"joey", "hangzhou"}};
  for (auto _ : state) {
    for (const auto& teamMember : teamMembers)
    {
        std::cout << teamMember.first << teamMember.second <<'\n';
    }
  }
}
// Register the function as a benchmark
BENCHMARK(structuredNormal);

static void structuredBinding(benchmark::State& state) {
  std::map<std::string, std::string> teamMembers {{"john","shanghai"}, {"joey", "hangzhou"}};
  for (auto _ : state) {
    //std::string copy(x);
    for (const auto& [name, city] : teamMembers)
    {
        std::cout << name << city <<'\n';
    }
  }
}
BENCHMARK(structuredBinding);
```

## where to use
1. struct or class with all public members
2. c style array
3. tuple-like object, like std::pair<>, std::array<>, std::tuple<>

[structured bindings](c17-the-complete-guide_compress.pdf)