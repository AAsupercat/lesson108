# 用于总结Hot100中的思路和模版

```cpp

```

## 哈希

### 两数之和

```cpp
class Solution {
public:
    vector<int> twoSum(vector<int>& nums, int target) {
        unordered_map<int, int> hash;   //存放[值，下标]
        for (int i = 0; i < nums.size(); ++i) 
        {
            if (hash.count(target - nums[i])) //查找是否有向对应的值
                return {i, hash[target - nums[i]]};
            else
                hash[nums[i]] = i;
        }
        return {};
    }
};
```

### 字母异位词分组

```cpp
class Solution {
public:
    vector<vector<string>> groupAnagrams(vector<string>& strs) 
    {
        vector<vector<string>> ret;
        unordered_map<string,vector<string>> hash;
        for(auto& str : strs)
        {
            string tmp = str;
            sort(tmp.begin(),tmp.end());
            hash[tmp].push_back(str);
        }
        
        for(auto [key,value]:hash)
        {
            ret.emplace_back(value);
        }

        return ret;
    }
};
```

### 最长连续序列

```cpp
class Solution {
public:
    int longestConsecutive(vector<int>& nums) 
    {
        unordered_set<int> hash;    //去重
        int Maxlen = 0;
        for (auto num : nums)
            hash.insert(num);
        for (auto num : hash) 
        {
            if (!hash.count(num - 1)) //找最小，如果不是连续序列最小值跳过
            {
                int count = 1;
                while (hash.count(num + 1)) 
                {
                    count++;
                    num++;
                }
                Maxlen = max(count, Maxlen);
            }
        }
        return Maxlen;
    }
};
```

## 双指针

### 移动零
```cpp
class Solution {
public:
    void moveZeroes(vector<int>& nums) {
        int dest=-1,cur=0;
        int n=nums.size();
        while(cur<n)
        {
            //[0,dest]--非零,[dest,cur]--零,[cur,n-1]--没维护
            if(nums[cur])
                swap(nums[++dest],nums[cur]);
            cur++;
        }
        return ;
    }
};
```

### 盛水最多的容器

```cpp
class Solution {
public:
    int maxArea(vector<int>& height) {
        int MaxArea=0;
        int left=0,right=height.size()-1;
        while(left<right)
        {
            //Area = h * w ; 其中w一直减小，h必须变大才有机会
            int h=min(height[left],height[right]);
            int w=right-left;
            MaxArea=max(MaxArea,h*w);
            height[left]<height[right]?left++:right--;
        }
        return MaxArea;
    }
};
```

### 三数之和

```cpp
//答案中不可以包含重复三元组，三个去重
class Solution {
public:
    vector<vector<int>> threeSum(vector<int>& nums) 
    {
        vector<vector<int>> ret;
        int n=nums.size();
        //优先排序,这里返回的是值
        sort(nums.begin(),nums.end());
        for(int i=0;i<n-2;i++)
        {
            int target=-nums[i];
            int left=i+1,right=n-1;
            while(left<right)
            {
                if(nums[left]+nums[right]>target)
                    right--;
                else if(nums[left]+nums[right]<target)
                    left++;
                else
                {
                    //去重
                    ret.push_back({nums[i],nums[left],nums[right]});
                    //循环left最大n-2,left+1不越界
                    while(left<right&&nums[left]==nums[left+1]) left++;
                    while(left<right&&nums[right]==nums[right-1]) right--;
                    left++,right--;
                }
            }
            while(i<n-2&&nums[i]==nums[i+1]) i++;
        }
        return ret;
    }
};
```

### 接雨水
重点是，计算每个位置能储存多少水。
由两个前缀最大值数组和后缀最大数组优化而来，每次统计当前位置(最大高度小的那边)，计算能储存多少，累加起来；
```cpp
class Solution {
public:
    int trap(vector<int>& height) 
    {
        int left_max = 0, right_max = 0;
        int left = 0, right = height.size() - 1;
        int water = 0;
        //计算每个位置根据left_max和right_max形成的桶，该位置能接多少雨水
        while(left<right)
        {
            left_max=max(left_max,height[left]);
            right_max=max(right_max,height[right]);
            if(left_max<right_max)
                water+=left_max-height[left++];
            else
                water+=right_max-height[right--];
        }
        return water;   
    }
};
```

## 滑动窗口

### 无重复字符的最长子串

```cpp
class Solution {
public:
    int lengthOfLongestSubstring(string s) 
    {
        int hash[128];
        int left=0,right=0,MaxLen=0;
        while(right<s.size())
        {
            hash[s[right]]++;
            while(hash[s[right]]>1)
            {
                hash[s[left]]--;
                left++;
            }
            MaxLen=max(MaxLen,right-left+1);
            right++;
        }
        return MaxLen;
    }
};
```

### 找到字符串中所有字母异位词
还有优化，比较复杂，后续补
```cpp
class Solution {
public:
    vector<int> findAnagrams(string s, string p) 
    {
        int m = s.size(), n = p.size();
        vector<int> ret;
        int hash1[26] = {0};
        for (char& ch : p)
            hash1[ch - 'a']++; // 仅仅包含小写字母

        int hash2[26] = {0};
        int left = 0, right = 0, count = 0;
        while (right < m) {
            int in = s[right] - 'a';
            if (hash2[in] < hash1[in])  count++; // 更新有效值
            hash2[in]++;
            while (right - left + 1 > n) 
            {
                int out = s[left] - 'a';
                if (hash2[out] <= hash1[out])   count--;
                hash2[out]--;
                left++;
            }
            if (count == n) ret.push_back(left);
            right++;
        }
        return ret;
    }
};
```

## 子串

### 和为k的子数组

```cpp
class Solution {
public:
    int subarraySum(vector<int>& nums, int k) 
    {
        unordered_map<int,int> hash;
        hash[0]=1;  //初始化，表示整个数组的和满足要求
        int ret=0,sum=0;
        for(int num:nums)
        {
            sum+=num;
            //sum-k 就是找前缀和为sum-k的个数
            if(hash.count(sum-k)) ret+=hash[sum-k];
            hash[sum]++;
        }
        return ret;
    }
};
```

### 滑动窗口最大值

单调双端队列：O(n),O(k)
```cpp
class Solution {
public:
    vector<int> maxSlidingWindow(vector<int>& nums, int k) 
    {
        //采用单调双端容器
        deque<int> dq;
        vector<int> ret;

        for(int i=0;i<nums.size();i++)
        {
            //尾巴部分比当前部分小，表示不可能最大
            while(!dq.empty()&&nums[dq.back()]<nums[i]) 
                dq.pop_back();
            dq.push_back(i);

            int left=i-k+1; //左边界的下标
            while(dq.front()<left) 
                dq.pop_front();

            if(left>=0) 
                ret.push_back(nums[dq.front()]);
        }   
        return ret;
    }
};
```
优先级队列：O(nlogn),O(n)
```cpp
class Solution {
public:
    vector<int> maxSlidingWindow(vector<int>& nums, int k) {
        int n = nums.size();
        // 存储值和下标
        priority_queue<pair<int, int>> heap;
        vector<int> ret;
        // 初始化大根堆，默认大根堆,k个
        for (int i = 0; heap.size() < k; i++)
            heap.emplace(nums[i], i);
        ret.push_back(heap.top().first);

        for (int i = k; i < nums.size(); i++) {
            heap.emplace(nums[i], i);
            // 只要top是在[i-k+1,i]，不需要维护堆为k大小
            // top是在i-k+1左边区域，表示已经出滑动窗口的区域，就是无效值
            while (heap.top().second < i - k + 1)
            // while (heap.top().second + k <= i)
                heap.pop();
            ret.push_back(heap.top().first);
        }
        return ret;
    }
};
```
### 最小覆盖子串

```cpp
class Solution {
public:
    string minWindow(string s, string t) {
        int hash1[58]={0},hash2[58]={0};
        int n=s.size(),len=t.size();
        int size = INT_MAX,index = 0;
        
        //t中数据导入hash2
        for(auto& ch:t) hash2[ch-'A']++;

        for(int left=0,right=0,count=0;right<n;right++)
        {
            //进窗口+维护count
            int in = s[right] - 'A';
            hash1[in]++;
            if(hash1[in]<=hash2[in]) count++;
            //判断：
            while(count==len)
            {
                //更新结果 长度+开始下标
                if(size>right-left+1)
                {
                    size=right-left+1;
                    index=left;
                }
                //出窗口+维护count
                int out = s[left] - 'A';
                if(hash1[out]<=hash2[out]) count--;
                hash1[out]--; left++;
            }
        }
        if(size==INT_MAX) return "";
        else return s.substr(index,size);
    }
};
```

