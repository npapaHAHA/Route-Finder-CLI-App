#ifndef FLIGHTSEARCH_H
#define FLIGHTSEARCH_H

#include <string>
#include <vector>
#include <iostream>
#include <nlohmann/json.hpp>
#include <cpr/cpr.h>
#include <chrono>
#include <iomanip>
#include <ctime>
#include <fstream>

class Route {
public:
    std::string departure_time;
    std::string arrival_time;
    std::vector<std::string> transport_details;
    bool has_transfers;
    std::string from_city;
    std::string transfer_city;
    std::string to_city;

    Route(const std::string &departure_time, const std::string &arrival_time,
          const std::vector<std::string> &transport_details,
          bool has_transfers, const std::string &from_city, const std::string &transfer_city,
          const std::string &to_city)
        : departure_time(departure_time), arrival_time(arrival_time), transport_details(transport_details),
          has_transfers(has_transfers), from_city(from_city), transfer_city(transfer_city), to_city(to_city) {
    }
};

class FlightSearch {
public:
    explicit FlightSearch(const std::string &date);

    bool FetchRoutes(const std::string &from, const std::string &to);

    void FindRoundTripRoutes(const std::string &from, const std::string &to) const;

    void DisplayFilteredRoute(const Route &route) const;

    void ClearCache();

    void ClearFileCache(const std::string &cacheFilePath);

    void ClearAllCache(const std::string &cacheFilePath);

    bool LoadCacheFromFile(const std::string &cacheFilePath);

    void SaveCacheToFile(const std::string &filePath);

    bool IsInternetConnected();

private:
    std::string date;
    std::vector<Route> routes;

    Route ParseRoute(const nlohmann::json &segment, const nlohmann::json &details, bool has_transfers);

    bool IsCompatible(const Route &routeTo, const Route &routeFrom) const;

    std::vector<Route> FilterRoutes() const;
};

#endif
