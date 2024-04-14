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
