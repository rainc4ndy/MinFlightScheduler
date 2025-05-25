//
// Created by Administrator on 2025/5/25.
//

#ifndef CALGORITHM_H
#define CALGORITHM_H
#include <future>

#include "CTestcase.h"

using ll = long long;
using Result = std::pair<ll, int>;

class CAlgorithm {
public:
    std::future<Result> submitTask(const std::vector<ll>* _D,
                                const std::vector<ll>* _P,
                                const std::vector<ll>* _cap);
    Result run(std::vector<ll> D,
    std::vector<ll> P,
    std::vector<ll> cap);
};


#endif //CALGORITHM_H
