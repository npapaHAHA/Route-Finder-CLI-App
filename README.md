# Lab Work 6: Route Finder CLI App (Yandex API)

This lab focuses on designing and implementing a C++ command-line application that finds available travel routes between **Saint Petersburg** and your **hometown** using the [Yandex Schedule API](https://yandex.ru/dev/rasp/doc/ru/).

If you are from Saint Petersburg, use **Pskov** as the target city.

---

## 🎯 Objective

Develop a console application that:
- Finds routes **from Saint Petersburg to your hometown** and back
- Supports any type of transport (bus, train, flight, etc.)
- Supports **at most 1 transfer**
- Accepts the **date of travel** as a command-line argument
- **Caches** results (in-memory and on disk) to reduce API requests

---

## 📡 Data Source

The program must use the **Yandex Schedule API**:

- API Docs: [https://yandex.ru/dev/rasp/doc/ru/](https://yandex.ru/dev/rasp/doc/ru/)
- Free tier: 500 requests per day
- You should **minimize unnecessary requests** by caching immutable data (like station codes or routes between common cities)

---

## ⚙️ Technologies and Libraries

You must use:

- [`cpr`](https://github.com/libcpr/cpr) — HTTP client for making requests
- [`nlohmann/json`](https://github.com/nlohmann/json) — For working with JSON responses

Make sure to:
- Handle connection errors gracefully
- Validate responses before using them
- Implement fallback mechanisms if API quota is exceeded

---

## 🧱 Application Structure

The app should include the following components:

- **HTTP Module** — to send GET requests via `cpr`
- **JSON Parser** — to extract route information from Yandex API responses
- **Cache System** — both memory-based and file-based (JSON or binary)
- **Route Planner** — to determine direct or single-transfer paths
- **CLI Handler** — for parsing arguments (e.g. travel date)

---

## 🧾 Command-Line Interface

Basic usage:

```bash
./route_finder 2025-03-01
```
- Input: date (format: YYYY-MM-DD)
- Output: list of available routes from Saint Petersburg to your hometown (and back)
---

## 💡 Recommendations
- Use smart pointers for memory management
- Use STL containers (`map`, `vector`, `unordered_map`) for caching
- Use standard algorithms (`std::find_if`, `std::sort`) for data processing
- Design the code to be modular and testable

---

## 🧠 Theoretical Topics Involved
- Linkage and Storage Duration

- Smart Pointers

- STL Containers

- Standard Algorithms

---

## 🛠 Example Output (Simplified)

```bash
Date: 2025-03-01
From: Saint Petersburg
To: Kazan

Available Routes:
1. Train SPB → Moscow, Bus Moscow → Kazan
2. Direct Flight SPB → Kazan
...
```
---

## ⚠️ Notes
- Your application should not crash on invalid input or API errors

- Use `.env` or config files for storing API keys (not hardcoded!)

- Implement proper error handling and fallback behavior

- If the result is already cached — use it instead of sending another request
---

## 👨‍💻 Author
1st-year CS student @ ITMO University
GitHub: [npapaHAHA](https://github.com/npapaHAHA)





























