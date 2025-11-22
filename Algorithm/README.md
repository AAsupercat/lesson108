# 算法学习
## 基础算法
### 双指针

### 滑动窗口

### 二分查找

### 前缀和

### 模拟

### 哈希表

### 字符串

### 栈

### 分治


#### 快速排序（非递归版本）

使用栈来模拟递归过程，将需要排序的区间[l, r]压入栈中，循环处理直到栈为空。

```cpp
class Solution {
public:
    // 分区函数：将数组分为两部分，左边小于基准值，右边大于等于基准值
    int partition(vector<int>& nums, int left, int right) {
        // 选择最右边的元素作为基准值
        int pivot = nums[right];
        int i = left - 1; // i指向小于基准值的最后一个元素的位置
        
        for (int j = left; j < right; j++) {
            // 如果当前元素小于基准值，将其交换到左边区域，[0,1]维护的是小于基准值的数据
            if (nums[j] < pivot) {
                i++;
                swap(nums[i], nums[j]);
            }
        }
        // 将基准值放到正确位置
        swap(nums[i + 1], nums[right]);
        return i + 1; // 返回基准值的位置
    }
    
    void quickSort(vector<int>& nums) {
        int n = nums.size();
        if (n <= 1) return;
        
        stack<pair<int, int>> stk; // 存储待排序区间[l, r]
        stk.push({0, n - 1}); // 初始区间：整个数组
        
        while (!stk.empty()) {
            auto [left, right] = stk.top();
            stk.pop();
            
            if (left < right) {
                // 分区操作
                int pivot_idx = partition(nums, left, right);
                
                // 将左右两个子区间压入栈中
                // 先处理右区间，再处理左区间（栈的特性，后进先出）
                stk.push({pivot_idx + 1, right}); // 右区间
                stk.push({left, pivot_idx - 1});  // 左区间
            }
        }
    }
};
```



### 队列 + 宽搜(BFS)

### 优先级队列

### BFS解决FloodFill问题

### BFS解决最短路径问题

### 多源BFS

### BFS解决拓扑排序

---

## 递归回溯与搜索
### 递归算法

### 搜索算法

### 回溯与剪枝

### FloodFill算法

### 记忆化搜索

---

## 动态规划

### 斐波那契数列模型

### 路径问题

### 简单多状态dp问题

### 子数组问题

### 子序列问题

### 回文串问题

### 两个数组的dp问题

### 01背包问题

### 完全背包问题

### 二维费用背包问题

### 似包非包

### 卡特兰数

---

## 贪心算法