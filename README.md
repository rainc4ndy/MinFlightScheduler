# 无人机最小总飞行距离分配调度系统


(This is the GUI implementation of the algorithm system)


Idea and author: **Blume** [https://blog.yamero.cc/](https://blog.yamero.cc/)


在沿海城市杭市我们最近部署了一套无人机系统，多部无人机沿着一条固定的直线路线来回飞行。
由于当地天气不稳定，我们在这条直线上多个位置部署了无人机的停靠站，每个停靠站用坐标 `x` 和容量 `c` 表示。
给出某时刻所有无人机的坐标，求所有无人机都飞到停靠站停靠的总飞行距离总和是多少?


![illustration](resource/16ba63241543977c4823d85904d826d.png)


## 解析


可以建模为一个**单维度有容量约束的最近停靠分配问题**
即在一条数轴上（沿固定直线路线），将所有无人机分配到合适的停靠站，使得总飞行距离最小，且满足停靠站的容量约束。


### 输入：


- $D = {d_1, d_2, ..., d_m}$：$m$ 个无人机的位置。
- $P = {p_1, p_2, ..., p_n}$：$n$ 个停靠点的坐标及其容量限制 $C(p_i)$。
- $c(d_i, p_j)$：无人机 $d_i$ 飞行至停靠点 $p_j$ 的距离。

### 约束条件：


- 每个无人机必须被分配到一个停靠点，并能够成功降落。
- 每个停靠点的总停靠无人机数量不能超过其容量约束 $C(p_i)$。

### 目标函数：


设 $x_{ij}$ 为决策变量，若无人机 $d_i$ 选择停靠点 $p_j$ 则 $x_{ij} = 1$，否则 $x_{ij} = 0$。
目标是最小化总飞行距离：


$$
\min \sum_{i=1}^{m} \sum_{j=1}^{n} \text{dist}(d_i, p_j) \cdot x_{ij}
$$



## 输入格式


- 第一行给出两个整数 `m`, `n`，分别表示无人机的数量和停靠站的数量
- 第二行给出 `m` 个整数，第 `i` 个整数表示无人机 `i` 的初始位置
- 第三行给出 `n` 个整数，第 `i` 个整数表示停靠站 `i` 的初始位置
- 第四行给出 `n` 个整数，第 `i` 个整数表示停靠站 `i` 的容量

## 输出格式


一个整数，表示将所有无人机分配到合适的停靠站的最小总飞行距离



## 样例1


**输入**


```
4 4
2 6 12 16
0 4 10 14
2 2 2 2
```


**输出**


```
8
```


## 样例2


**输入**


```
4 4
2 6 12 16
0 4 10 14
0 0 0 100
```


**输出**


```
24
```



## 方法一：贪心策略


1. **按无人机位置排序**
2. **按停靠站排序**
3. **逐个分配**：从左到右遍历无人机，每次分配到最近且有剩余容量的停靠站。


缺点：贪心方法并不能保证全局最优，因为前面的分配可能会让后面的选择变差。




## 方法二：动态规划


### 1. 状态定义


定义 $dp[i]$ 表示前 $i$ 个无人机（按顺序）分配完毕的最小总飞行距离。
当当前停靠站（坐标为 $X$，容量为 $c_j$）负责无人机下标区间 $[k, i)$，其分配花费为：


$$
\text{cost}(k, i, X) = \sum_{t=k}^{i-1} |D[t] - X|
$$


转移条件需满足：$i - k \leq c_j$，即 $k \geq \max(0, i - c_j)$
状态转移公式为：


$$
dp[i] = \min_{\max(0, i - c_j) \leq k &lt; i} \left\{ dp[k] + \text{cost}(k, i, X) \right\}
$$



### 2. 前缀和优化


构造前缀和数组 $Q$：


$$
Q[0] = 0, \quad Q[i] = \sum_{t=0}^{i-1} |D[t] - X|
$$


于是：


$$
\text{cost}(k, i, X) = Q[i] - Q[k]
$$


最终转移公式变为：


$$
dp[i] = Q[i] + \min_{\max(0, i - c_j) \leq k &lt; i} \left\{ dp[k] - Q[k] \right\}
$$



### 3. 单调队列优化


为了维护滑动窗口中的最小值：


- 队列元素是下标 $k$，对应值为 $dp[k] - Q[k]$
- 每次计算 $dp[i]$ 时维护一个满足 $k \in [\max(0, i - c_j), i)$ 的单调队列
- 队首即为最优转移点


## 代码实现

&lt;details&gt;
&lt;summary&gt;点击展开 C++ 代码&lt;/summary&gt;

```cpp
#include 
using namespace std;

#define fastio ios_base::sync_with_stdio(false); cin.tie(0); cout.tie(0)
using ll = long long;
using pll = pair;

const ll INF = 1e18;

void solve() {
    ll m, n;
    cin &gt;&gt; m &gt;&gt; n;
    vector D(m);
    vector P(n); // 每个停靠站 (位置，容量)
    
    for (int i = 0; i &lt; m; i++) cin &gt;&gt; D[i];
    for (int j = 0; j &lt; n; j++) cin &gt;&gt; P[j].first;
    for (int j = 0; j &lt; n; j++) cin &gt;&gt; P[j].second;
    
    sort(D.begin(), D.end());
    sort(P.begin(), P.end());
    
    vector dp(m+1, INF);
    dp[0] = 0;
    
    for (int j = 0; j &lt; n; j++) {
        ll X = P[j].first;
        int cap = P[j].second;
        if (cap == 0) continue;
        
        vector Q(m+1, 0);
        for (int i = 1; i &lt;= m; i++)
            Q[i] = Q[i-1] + abs(D[i-1] - X);
        
        deque dq;
        vector newdp(m+1, INF);
        newdp[0] = dp[0];
        
        for (int i = 1; i &lt;= m; i++) {
            while (!dq.empty() && dq.front() &lt; max(0LL, i - cap)) dq.pop_front();
            while (!dq.empty() && dp[dq.back()] - Q[dq.back()] &gt;= dp[i-1] - Q[i-1]) dq.pop_back();
            dq.push_back(i-1);
            newdp[i] = min(newdp[i], Q[i] + dp[dq.front()] - Q[dq.front()]);
        }
        dp = newdp;
    }
    
    cout &lt;&lt; dp[m] &lt;&lt; endl;
}
```

&lt;/details&gt;