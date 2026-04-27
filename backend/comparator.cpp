#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <queue>
#include <cmath>
#include<iomanip>

using namespace std;

struct Product {
    int id;
    string name;
    string category;
    float price;
    float rating;
    string platform;
    string link;
};

vector<Product> loadData() {
    vector<Product> products;
    ifstream file("C:/xampp/htdocs/data/products.csv");
    string line;

    if (!file.is_open()) {
        cout << "<h2>ERROR: CSV not found</h2>";
        return products;
    }

    getline(file, line); // skip header

    while (getline(file, line)) {
        stringstream ss(line);
        Product p;
        string temp;

        try {
            getline(ss, temp, ','); if (temp.empty()) continue;
            p.id = stoi(temp);

            getline(ss, p.name, ',');
            getline(ss, p.category, ',');

            getline(ss, temp, ','); p.price = stof(temp);
            getline(ss, temp, ','); p.rating = stof(temp);

            getline(ss, p.platform, ',');
            getline(ss, p.link, ',');

            products.push_back(p);
        } catch (...) {
            continue;
        }
    }
    return products;
}

vector<Product> searchProducts(vector<Product>& products, string keyword) {
    vector<Product> result;

    transform(keyword.begin(), keyword.end(), keyword.begin(), ::tolower);

    for (auto &p : products) {
        string name = p.name;
        transform(name.begin(), name.end(), name.begin(), ::tolower);

        if (name.find(keyword) != string::npos) {
            result.push_back(p);
        }
    }
    return result;
}

bool comparePrice(const Product &a, const Product &b) {
    return a.price < b.price;
}

bool compareRating(const Product &a, const Product &b) {
    return a.rating > b.rating;
}

Product getBestProduct(vector<Product>& products) {
    float maxRating = 0, minPrice = 1e9;

    for (auto &p : products) {
        maxRating = max(maxRating, p.rating);
        minPrice = min(minPrice, p.price);
    }

    Product best = products[0];
    float bestScore = -1;

    for (auto &p : products) {
        float score = 0.6 * (p.rating / maxRating) + 0.4 * (minPrice / p.price);

        if (score > bestScore) {
            bestScore = score;
            best = p;
        }
    }
    return best;
}

vector<Product> getTopKProducts(vector<Product>& products, int k) {

    auto cmp = [](const Product &a, const Product &b) {
        return a.rating < b.rating;
    };

    priority_queue<Product, vector<Product>, decltype(cmp)> pq(cmp);

    for (auto &p : products) pq.push(p);

    vector<Product> topK;
    while (!pq.empty() && k--) {
        topK.push_back(pq.top());
        pq.pop();
    }
    return topK;
}

vector<Product> getSmartRecommendations(vector<Product>& products, Product base) {

    vector<pair<float, Product>> scored;

    for (auto &p : products) {
        if (p.category == base.category && p.name != base.name) {

            float score = 0;
            score += (p.rating / 5.0) * 0.5;
            score += (1.0 / (fabs(p.price - base.price) + 1)) * 0.5;

            scored.push_back({score, p});
        }
    }

    sort(scored.begin(), scored.end(),
         [](const pair<float, Product> &a, const pair<float, Product> &b) {
             return a.first > b.first;
         });

    vector<Product> result;
    for (int i = 0; i < min(3, (int)scored.size()); i++) {
        result.push_back(scored[i].second);
    }

    return result;
}

vector<Product> getSameProductDeals(vector<Product>& products, string query) {
    vector<Product> deals;

    for (auto &p : products) {
        string name = p.name;
        transform(name.begin(), name.end(), name.begin(), ::tolower);

        if (name == query) deals.push_back(p);
    }
    return deals;
}

Product getBestDeal(vector<Product>& deals) {
    Product best = deals[0];

    for (auto &d : deals) {
        if (d.price < best.price ||
           (d.price == best.price && d.rating > best.rating)) {
            best = d;
        }
    }
    return best;
}

void printAdvancedHTML(vector<Product>& products,
                       Product best,
                       vector<Product>& topK,
                       vector<Product>& recommendations,
                       string sortType) {

    cout << "<h2>Search Results</h2>";
    cout << "<div class='grid'>";

    for (auto &p : products) {
        cout << "<div class='card'>";

        cout << "<h3>" << p.name << "</h3>";

        cout << "<p class='platform'>🛒 " << p.platform << "</p>";

        cout << "<p class='price'>₹" << p.price << "</p>";
        cout << "<p class='rating'>⭐ " << p.rating << "</p>";

        if (p.name == best.name) {
            if (sortType == "rating")
                cout << "<p class='best'>⭐ Highest Rated</p>";
            else if (sortType == "price")
                cout << "<p class='best'>💰 Cheapest</p>";
            else
                cout << "<p class='best'>🏆 Best Value</p>";
        }

        string link = p.link;
        if (link.find("http") != 0) {
            link = "https://" + link;
        }

        cout << "<a href='" << link << "' target='_blank'>";
        cout << "<button class='buy-btn'>Buy on " << p.platform << "</button>";
        cout << "</a>";

        cout << "</div>";
    }

    cout << "</div>";

    // TOP K
    cout << "<h2>Top Rated Products</h2>";
    for (auto &p : topK) {
        cout << "<p>" << p.name << " (⭐ " << p.rating << ")</p>";
    }

    // RECOMMENDATIONS
    cout << "<h2>Smart Recommendations</h2>";
    for (auto &p : recommendations) {
        cout << "<p>" << p.name << " - ₹" << p.price << "</p>";
    }
}
void printComparison(vector<Product>& deals, Product bestDeal) {

    cout << "<h2>Price Comparison</h2>";
    cout << "<table border='1'>";
    cout << "<tr><th>Platform</th><th>Price</th><th>Rating</th></tr>";

    for (auto &d : deals) {
        cout << "<tr><td>" << d.platform << "</td><td>"
             << d.price << "</td><td>" << d.rating << "</td></tr>";
    }

    cout << "</table>";
    cout << "<h3 style='color:green'>Best Deal: "
         << bestDeal.platform << " (₹" << bestDeal.price << ")</h3>";
}

string urlDecode(string str) {
    string ret;
    char ch;
    int i, ii;
    for (i = 0; i < str.length(); i++) {
        if (str[i] == '%') {
            sscanf(str.substr(i + 1, 2).c_str(), "%x", &ii);
            ch = static_cast<char>(ii);
            ret += ch;
            i = i + 2;
        } else if (str[i] == '+') {
            ret += ' ';
        } else {
            ret += str[i];
        }
    }
    return ret;
}

int main() {

    cout << "Content-type:text/html; charset=UTF-8\r\n\r\n";

    string data = getenv("QUERY_STRING") ? getenv("QUERY_STRING") : "";
    string query = "", sortType = "price";

    int pos = data.find("query=");
    if (pos != string::npos) {
        int start = pos + 6;
        int end = data.find("&", start);
        query = (end != string::npos) ? data.substr(start, end - start) : data.substr(start);
    }

    int sortPos = data.find("sort=");
    if (sortPos != string::npos) {
        int start = sortPos + 5;
        int end = data.find("&", start);
        sortType = (end != string::npos) ? data.substr(start, end - start) : data.substr(start);
    }

    transform(query.begin(), query.end(), query.begin(), ::tolower);
    query = urlDecode(query);

    vector<Product> products = loadData();

    vector<Product> deals = getSameProductDeals(products, query);
    if (deals.size() > 1) {
        Product bestDeal = getBestDeal(deals);
        printComparison(deals, bestDeal);
        return 0;
    }

    vector<Product> filtered = searchProducts(products, query);

    if (filtered.empty()) {
        cout << "<h2>No products found</h2>";
        return 0;
    }

    if (sortType == "rating")
        sort(filtered.begin(), filtered.end(), compareRating);
    else
        sort(filtered.begin(), filtered.end(), comparePrice);

    Product best;

if (sortType == "rating") {
    best = filtered[0];   
} else if (sortType == "price") {
    best = filtered[0];   
} else {
    best = getBestProduct(filtered); 
}

    vector<Product> topK = getTopKProducts(filtered, 3);
    vector<Product> recommendations = getSmartRecommendations(products, best);

    printAdvancedHTML(filtered, best, topK, recommendations, sortType);

    return 0;
}