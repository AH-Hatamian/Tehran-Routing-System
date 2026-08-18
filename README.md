
# Traffic Routing System (C++)

A console-based traffic routing simulation for a city road network, built from scratch in C++ using core data structures — no external libraries.

Roads are modeled as a **weighted graph**, and the shortest path between two locations is computed using a **Dijkstra-style algorithm** implemented from scratch. Per-route traffic conditions (Light/Moderate/Heavy) are stored in a **Binary Search Tree** for fast lookup, and incoming route requests are processed with a custom **Queue** (backed by `std::deque`). Trip history is persisted to disk.

## Features

- Load road network + traffic data from file, or add routes live via interactive mode
- Compute shortest path and total distance between any two locations
- View per-segment traffic conditions along the computed route
- Process a queue of route requests in order, logging results to `history.txt`
- Handles missing files, malformed input, and UTF-8 BOM gracefully

## How to Run

```bash
g++ -std=c++17 -O2 main.cpp -o traffic_system
./traffic_system
```

`routes.txt` and `requests.txt` must be in the same folder (see format below). `history.txt` is created automatically.

**routes.txt** — `From:Valiasr, To:Tajrish, Distance:12, Traffic:Heavy`
**requests.txt** — `RequestID:1, UserID:101, From:Valiasr, To:Niavaran, RequestTime:08:15`

## Sample Output

```
Enter Starting City: Valiasr
Enter Destination City: Niavaran

==== Custom Route ====
Path: Valiasr -> Tajrish -> Niavaran
Total Distance: 17

--- Traffic Conditions ---
* Valiasr to Tajrish : Heavy
* Tajrish to Niavaran : Light
```

## Design Notes

- BST memory is fully released via a recursive destructor — no leaks.
- Queue uses `std::deque` instead of `std::vector` so dequeuing is O(1), not O(n).
- File parsing is wrapped in `try/catch` so malformed lines are skipped instead of crashing.
