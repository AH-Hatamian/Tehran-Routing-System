#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <deque>
#include <algorithm>
#include <utility>

using namespace std;

string trim(const string& str) {
    int start = 0;
    while (start < (int)str.size() && (str[start] == ' ' || str[start] == '\t' || str[start] == '\n' || str[start] == '\r')) start++;
    int end = str.size() - 1;
    while (end >= 0 && (str[end] == ' ' || str[end] == '\t' || str[end] == '\n' || str[end] == '\r')) end--;
    if (start > end) return "";
    return str.substr(start, end - start + 1);
}

vector<string> splitString(const string& str, char delimiter) {
    vector<string> tokens;
    string token = "";
    for (char c : str) {
        if (c == delimiter) {
            tokens.push_back(token);
            token = "";
        } else {
            token += c;
        }
    }
    if (!token.empty()) tokens.push_back(token);
    return tokens;
}

int trafficStringToLevel(const string& traffic) {
    if (traffic == "Light") return 1;
    if (traffic == "Moderate") return 2;
    if (traffic == "Heavy") return 3;
    return 0;
}

class TrafficData {
public:
    string routeName;
    int trafficLevel;

    TrafficData(const string& name, int level) {
        routeName = name;
        trafficLevel = level;
    }

    bool operator<(const TrafficData& other) const {
        return routeName < other.routeName;
    }

    bool operator>(const TrafficData& other) const {
        return routeName > other.routeName;
    }

    bool operator==(const TrafficData& other) const {
        return routeName == other.routeName;
    }
};

class TrafficNode {
public:
    TrafficData data;
    TrafficNode* left;
    TrafficNode* right;

    TrafficNode(const TrafficData& d) : data(d) {
        left = 0;
        right = 0;
    }
};

class TrafficBST {
private:
    TrafficNode* root;

    TrafficNode* insert(TrafficNode* node, const TrafficData& data) {
        if (node == 0) return new TrafficNode(data);
        if (data < node->data) node->left = insert(node->left, data);
        else if (data > node->data) node->right = insert(node->right, data);
        return node;
    }

    TrafficNode* search(TrafficNode* node, const string& route) {
        if (node == 0) return 0;
        if (route == node->data.routeName) return node;
        if (route < node->data.routeName) return search(node->left, route);
        return search(node->right, route);
    }

    void inorder(TrafficNode* node, vector<TrafficData>& result) {
        if (node == 0) return;
        inorder(node->left, result);
        result.push_back(node->data);
        inorder(node->right, result);
    }

    void destroyTree(TrafficNode* node) {
        if (node != nullptr) {
            destroyTree(node->left);
            destroyTree(node->right);
            delete node;
        }
    }

public:
    TrafficBST() {
        root = 0;
    }

    ~TrafficBST() {
        destroyTree(root);
    }

    void clear() {
        destroyTree(root);
        root = 0;
    }

    void insert(const string& route, int level) {
        root = insert(root, TrafficData(route, level));
    }

    int getTrafficLevel(const string& route) {
        TrafficNode* node = search(root, route);
        if (node != 0) return node->data.trafficLevel;
        return -1;
    }

    vector<TrafficData> getAllTrafficData() {
        vector<TrafficData> result;
        inorder(root, result);
        return result;
    }
};

class Edge {
public:
    string destination;
    int weight;

    Edge(const string& dest, int w) {
        destination = dest;
        weight = w;
    }
};

class Graph {
private:
    vector<string> nodeNames;
    vector<vector<Edge>> adjacencyList;

    int getNodeIndex(const string& name) {
        for (int i = 0; i < (int)nodeNames.size(); i++) {
            if (nodeNames[i] == name) return i;
        }
        return -1;
    }

public:
    void clear() {
        nodeNames.clear();
        adjacencyList.clear();
    }

    void addNode(const string& name) {
        if (!containsNode(name)) {
            nodeNames.push_back(name);
            adjacencyList.push_back(vector<Edge>());
        }
    }

    void addEdge(const string& from, const string& to, int weight) {
        addNode(from);
        addNode(to);
        int index = getNodeIndex(from);
        adjacencyList[index].push_back(Edge(to, weight));
    }

    vector<Edge> getNeighbors(const string& node) {
        int index = getNodeIndex(node);
        if (index != -1) return adjacencyList[index];
        return vector<Edge>();
    }

    bool containsNode(const string& name) {
        return getNodeIndex(name) != -1;
    }

    vector<string> getAllNodes() {
        return nodeNames;
    }

    int getNodeIndexPublic(const string& name) {
        return getNodeIndex(name);
    }
};

class RouteFinder {
public:
    static pair<vector<string>, int> findShortestPath(Graph& graph, const string& start, const string& end) {
        vector<string> nodes = graph.getAllNodes();
        int n = (int)nodes.size();

        int INF = 1000000000;

        vector<int> distance(n, INF);
        vector<bool> visited(n, false);
        vector<int> previous(n, -1);

        int startIndex = graph.getNodeIndexPublic(start);
        int endIndex = graph.getNodeIndexPublic(end);

        if (startIndex == -1 || endIndex == -1) return {vector<string>(), -1};

        distance[startIndex] = 0;

        for (int i = 0; i < n; i++) {
            int minDist = INF;
            int current = -1;

            for (int j = 0; j < n; j++) {
                if (!visited[j] && distance[j] < minDist) {
                    minDist = distance[j];
                    current = j;
                }
            }

            if (current == -1) break;
            visited[current] = true;

            vector<Edge> neighbors = graph.getNeighbors(nodes[current]);
            for (int k = 0; k < (int)neighbors.size(); k++) {
                string neighborName = neighbors[k].destination;
                int weight = neighbors[k].weight;
                int neighborIndex = graph.getNodeIndexPublic(neighborName);

                if (distance[current] + weight < distance[neighborIndex]) {
                    distance[neighborIndex] = distance[current] + weight;
                    previous[neighborIndex] = current;
                }
            }
        }

        vector<string> path;
        int current = endIndex;
        while (current != -1) {
            path.push_back(nodes[current]);
            current = previous[current];
        }
        
        reverse(path.begin(), path.end());

        if (path.empty() || path[0] != start) return {vector<string>(), -1};
        return {path, distance[endIndex]};
    }
};

template <class T>
class Queue {
private:
    deque<T> items;
public:
    void clear() {
        items.clear();
    }

    void enqueue(const T& item) {
        items.push_back(item);
    }

    void dequeue() {
        if (!isEmpty()) items.pop_front();
    }

    T front() {
        return items.front();
    }

    bool isEmpty() {
        return items.empty();
    }

    int size() {
        return items.size();
    }
};

class Request {
private:
    int requestId;
    int userId;
    string from;
    string to;
    string requestTime;

public:
    Request() {
        requestId = 0;
        userId = 0;
    }

    Request(int rid, int uid, const string& f, const string& t, const string& time) {
        requestId = rid;
        userId = uid;
        from = f;
        to = t;
        requestTime = time;
    }

    int getRequestId() {
        return requestId;
    }

    int getUserId() {
        return userId;
    }

    string getFrom() {
        return from;
    }

    string getTo() {
        return to;
    }

    string getRequestTime() {
        return requestTime;
    }

    void setRequest(int rid, int uid, const string& f, const string& t, const string& time) {
        requestId = rid;
        userId = uid;
        from = f;
        to = t;
        requestTime = time;
    }
};

class HistoryRecord {
private:
    int userId;
    string from;
    string to;
    int travelTime;
    string date;
    int alt;

public:
    HistoryRecord(int uid, const string& f, const string& t, int time, const string& d, int a) {
        userId = uid;
        from = f;
        to = t;
        travelTime = time;
        date = d;
        alt = a;
    }

    string toString() const {
        return "UserID:" + to_string(userId) + ", From:" + from + ", To:" + to +
               ", TravelTime:" + to_string(travelTime) + ", Date:" + date + ", Alt:" + to_string(alt);
    }
};

bool loadRoutesFromFile(Graph& graph, TrafficBST& trafficBST) {
    graph.clear();
    trafficBST.clear();

    ifstream file("routes.txt");
    if (!file.is_open()) {
        cout << " [!] ERROR: Cannot find 'routes.txt'. Make sure it's in the same folder as the executable.\n";
        return false;
    }

    string line;
    bool firstLine = true;
    while (getline(file, line)) {
        if (firstLine && line.size() >= 3 && line[0] == (char)0xEF && line[1] == (char)0xBB && line[2] == (char)0xBF) {
            line = line.substr(3);
        }
        firstLine = false;

        string fromValue, toValue, distValue, trafficValue;
        vector<string> tokens = splitString(line, ',');

        for (int i = 0; i < (int)tokens.size(); i++) {
            int pos = tokens[i].find(':');
            if (pos == -1) continue;
            string key = trim(tokens[i].substr(0, pos));
            string value = trim(tokens[i].substr(pos + 1));
            
            if (key == "From") fromValue = value;
            else if (key == "To") toValue = value;
            else if (key == "Distance") distValue = value;
            else if (key == "Traffic") trafficValue = value;
        }

        if (fromValue.length() > 0 && toValue.length() > 0 && distValue.length() > 0) {
            try {
                int weight = (int)stof(distValue);
                graph.addEdge(fromValue, toValue, weight);
                trafficBST.insert(fromValue + "-" + toValue, trafficStringToLevel(trafficValue));
            } catch (...) { continue; }
        }
    }
    file.close();
    return true;
}

bool loadRequestsFromFile(Queue<Request>& requestQueue) {
    requestQueue.clear();

    ifstream file("requests.txt");
    if (!file.is_open()) {
         cout << " [!] ERROR: Cannot find 'requests.txt'. Make sure it's in the same folder as the executable.\n";
         return false;
    }

    string line;
    bool firstLine = true;
    while (getline(file, line)) {
        if (firstLine && line.size() >= 3 && line[0] == (char)0xEF && line[1] == (char)0xBB && line[2] == (char)0xBF) {
            line = line.substr(3);
        }
        firstLine = false;

        int requestId = 0, userId = 0;
        string from = "", to = "", time = "";

        vector<string> tokens = splitString(line, ',');

        for (int i = 0; i < (int)tokens.size(); i++) {
            int pos = tokens[i].find(':');
            if (pos == -1) continue;
            string key = trim(tokens[i].substr(0, pos));
            string value = trim(tokens[i].substr(pos + 1));
            try {
                if (key == "RequestID") requestId = stoi(value);
                else if (key == "UserID") userId = stoi(value);
                else if (key == "From") from = value;
                else if (key == "To") to = value;
                else if (key == "RequestTime") time = value;
            } catch (...) { continue; }
        }

        Request req(requestId, userId, from, to, time);
        requestQueue.enqueue(req);
    }
    file.close();
    return true;
}

void loadHistoryFromFile(vector<HistoryRecord>& history) {
    history.clear();

    ifstream file("history.txt");
    if (!file.is_open()) return;

    string line;
    while (getline(file, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back(); 
        }

        int uid = 0, time = 0, alt = 0;
        string from = "", to = "", date = "";

        vector<string> tokens = splitString(line, ',');

        for (int i = 0; i < (int)tokens.size(); i++) {
            int sep = tokens[i].find(':');
            if (sep == -1) continue;
            string key = trim(tokens[i].substr(0, sep));
            string value = trim(tokens[i].substr(sep + 1));

            try {
                if (key == "UserID") uid = stoi(value);
                else if (key == "From") from = value;
                else if (key == "To") to = value;
                else if (key == "TravelTime") time = stoi(value);
                else if (key == "Date") date = value;
                else if (key == "Alt") alt = stoi(value);
            } catch (...) {
                continue;
            }
        }

        history.push_back(HistoryRecord(uid, from, to, time, date, alt));
    }
    file.close();
}

void showGraph(Graph& graph) {
    vector<string> nodes = graph.getAllNodes();
    for (int i = 0; i < (int)nodes.size(); i++) {
        cout << nodes[i] << " -> ";
        vector<Edge> neighbors = graph.getNeighbors(nodes[i]);
        for (int j = 0; j < (int)neighbors.size(); j++) {
            cout << neighbors[j].destination << "(" << neighbors[j].weight << ")";
            if (j != (int)neighbors.size() - 1) cout << ", ";
        }
        cout << "\n";
    }
}

void showRequests(Queue<Request>& queue) {
    if (queue.isEmpty()) {
        cout << "No requests available.\n";
        return;
    }
    Queue<Request> temp = queue;
    while (!temp.isEmpty()) {
        Request req = temp.front();
        cout << "RequestID: " << req.getRequestId() << ", UserID: " << req.getUserId() << ", From: " << req.getFrom() << ", To: " << req.getTo() << ", Time: " << req.getRequestTime() << "\n";
        temp.dequeue();
    }
}

void showHistory(const vector<HistoryRecord>& history) {
    if (history.empty()) {
        cout << "No history available.\n";
        return;
    }
    for (int i = 0; i < (int)history.size(); i++) {
        cout << history[i].toString() << "\n";
    }
}

void processNextRequest(Graph& graph, Queue<Request>& requestQueue, vector<HistoryRecord>& history) {
    if (requestQueue.isEmpty()) {
        cout << "No requests to process.\n";
        return;
    }
    
    Request req = requestQueue.front();
    requestQueue.dequeue();

    int altFlag = 0;
    pair<vector<string>, int> result = RouteFinder::findShortestPath(graph, req.getFrom(), req.getTo());
    vector<string> path = result.first;
    int travelTime = result.second;

    if (path.empty()) {
        cout << "No path found from " << req.getFrom() << " to " << req.getTo() << ".\n";
    } else {
        cout << "User " << req.getUserId() << "'s route from " << req.getFrom() << " to " << req.getTo() << ":\n";
        for (int i = 0; i < (int)path.size(); i++) {
            cout << path[i];
            if (i != (int)path.size() - 1) cout << " -> ";
        }
        cout << "\nEstimated travel distance: " << travelTime << "\n";

        history.push_back(HistoryRecord(req.getUserId(), req.getFrom(), req.getTo(), travelTime, req.getRequestTime(), altFlag));

        ofstream histFile("history.txt", ios::app);
        if (histFile.is_open()) {
            histFile << history.back().toString() << "\n";
            histFile.close();
        }
    }
}

void interactiveRouting(Graph& graph, TrafficBST& traffic, vector<HistoryRecord>& history) {
    string userFrom, userTo;
    cout << "Enter Starting City: ";
    cin >> userFrom;
    cout << "Enter Destination City: ";
    cin >> userTo;

    if (!graph.containsNode(userFrom) || !graph.containsNode(userTo)) {
        cout << "One or both cities not found in the network. Creating a new direct route...\n";
        int dist;
        string trafficLevelStr;
        cout << "Enter distance for this new route: ";
        cin >> dist;
        cout << "Enter traffic level (Light/Moderate/Heavy): ";
        cin >> trafficLevelStr;

        graph.addEdge(userFrom, userTo, dist);
        traffic.insert(userFrom + "-" + userTo, trafficStringToLevel(trafficLevelStr));

        ofstream outFile("routes.txt", ios::app);
        if (outFile.is_open()) {
            outFile << "From:" << userFrom << ", To:" << userTo << ", Distance:" << dist << ", Traffic:" << trafficLevelStr << "\n";
            outFile.close();
        }
        cout << "New route added and saved to routes.txt\n";
    }

    pair<vector<string>, int> res = RouteFinder::findShortestPath(graph, userFrom, userTo);
    vector<string> p = res.first;
    int dist = res.second;

    if (p.empty()) {
        cout << "No path found from " << userFrom << " to " << userTo << ".\n";
    } else {
        cout << "\n==== Custom Route ====\n";
        cout << "Path: ";
        for (int i = 0; i < (int)p.size(); i++) {
            cout << p[i];
            if (i != (int)p.size() - 1) cout << " -> ";
        }
        cout << "\nTotal Distance: " << dist << "\n\n";
        
        cout << "--- Traffic Conditions ---\n";
        for (int i = 0; i < (int)p.size() - 1; i++) {
            string edgeKey = p[i] + "-" + p[i+1];
            int tLevel = traffic.getTrafficLevel(edgeKey);
            string status = "Unknown";
            if (tLevel == 1) status = "Light";
            else if (tLevel == 2) status = "Moderate";
            else if (tLevel == 3) status = "Heavy";
            
            cout << "* " << p[i] << " to " << p[i+1] << " : " << status << "\n";
        }
        cout << "======================\n";

        history.push_back(HistoryRecord(9999, userFrom, userTo, dist, "LiveSearch", 0));
        ofstream histFile("history.txt", ios::app);
        if (histFile.is_open()) {
            histFile << history.back().toString() << "\n";
            histFile.close();
        }
    }
}

int main() {
    Graph graph;
    TrafficBST traffic;
    Queue<Request> requestQueue;
    vector<HistoryRecord> history;

    int choice;

    do {
        cout << "\n--- Traffic & Routing System ---\n";
        cout << "1. Load graph and traffic data\n";
        cout << "2. Load route requests\n";
        cout << "3. Show graph\n";
        cout << "4. Show all requests\n";
        cout << "5. Process next request\n";
        cout << "6. Show user route history\n";
        cout << "7. Interactive Live Routing\n";
        cout << "0. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch(choice) {
            case 1:
                if(loadRoutesFromFile(graph, traffic)) {
                    cout << "Graph and traffic data loaded successfully.\n";
                }
                break;

            case 2:
                if(loadRequestsFromFile(requestQueue)) {
                    cout << "Requests loaded successfully.\n";
                }
                break;

            case 3:
                showGraph(graph);
                break;

            case 4:
                showRequests(requestQueue);
                break;

            case 5:
                processNextRequest(graph, requestQueue, history);
                break;

            case 6:
                loadHistoryFromFile(history);
                showHistory(history);
                break;

            case 7:
                interactiveRouting(graph, traffic, history);
                break;

            case 0:
                cout << "Exiting...\n";
                break;

            default:
                cout << "Invalid choice.\n";
                break;
        }

    } while (choice != 0);

    return 0;
}