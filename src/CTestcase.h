//
// Created by Administrator on 2025/5/25.
//

#ifndef CTESTCASE_H
#define CTESTCASE_H

#include <vector>
using ll = long long;
class CTestcase {
    using ll = long long;

private:
    ll num_drone, num_station;
    std::vector<ll> pDrone, pStation, distance;
    ll result;

public:
    // Setters
    void setNumDrone(ll val) { num_drone = val; }
    void setNumStation(ll val) { num_station = val; }
    void setPDrone(const std::vector<ll>& vec) { pDrone = vec; }
    void setPStation(const std::vector<ll>& vec) { pStation = vec; }
    void setDistance(const std::vector<ll>& vec) { distance = vec; }
    void setResult(ll val) { result = val; }

    // Getters
    ll getNumDrone() const { return num_drone; }
    ll getNumStation() const { return num_station; }
    const std::vector<ll>& getPDrone() const { return pDrone; }
    const std::vector<ll>& getPStation() const { return pStation; }
    const std::vector<ll>& getDistance() const { return distance; }
    ll getResult() const { return result; }
};

#endif //CTESTCASE_H
