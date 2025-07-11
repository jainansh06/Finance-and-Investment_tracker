#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <numeric>
#include <random>
#include <sstream>
#include <vector>

using namespace std;

enum class TransactionType { INCOME, EXPENSE, INVESTMENT, WITHDRAWAL };
enum class InvestmentType { STOCK, BOND, MUTUAL_FUND, CRYPTO, ETF };
enum class ExpenseCategory {
  FOOD,
  TRANSPORT,
  UTILITIES,
  ENTERTAINMENT,
  HEALTHCARE,
  EDUCATION,
  OTHER
};

class Date {
  int day, month, year;

public:
  Date() {
    auto now = chrono::system_clock::now();
    auto time_t = chrono::system_clock::to_time_t(now);
    auto tm = *localtime(&time_t);
    day = tm.tm_mday;
    month = tm.tm_mon + 1;
    year = tm.tm_year + 1900;
  }
  Date(int d, int m, int y) : day(d), month(m), year(y) {}

  string toString() const {
    return to_string(day) + "/" + to_string(month) + "/" + to_string(year);
  }
  string serialize() const {
    return to_string(day) + "," + to_string(month) + "," + to_string(year);
  }

  static Date deserialize(const string &str) {
    stringstream ss(str);
    string token;
    int d, m, y;
    getline(ss, token, ',');
    d = stoi(token);
    getline(ss, token, ',');
    m = stoi(token);
    getline(ss, token, ',');
    y = stoi(token);
    return Date(d, m, y);
  }
};

class Transaction {
  static int nextId;
  int id;
  string description;
  double amount;
  Date date;
  TransactionType type;
  ExpenseCategory category;

public:
  Transaction(const string &desc, double amt, TransactionType t,
              ExpenseCategory cat = ExpenseCategory::OTHER)
      : id(nextId++), description(desc), amount(amt), date(Date()), type(t),
        category(cat) {}

  Transaction(const string &desc, double amt, const Date &d, TransactionType t,
              ExpenseCategory cat = ExpenseCategory::OTHER)
      : id(nextId++), description(desc), amount(amt), date(d), type(t),
        category(cat) {}

  // Getters
  int getId() const { return id; }
  double getAmount() const { return amount; }
  TransactionType getType() const { return type; }
  ExpenseCategory getCategory() const { return category; }

  string typeToString() const {
    const char *types[] = {"Income", "Expense", "Investment", "Withdrawal"};
    return types[static_cast<int>(type)];
  }

  string categoryToString() const {
    const char *categories[] = {"Food",          "Transport",  "Utilities",
                                "Entertainment", "Healthcare", "Education",
                                "Other"};
    return categories[static_cast<int>(category)];
  }

  void display() const {
    cout << left << setw(5) << id << setw(20) << description << setw(12)
         << fixed << setprecision(2) << "₹" << amount << setw(12)
         << typeToString() << setw(15) << categoryToString() << setw(12)
         << date.toString() << endl;
  }

  string serialize() const {
    return to_string(id) + "," + description + "," + to_string(amount) + "," +
           date.serialize() + "," + to_string(static_cast<int>(type)) + "," +
           to_string(static_cast<int>(category));
  }

  static Transaction deserialize(const string &str) {
    stringstream ss(str);
    string token;
    getline(ss, token, ',');
    int id = stoi(token);
    getline(ss, token, ',');
    string desc = token;
    getline(ss, token, ',');
    double amount = stod(token);

    string dateStr;
    for (int i = 0; i < 3; i++) {
      getline(ss, token, ',');
      dateStr += token + (i < 2 ? "," : "");
    }
    Date date = Date::deserialize(dateStr);

    getline(ss, token, ',');
    TransactionType type = static_cast<TransactionType>(stoi(token));
    getline(ss, token, ',');
    ExpenseCategory category = static_cast<ExpenseCategory>(stoi(token));

    return Transaction(desc, amount, date, type, category);
  }
};
int Transaction::nextId = 1;

class Investment {
  string symbol, name;
  InvestmentType type;
  double quantity, purchasePrice, currentPrice;
  Date purchaseDate;

public:
  Investment(const string &sym, const string &n, InvestmentType t, double qty,
             double price, const Date &date = Date())
      : symbol(sym), name(n), type(t), quantity(qty), purchasePrice(price),
        currentPrice(price), purchaseDate(date) {}

  // Getters
  string getSymbol() const { return symbol; }
  InvestmentType getType() const { return type; }
  double getCurrentPrice() const { return currentPrice; }
  void setCurrentPrice(double price) { currentPrice = price; }

  double getCurrentValue() const { return quantity * currentPrice; }
  double getInitialValue() const { return quantity * purchasePrice; }
  double getGainLoss() const { return getCurrentValue() - getInitialValue(); }
  double getGainLossPercentage() const {
    return (getGainLoss() / getInitialValue()) * 100.0;
  }

  string typeToString() const {
    const char *types[] = {"Stock", "Bond", "Mutual Fund", "Crypto", "ETF"};
    return types[static_cast<int>(type)];
  }

  void display() const {
    cout << left << setw(8) << symbol << setw(20) << name << setw(12)
         << typeToString() << setw(10) << fixed << setprecision(2) << quantity
         << setw(12) << "₹" << purchasePrice << setw(12) << "₹" << currentPrice
         << setw(12) << "₹" << getCurrentValue() << setw(12) << "₹"
         << getGainLoss() << setw(10) << setprecision(1)
         << getGainLossPercentage() << "%" << endl;
  }

  string serialize() const {
    return symbol + "," + name + "," + to_string(static_cast<int>(type)) + "," +
           to_string(quantity) + "," + to_string(purchasePrice) + "," +
           to_string(currentPrice) + "," + purchaseDate.serialize();
  }

  static Investment deserialize(const string &str) {
    stringstream ss(str);
    string token;
    getline(ss, token, ',');
    string symbol = token;
    getline(ss, token, ',');
    string name = token;
    getline(ss, token, ',');
    InvestmentType type = static_cast<InvestmentType>(stoi(token));
    getline(ss, token, ',');
    double quantity = stod(token);
    getline(ss, token, ',');
    double purchasePrice = stod(token);
    getline(ss, token, ',');
    double currentPrice = stod(token);

    string dateStr;
    for (int i = 0; i < 3; i++) {
      getline(ss, token, ',');
      dateStr += token + (i < 2 ? "," : "");
    }
    Date date = Date::deserialize(dateStr);

    Investment inv(symbol, name, type, quantity, purchasePrice, date);
    inv.setCurrentPrice(currentPrice);
    return inv;
  }
};

class Portfolio {
  vector<unique_ptr<Investment>> investments;
  string name;

public:
  Portfolio(const string &n) : name(n) {}

  void addInvestment(unique_ptr<Investment> investment) {
    investments.push_back(move(investment));
  }

  double getTotalValue() const {
    return accumulate(investments.begin(), investments.end(), 0.0,
                      [](double sum, const unique_ptr<Investment> &inv) {
                        return sum + inv->getCurrentValue();
                      });
  }

  double getTotalGainLoss() const {
    return accumulate(investments.begin(), investments.end(), 0.0,
                      [](double sum, const unique_ptr<Investment> &inv) {
                        return sum + inv->getGainLoss();
                      });
  }

  double getTotalGainLossPercentage() const {
    double totalInitial =
        accumulate(investments.begin(), investments.end(), 0.0,
                   [](double sum, const unique_ptr<Investment> &inv) {
                     return sum + inv->getInitialValue();
                   });
    return totalInitial > 0 ? (getTotalGainLoss() / totalInitial) * 100.0 : 0.0;
  }

  void display() const {
    cout << "\n=== Portfolio: " << name << " ===" << endl;
    cout << left << setw(8) << "Symbol" << setw(20) << "Name" << setw(12)
         << "Type" << setw(10) << "Quantity" << setw(12) << "Buy Price"
         << setw(12) << "Current" << setw(12) << "Value" << setw(12)
         << "Gain/Loss" << setw(10) << "%" << endl;
    cout << string(120, '-') << endl;

    for (const auto &inv : investments) {
      inv->display();
    }

    cout << string(120, '-') << endl;
    cout << "Total Portfolio Value: ₹" << fixed << setprecision(2)
         << getTotalValue() << endl;
    cout << "Total Gain/Loss: ₹" << getTotalGainLoss() << " ("
         << setprecision(1) << getTotalGainLossPercentage() << "%)" << endl;
  }

  void updateMarketPrices() {
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(-0.05, 0.05);

    for (auto &inv : investments) {
      double change = dis(gen);
      double newPrice = inv->getCurrentPrice() * (1 + change);
      inv->setCurrentPrice(max(0.01, newPrice));
    }
  }

  size_t getInvestmentCount() const { return investments.size(); }
  Investment *getInvestmentAt(size_t index) {
    return (index < investments.size()) ? investments[index].get() : nullptr;
  }
};

class FinanceTracker {
  vector<unique_ptr<Transaction>> transactions;
  unique_ptr<Portfolio> portfolio;
  string dataFile, portfolioFile;

public:
  FinanceTracker(const string &dataFileName = "finance_data.csv",
                 const string &portfolioFileName = "portfolio_data.csv")
      : dataFile(dataFileName), portfolioFile(portfolioFileName) {
    portfolio = make_unique<Portfolio>("My Portfolio");
    loadData();
  }

  ~FinanceTracker() { saveData(); }

  void addTransaction(const string &description, double amount,
                      TransactionType type,
                      ExpenseCategory category = ExpenseCategory::OTHER) {
    transactions.push_back(
        make_unique<Transaction>(description, amount, type, category));
  }

  void addInvestment(const string &symbol, const string &name,
                     InvestmentType type, double quantity, double price) {
    auto investment =
        make_unique<Investment>(symbol, name, type, quantity, price);
    portfolio->addInvestment(move(investment));
    addTransaction("Investment: " + symbol, quantity * price,
                   TransactionType::INVESTMENT);
  }

  double getTotalIncome() const {
    return accumulate(transactions.begin(), transactions.end(), 0.0,
                      [](double sum, const unique_ptr<Transaction> &t) {
                        return sum + (t->getType() == TransactionType::INCOME
                                          ? t->getAmount()
                                          : 0);
                      });
  }

  double getTotalExpenses() const {
    return accumulate(transactions.begin(), transactions.end(), 0.0,
                      [](double sum, const unique_ptr<Transaction> &t) {
                        return sum + (t->getType() == TransactionType::EXPENSE
                                          ? t->getAmount()
                                          : 0);
                      });
  }

  double getTotalInvestments() const {
    return accumulate(transactions.begin(), transactions.end(), 0.0,
                      [](double sum, const unique_ptr<Transaction> &t) {
                        return sum +
                               (t->getType() == TransactionType::INVESTMENT
                                    ? t->getAmount()
                                    : 0);
                      });
  }

  double getNetWorth() const {
    return getTotalIncome() - getTotalExpenses() + portfolio->getTotalValue();
  }

  map<ExpenseCategory, double> getExpenseByCategory() const {
    map<ExpenseCategory, double> expenses;
    for (const auto &t : transactions) {
      if (t->getType() == TransactionType::EXPENSE) {
        expenses[t->getCategory()] += t->getAmount();
      }
    }
    return expenses;
  }

  void displayTransactions() const {
    cout << "\n=== Transaction History ===" << endl;
    cout << left << setw(5) << "ID" << setw(20) << "Description" << setw(12)
         << "Amount" << setw(12) << "Type" << setw(15) << "Category" << setw(12)
         << "Date" << endl;
    cout << string(80, '-') << endl;
    for (const auto &t : transactions) {
      t->display();
    }
  }

  void displaySummary() const {
    cout << "\n=== Financial Summary ===" << endl;
    cout << "Total Income: ₹" << fixed << setprecision(2) << getTotalIncome()
         << endl;
    cout << "Total Expenses: ₹" << getTotalExpenses() << endl;
    cout << "Total Investments: ₹" << getTotalInvestments() << endl;
    cout << "Portfolio Value: ₹" << portfolio->getTotalValue() << endl;
    cout << "Net Worth: ₹" << getNetWorth() << endl;

    cout << "\n=== Expense Breakdown ===" << endl;
    auto expenseBreakdown = getExpenseByCategory();
    for (const auto &pair : expenseBreakdown) {
      cout << "  " << static_cast<int>(pair.first) << ": ₹" << pair.second
           << endl;
    }
  }

  void displayPortfolio() const { portfolio->display(); }
  void updateMarketPrices() {
    portfolio->updateMarketPrices();
    cout << "Market prices updated!" << endl;
  }

  void saveData() const {
    ofstream file(dataFile);
    if (file.is_open()) {
      for (const auto &t : transactions) {
        file << t->serialize() << endl;
      }
      file.close();
    }

    ofstream portfolioFileStream(portfolioFile);
    if (portfolioFileStream.is_open()) {
      for (size_t i = 0; i < portfolio->getInvestmentCount(); ++i) {
        Investment *inv = portfolio->getInvestmentAt(i);
        if (inv) {
          portfolioFileStream << inv->serialize() << endl;
        }
      }
      portfolioFileStream.close();
    }
  }

  void loadData() {
    ifstream file(dataFile);
    if (file.is_open()) {
      string line;
      while (getline(file, line)) {
        if (!line.empty()) {
          try {
            Transaction t = Transaction::deserialize(line);
            transactions.push_back(make_unique<Transaction>(t));
          } catch (const exception &e) {
            cerr << "Error loading transaction: " << e.what() << endl;
          }
        }
      }
      file.close();
    }

    ifstream portfolioFileStream(portfolioFile);
    if (portfolioFileStream.is_open()) {
      string line;
      while (getline(portfolioFileStream, line)) {
        if (!line.empty()) {
          try {
            Investment inv = Investment::deserialize(line);
            portfolio->addInvestment(make_unique<Investment>(inv));
          } catch (const exception &e) {
            cerr << "Error loading investment: " << e.what() << endl;
          }
        }
      }
      portfolioFileStream.close();
    }
  }
};

class Menu {
  FinanceTracker tracker;

public:
  Menu() : tracker() {}

  void run() {
    int choice;
    do {
      cout << "\n=== Personal Finance & Investment Tracker ===" << endl;
      cout << "1. Add Transaction\n2. Add Investment\n3. View Transactions\n4. "
              "View Portfolio\n5. View Financial Summary\n6. Update Market "
              "Prices\n7. Exit"
           << endl;
      cout << "Enter your choice: ";
      cin >> choice;

      switch (choice) {
      case 1:
        addTransaction();
        break;
      case 2:
        addInvestment();
        break;
      case 3:
        tracker.displayTransactions();
        break;
      case 4:
        tracker.displayPortfolio();
        break;
      case 5:
        tracker.displaySummary();
        break;
      case 6:
        tracker.updateMarketPrices();
        break;
      case 7:
        cout << "Saving data and exiting..." << endl;
        tracker.saveData();
        break;
      default:
        cout << "Invalid choice. Please try again." << endl;
      }

      if (choice != 7) {
        cout << "\nPress Enter to continue...";
        cin.ignore();
        cin.get();
      }
    } while (choice != 7);
  }

private:
  void addTransaction() {
    string description;
    double amount;
    int typeChoice, categoryChoice;

    cout << "\nEnter transaction description: ";
    cin.ignore();
    getline(cin, description);
    cout << "Enter amount: ₹";
    cin >> amount;
    cout << "Select type (1-Income, 2-Expense, 3-Investment, 4-Withdrawal): ";
    cin >> typeChoice;

    TransactionType type = static_cast<TransactionType>(typeChoice - 1);
    ExpenseCategory category = ExpenseCategory::OTHER;

    if (type == TransactionType::EXPENSE) {
      cout << "Select category (1-Food, 2-Transport, 3-Utilities, "
              "4-Entertainment, 5-Healthcare, 6-Education, 7-Other): ";
      cin >> categoryChoice;
      category = static_cast<ExpenseCategory>(categoryChoice - 1);
    }

    tracker.addTransaction(description, amount, type, category);
    cout << "Transaction added successfully!" << endl;
  }

  void addInvestment() {
    string symbol, name;
    int typeChoice;
    double quantity, price;

    cout << "\nEnter investment symbol: ";
    cin >> symbol;
    cout << "Enter investment name: ";
    cin.ignore();
    getline(cin, name);
    cout << "Select type (1-Stock, 2-Bond, 3-Mutual Fund, 4-Crypto, 5-ETF): ";
    cin >> typeChoice;
    cout << "Enter quantity: ";
    cin >> quantity;
    cout << "Enter purchase price per unit: ₹";
    cin >> price;

    InvestmentType type = static_cast<InvestmentType>(typeChoice - 1);
    tracker.addInvestment(symbol, name, type, quantity, price);
    cout << "Investment added successfully!" << endl;
  }
};

int main() {
  try {
    Menu menu;
    menu.run();
  } catch (const exception &e) {
    cerr << "Error: " << e.what() << endl;
    return 1;
  }
  return 0;
}