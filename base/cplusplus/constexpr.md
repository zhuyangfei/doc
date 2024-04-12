# constexpr

## constexpr variable
即编译时，可知的常量，如
```cpp
constexpr size_t MAX_BUFFER_SIZE = 10;
```

## constexpr expression
若参数为编译时确定，则`constexpr expression`在编译时，计算结果。如果参数在运行时确定，则`constexpr expression`退化为运行时确定。如：
```cpp
constexpr int pow(int base, int exp) noexcept
{
    auto ret = 1;
    for(unsigned int i = 0 ; i < exp; i++) ret *=base;
    return ret;
}
int getBase();
int getExp();
constexpr int pow2_3 = pow(2, 3);   // calculate in compilation
int ret = pow(getBase(), getExp()); // calculate in runtime
constexpr int pow2_4 = pow(getBase(), getExp()); // compile error: non-constexpr getBase
```
```
#include <iostream>
#include <cmath>
class Point
{
    public:
    constexpr Point(int x, int y)
        :x_(x), y_(y)
    {}
    constexpr auto getX() const noexcept {return x_;}
    constexpr auto getY() const noexcept {return y_;}
    constexpr void setX(int x) noexcept {x_ = x;}   // without const
    constexpr void setY(int y) noexcept {y_ = y;}
    private:
        int x_;
        int y_;
};

constexpr auto getDistance(const Point& p1, const Point& p2) noexcept
{
    auto deltaY = p2.getY() - p1.getY();    // only constexpr expression call be invoked in constexpr
    auto deltaX = p2.getX() - p1.getX();
    return std::sqrt(deltaY * deltaY + deltaX * deltaX);
}

constexpr Point reflection(const Point& p1)
{
    Point p(0,0);
    p.setX(-p1.getX());
    p.setY(-p1.getY());
    return p;
}


int main()
{
    constexpr Point p1(1,1);
    constexpr Point p2(2,2);
    // p1.setX(2);       // compile error: a const object pass this discard qualifiers
    Point p3(5, 5);
    Point p4(6, 6);
    constexpr auto distanceP2_P1 = getDistance(p1, p2);
    //constexpr auto distanceP4_P3 = getDistance(p3, p4);   // compile error
    auto distanceP4_P3 = getDistance(p3, p4);
    std::cout << distanceP2_P1 << " " << distanceP4_P3;
    constexpr auto p5 = reflection(p1);
    std::cout << p5.getX();
    return 0;
}
```
1. 即只能将常量表达式的结果，赋值给`constexpr`变量
2. 只能传递`constexpr变量`给`constexpr表达式`，才能得到常量结果，否则，只能得到运行时结果
3. `constexpr表达式`内部只能调用`constexpr`接口
4. 注意`setX`与`getX`接口定义的不同点
