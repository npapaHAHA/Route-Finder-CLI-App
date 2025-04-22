#include "FlightSearch.h"

FlightSearch::FlightSearch(const std::string &date) : date(date) {
}

Route FlightSearch::ParseRoute(const nlohmann::json &segment, const nlohmann::json &details, bool has_transfers) {
    std::string departure_time = segment.contains("departure") ? segment["departure"].get<std::string>() : "N/A";
    std::string arrival_time = segment.contains("arrival") ? segment["arrival"].get<std::string>() : "N/A";

    std::string from_city = "Unknown";
    if (segment.contains("from")) {
        if (segment["from"].contains("title") && !segment["from"]["title"].empty()) {
            from_city = segment["from"]["title"].get<std::string>();
        } else if (segment["from"].contains("short_title") && !segment["from"]["short_title"].empty()) {
            from_city = segment["from"]["short_title"].get<std::string>();
        } else if (segment["from"].contains("code")) {
            from_city = "Station Code: " + segment["from"]["code"].get<std::string>();
        }
    }

    std::string to_city = "Unknown";
    if (segment.contains("to")) {
        if (segment["to"].contains("title") && !segment["to"]["title"].empty()) {
            to_city = segment["to"]["title"].get<std::string>();
        } else if (segment["to"].contains("short_title") && !segment["to"]["short_title"].empty()) {
            to_city = segment["to"]["short_title"].get<std::string>();
        } else if (segment["to"].contains("code")) {
            to_city = "Station Code: " + segment["to"]["code"].get<std::string>();
        }
    }

    std::vector<std::string> transport_details;
    if (segment.contains("thread") && segment["thread"].contains("number") && segment["thread"].contains(
            "transport_type")) {
        std::string transport_info = segment["thread"]["number"].get<std::string>();
        std::string transport_type = segment["thread"]["transport_type"].get<std::string>();
        transport_details.push_back(transport_type + " " + transport_info);
    }

    std::string transfer_city = "N/A";
    if (details.is_array()) {
        for (const auto &detail: details) {
            if (detail.contains("thread") && detail["thread"].contains("number") && detail["thread"].contains(
                    "transport_type")) {
                std::string detail_transport_info = detail["thread"]["number"].get<std::string>();
                std::string detail_transport_type = detail["thread"]["transport_type"].get<std::string>();
                transport_details.push_back(detail_transport_type + " " + detail_transport_info);
            }

            if (detail.contains("is_transfer") && detail["is_transfer"]) {
                if (detail.contains("transfer_point") && detail["transfer_point"].contains("title") && !detail[
                        "transfer_point"]["title"].empty()) {
                    transfer_city = detail["transfer_point"]["title"].get<std::string>();
                } else if (detail.contains("transfer_point") && detail["transfer_point"].contains("code")) {
                    transfer_city = "Transfer Code: " + detail["transfer_point"]["code"].get<std::string>();
                }
            }
        }
    }

    return Route(departure_time, arrival_time, transport_details, has_transfers, from_city, transfer_city, to_city);
}

bool FlightSearch::FetchRoutes(const std::string &from, const std::string &to) {
    std::string url = "https://api.rasp.yandex.net/v3.0/search/?from=" + from + "&to=" + to +
                      "&format=json&lang=ru_RU&apikey=YOUR_API_KEY&date=" + date +
                      "&transfers=true";
    cpr::Response response = cpr::Get(cpr::Url{url});

    if (response.status_code == 200) {
        try {
            nlohmann::json parsedJson = nlohmann::json::parse(response.text);
            if (parsedJson.contains("segments") && parsedJson["segments"].is_array()) {
                for (const auto &segment: parsedJson["segments"]) {
                    bool has_transfers = segment.contains("has_transfers")
                                             ? segment["has_transfers"].get<bool>()
                                             : false;
                    if (segment.contains("details") && segment["details"].is_array()) {
                        routes.push_back(ParseRoute(segment, segment["details"], has_transfers));
                    } else {
                        routes.push_back(ParseRoute(segment, nlohmann::json::array(), has_transfers));
                    }
                }
            }
            return true;
        } catch (const nlohmann::json::exception &e) {
            std::cerr << "JSON parsing error: " << e.what() << "\n";
            return false;
        }
    } else {
        std::cerr << "Error: " << response.status_code << " - " << response.error.message << "\n";
        return false;
    }
}

std::vector<Route> FlightSearch::FilterRoutes() const {
    std::vector<Route> filteredRoutes;

    for (const auto &route: routes) {
        if (!route.has_transfers || (route.has_transfers && !route.transport_details.empty())) {
            filteredRoutes.push_back(route);
        }
    }

    return filteredRoutes;
}

bool FlightSearch::IsCompatible(const Route &routeTo, const Route &routeFrom) const {
    auto parseTime = [](const std::string &timeStr) -> std::chrono::system_clock::time_point {
        std::tm tm = {};
        strptime(timeStr.c_str(), "%Y-%m-%dT%H:%M:%S%z", &tm);
        return std::chrono::system_clock::from_time_t(std::mktime(&tm));
    };

    auto arrivalTimeTo = parseTime(routeTo.arrival_time);
    auto departureTimeFrom = parseTime(routeFrom.departure_time);

    return departureTimeFrom >= arrivalTimeTo;
}

void FlightSearch::FindRoundTripRoutes(const std::string &from, const std::string &to) const {
    FlightSearch searchTo(date);
    FlightSearch searchFrom(date);

    std::cout << "\nSearching routes from " << from << " to " << to << " on " << date << "...\n";
    if (!searchTo.FetchRoutes(from, to)) {
        std::cerr << "No routes found from " << from << " to " << to << ".\n";
        return;
    }

    auto filteredRoutesTo = searchTo.FilterRoutes();

    std::cout << "\nSearching routes from " << to << " to " << from << " on " << date << "...\n";
    if (!searchFrom.FetchRoutes(to, from)) {
        std::cerr << "No routes found from " << to << " to " << from << ".\n";
        return;
    }

    auto filteredRoutesFrom = searchFrom.FilterRoutes();


    int maxCombinations = 999;
    int combinationCount = 0;

    for (const auto &routeTo: filteredRoutesTo) {
        for (const auto &routeFrom: filteredRoutesFrom) {
            if (!IsCompatible(routeTo, routeFrom)) continue;

            if (combinationCount >= maxCombinations) break;

            std::cout << "\nRoute To:\n";
            DisplayFilteredRoute(routeTo);
            std::cout << "\nRoute Back:\n";
            DisplayFilteredRoute(routeFrom);
            std::cout << "-------------------------\n";

            combinationCount++;
        }
        if (combinationCount >= maxCombinations) break;
    }

    if (combinationCount == 0) {
        std::cout << "No  round-trip routes found.\n";
    }
}

void FlightSearch::DisplayFilteredRoute(const Route &route) const {
    std::cout << "Departure: " << route.departure_time << " (" << route.from_city << ")\n";
    std::cout << "Arrival: " << route.arrival_time << " (" << route.to_city << ")\n";
    std::cout << "Transports:\n";
    for (const auto &transport: route.transport_details) {
        std::cout << "  - " << transport << "\n";
    }

    if (route.has_transfers) {
        std::cout << "Transfer in: " << route.transfer_city << "\n";
    }

    if (route.has_transfers) {
        std::cout << "This route has transfers.\n";
    } else {
        std::cout << "This route has no transfers.\n";
    }
}

bool FlightSearch::IsInternetConnected() {
    try {
        cpr::Response response = cpr::Get(cpr::Url{"https://www.github.com"});

        if (response.status_code == 200) {
            return true;
        } else {
            std::cerr << "Error: Unable to connect to the internet (HTTP Status: " << response.status_code << ")\n";
            return false;
        }
    } catch (const std::exception &e) {
        std::cerr << "Exception occurred while checking internet connection: " << e.what() << "\n";
        return false;
    }
}

void FlightSearch::ClearCache() {
    routes.clear();
}

void FlightSearch::ClearFileCache(const std::string &cacheFilePath) {
    if (std::remove(cacheFilePath.c_str()) != 0) {
        std::cerr << "Error deleting the cache file: " << cacheFilePath << "\n";
    } else {
        std::cout << "Cache file deleted successfully: " << cacheFilePath << "\n";
    }
}

void FlightSearch::ClearAllCache(const std::string &cacheFilePath) {
    ClearCache();
    ClearFileCache(cacheFilePath);
}

bool FlightSearch::LoadCacheFromFile(const std::string &cacheFilePath) {
    std::ifstream file(cacheFilePath);
    if (file.is_open()) {
        nlohmann::json j;
        file >> j;


        for (const auto &route: j["routes"]) {
            routes.push_back(ParseRoute(route["segment"], route["details"], route["has_transfers"]));
        }
        return true;
    }
    return false;
}

void FlightSearch::SaveCacheToFile(const std::string &filePath) {
    std::ofstream file(filePath);
    if (file.is_open()) {
        nlohmann::json j;

        for (const auto &route: routes) {
            nlohmann::json routeJson;

            j["routes"].push_back(routeJson);
        }
        file << j.dump();
    }
}

//set(CMAKE_EXE_LINKER_FLAGS "-static")