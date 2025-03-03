# Data structure and algorithm

## Skip list
跳表，是在list的基础上，建立了K层索引，如：每两个节点，抽取一个节点，即K = logN。通过索引层，能够在O(logN)的时间复杂度上，实现快速的查找，插入与删除。
- K层索引的维护
  如：通过随机函数，把新插入的元素，作为K层索引节点。
适用场景：
1. 与RB tree类似，适用于频繁查找，插入，删除的业务，同时，还支持区间查找。跳表较RB tree实现上更简单
详见：https://time.geekbang.org/column/article/42896

## Binary search
二分查找适用于大数据量的有序数组（支持随机访问），同时，该数组应该是静态的，即没有频繁的插入与删除。
二分查找的复杂度为O(logN)。
详见：https://time.geekbang.org/column/article/42520

## sort
- insertion sort
  在未排序的区域，从一个元素开始，作为对象插入到已排序的区域，直到未排序区域元素个数为0.
  时间复杂度O（n平方），空间复杂度O（1），是一种稳定排序
- selection sort
  在未排序区域，选择最小值，将它放到已排序区域，直到未排序区域元素个数为0.
  时间复杂度O（n平方），空间复杂度O（1），是一种不稳定排序
- merge sort
  依次将数组按2等分进行分割，直到每个set只有一个元素，然后，逆向进行merge到一个数组。
  算法思想，大数组排序，可以分成多个子数组排序，然后，将排序好的子数组，进行合并
  时间复杂度O（nlog(n)），空间复杂度O（1），是一种稳定排序
- quick sort
  时间复杂度O（nlog(n)），空间复杂度O（1），是一种不稳定排序

## Dynamic programming
Dynamic programming，通常用来找到问题的最优解，如：最短路径，0-1背包问题等。
适合`Dynamic programming`解决的问题，包含的特征：
- 一个模型：`多阶段决策最优解模型`
问题的解决，包含多个阶段的决策。每个决策阶段，对应一组决策状态，通过选择一组决策状态序列，找到问题的最优解。
- 三个特征：
  1. 最优子问题
   问题的最优解，可以从子问题的最优解中推导。即后面的状态，可以通过前面的状态来推导。
  2. 无后性
   当前的状态，只与上一个状态有关，不需要关心怎么到上一个状态。某个阶段的状态确定后，不受其后状态的影响。
  3. 重复子问题
   即在递归树中，有重复的子节点。

两种实现方法：
1. 状态转移表法
   步骤：定义状态-》画递归树-》建状态转移表-》填状态转移表-》代码实现填表过程。
   从0开始，求解n。
2. 状态转移方程法
   步骤：找最优子结构-》找到状态转移方程-》代码实现代码转移方程。
   从n开始递推，通过设`备忘录`来求解。
针对不同的问题，选择不同的方法求解。
- reference
https://time.geekbang.org/column/article/75702