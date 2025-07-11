#include <algorithm>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <numeric>
#include <random>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

class Transaction;
class Investment;
class Portfolio;
class FinanceTracker;

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
private:
  int day, month, year;

public:
  Date() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto tm = *std::localtime(&time_t);
    day = tm.tm_mday;
    month = tm.tm_mon + 1;
    year = tm.tm_year + 1900;
  }

  Date(int d, int m, int y) : day(d), month(m), year(y) {}

  std::string toString() const {
    return std::to_string(day) + "/" + std::to_string(month) + "/" +
           std::to_string(year);
  }

  bool operator<(const Date &other) const {
    if (year != other.year)
      return year < other.year;
    if (month != other.month)
      return month < other.month;
    return day < other.day;
  }

  bool operator==(const Date &other) const {
    return day == other.day && month == other.month && year == other.year;
  }

  std::string serialize() const { // Convert to string for file storage
    return std::to_string(day) + "," + std::to_string(month) + "," +
           std::to_string(year);
  }
  static Date deserialize(const std::string &str) {
    std::stringstream ss(str);
    std::string token;
    int d, m, y;

    std::getline(ss, token, ',');
    d = std::stoi(token);
    std::getline(ss, token, ',');
    m = std::stoi(token);
    std::getline(ss, token, ',');
    y = std::stoi(token);

    return Date(d, m, y);
  }
};

class Transaction {
private:
  static int nextId;
  int id;
  std::string description;
  double amount;
  Date date;
  TransactionType type;
  ExpenseCategory category;

public:
  Transaction(const std::string &desc, double amt, TransactionType t,
              ExpenseCategory cat = ExpenseCategory::OTHER)
      : id(nextId++), description(desc), amount(amt), date(Date()), type(t),
        category(cat) {}

  Transaction(const std::string &desc, double amt, const Date &d,
              TransactionType t, ExpenseCategory cat = ExpenseCategory::OTHER)
      : id(nextId++), description(desc), amount(amt), date(d), type(t),
        category(cat) {}

  // Getters
  int getId() const { return id; }
  std::string getDescription() const { return description; }
  double getAmount() const { return amount; }
  Date getDate() const { return date; }
  TransactionType getType() const { return type; }
  ExpenseCategory getCategory() const { return category; }

  // Setters
  void setDescription(const std::string &desc) { description = desc; }
  void setAmount(double amt) { amount = amt; }
  void setCategory(ExpenseCategory cat) { category = cat; }

  std::string typeToString() const {
    switch (type) {
    case TransactionType::INCOME:
      return "Income";
    case TransactionType::EXPENSE:
      return "Expense";
    case TransactionType::INVESTMENT:
      return "Investment";
    case TransactionType::WITHDRAWAL:
      return "Withdrawal";
    default:
      return "Unknown";
    }
  }
  std::string categoryToString() const {
    switch (category) {
    case ExpenseCategory::FOOD:
      return "Food";
    case ExpenseCategory::TRANSPORT:
      return "Transport";
    case ExpenseCategory::UTILITIES:
      return "Utilities";
    case ExpenseCategory::ENTERTAINMENT:
      return "Entertainment";
    case ExpenseCategory::HEALTHCARE:
      return "Healthcare";
    case ExpenseCategory::EDUCATION:
      return "Education";
    case ExpenseCategory::OTHER:
      return "Other";
    default:
      return "Unknown";
    }
  }

  void display() const {
    std::cout << std::left << std::setw(5) << id << std::setw(20) << description
              << std::setw(12) << std::fixed << std::setprecision(2) << "₹"
              << amount << std::setw(12) << typeToString() << std::setw(15)
              << categoryToString() << std::setw(12) << date.toString()
              << std::endl;
  }
  std::string serialize() const {
    return std::to_string(id) + "," + description + "," +
           std::to_string(amount) + "," + date.serialize() + "," +
           std::to_string(static_cast<int>(type)) + "," +
           std::to_string(static_cast<int>(category));
  }
  static Transaction deserialize(const std::string &str) {
    std::stringstream ss(str);
    std::string token;

    std::getline(ss, token, ',');
    int id = std::stoi(token);

    std::getline(ss, token, ',');
    std::string desc = token;

    std::getline(ss, token, ',');
    double amount = std::stod(token);

    std::getline(ss, token, ',');
    std::string dateStr = token + ",";
    std::getline(ss, token, ',');
    dateStr += token + ",";
    std::getline(ss, token, ',');
    dateStr += token;
    Date date = Date::deserialize(dateStr);

    std::getline(ss, token, ',');
    TransactionType type = static_cast<TransactionType>(std::stoi(token));

    std::getline(ss, token, ',');
    ExpenseCategory category = static_cast<ExpenseCategory>(std::stoi(token));

    Transaction t(desc, amount, date, type, category);
    return t;
  }
};
int Transaction::nextId = 1;

class Investment {
private:
  std::string symbol;
  std::string name;
  InvestmentType type;
  double quantity;
  double purchasePrice;
  double currentPrice;
  Date purchaseDate;

public:
  Investment(const std::string &sym, const std::string &n, InvestmentType t,
             double qty, double price, const Date &date = Date())
      : symbol(sym), name(n), type(t), quantity(qty), purchasePrice(price),
        currentPrice(price), purchaseDate(date) {}

  // Getters
  std::string getSymbol() const { return symbol; }
  std::string getName() const { return name; }
  InvestmentType getType() const { return type; }
  double getQuantity() const { return quantity; }
  double getPurchasePrice() const { return purchasePrice; }
  double getCurrentPrice() const { return currentPrice; }
  Date getPurchaseDate() const { return purchaseDate; }

  // Setters
  void setCurrentPrice(double price) { currentPrice = price; }
  void addQuantity(double qty) { quantity += qty; }

  double getCurrentValue() const { return quantity * currentPrice; }
  double getInitialValue() const { return quantity * purchasePrice; }
  double getGainLoss() const { return getCurrentValue() - getInitialValue(); }
  double getGainLossPercentage() const {
    return (getGainLoss() / getInitialValue()) * 100.0;
  }

  std::string typeToString() const {
    switch (type) {
    case InvestmentType::STOCK:
      return "Stock";
    case InvestmentType::BOND:
      return "Bond";
    case InvestmentType::MUTUAL_FUND:
      return "Mutual Fund";
    case InvestmentType::CRYPTO:
      return "Crypto";
    case InvestmentType::ETF:
      return "ETF";
    default:
      return "Unknown";
    }
  }

  void display() const {
    std::cout << std::left << std::setw(8) << symbol << std::setw(20) << name
              << std::setw(12) << typeToString() << std::setw(10) << std::fixed
              << std::setprecision(2) << quantity << std::setw(12) << "₹"
              << purchasePrice << std::setw(12) << "₹" << currentPrice
              << std::setw(12) << "₹" << getCurrentValue() << std::setw(12)
              << "₹" << getGainLoss() << std::setw(10) << std::setprecision(1)
              << getGainLossPercentage() << "%" << std::endl;
  }

  std::string serialize() const {
    return symbol + "," + name + "," + std::to_string(static_cast<int>(type)) +
           "," + std::to_string(quantity) + "," +
           std::to_string(purchasePrice) + "," + std::to_string(currentPrice) +
           "," + purchaseDate.serialize();
  }
  static Investment deserialize(const std::string &str) {
    std::stringstream ss(str);
    std::string token;

    std::getline(ss, token, ',');
    std::string symbol = token;

    std::getline(ss, token, ',');
    std::string name = token;

    std::getline(ss, token, ',');
    InvestmentType type = static_cast<InvestmentType>(std::stoi(token));

    std::getline(ss, token, ',');
    double quantity = std::stod(token);

    std::getline(ss, token, ',');
    double purchasePrice = std::stod(token);

    std::getline(ss, token, ',');
    double currentPrice = std::stod(token);

    std::getline(ss, token, ',');
    std::string dateStr = token + ",";
    std::getline(ss, token, ',');
    dateStr += token + ",";
    std::getline(ss, token, ',');
    dateStr += token;
    Date date = Date::deserialize(dateStr);

    Investment inv(symbol, name, type, quantity, purchasePrice, date);
    inv.setCurrentPrice(currentPrice);
    return inv;
  }
};

class Portfolio {
private:
  std::vector<std::unique_ptr<Investment>> investments;
  std::string name;

public:
  Portfolio(const std::string &n) : name(n) {}

  void addInvestment(std::unique_ptr<Investment> investment) {
    investments.push_back(std::move(investment));
  }
  bool
  removeInvestment(const std::string &symbol) { // Remove investment by symbol
    auto it = std::find_if(investments.begin(), investments.end(),
                           [&symbol](const std::unique_ptr<Investment> &inv) {
                             return inv->getSymbol() == symbol;
                           });

    if (it != investments.end()) {
      investments.erase(it);
      return true;
    }
    return false;
  }
  Investment *
  getInvestment(const std::string &symbol) { // Get investment by symbol
    auto it = std::find_if(investments.begin(), investments.end(),
                           [&symbol](const std::unique_ptr<Investment> &inv) {
                             return inv->getSymbol() == symbol;
                           });

    return (it != investments.end()) ? it->get() : nullptr;
  }

  double getTotalValue() const { // portfolio metrics
    return std::accumulate(
        investments.begin(), investments.end(), 0.0,
        [](double sum, const std::unique_ptr<Investment> &inv) {
          return sum + inv->getCurrentValue();
        });
  }

  double getTotalGainLoss() const {
    return std::accumulate(
        investments.begin(), investments.end(), 0.0,
        [](double sum, const std::unique_ptr<Investment> &inv) {
          return sum + inv->getGainLoss();
        });
  }

  double getTotalGainLossPercentage() const {
    double totalInitial =
        std::accumulate(investments.begin(), investments.end(), 0.0,
                        [](double sum, const std::unique_ptr<Investment> &inv) {
                          return sum + inv->getInitialValue();
                        });

    return totalInitial > 0 ? (getTotalGainLoss() / totalInitial) * 100.0 : 0.0;
  }

  // analysis
  std::map<InvestmentType, double> getDiversification() const {
    std::map<InvestmentType, double> distribution;
    double totalValue = getTotalValue();

    for (const auto &inv : investments) {
      distribution[inv->getType()] += inv->getCurrentValue();
    }
    for (auto &pair : distribution) {
      pair.second = (pair.second / totalValue) * 100.0;
    }

    return distribution;
  }

  void display() const {
    std::cout << "\n=== Portfolio: " << name << " ===" << std::endl;
    std::cout << std::left << std::setw(8) << "Symbol" << std::setw(20)
              << "Name" << std::setw(12) << "Type" << std::setw(10)
              << "Quantity" << std::setw(12) << "Buy Price" << std::setw(12)
              << "Current" << std::setw(12) << "Value" << std::setw(12)
              << "Gain/Loss" << std::setw(10) << "%" << std::endl;
    std::cout << std::string(120, '-') << std::endl;

    for (const auto &inv : investments) {
      inv->display();
    }

    std::cout << std::string(120, '-') << std::endl;
    std::cout << "Total Portfolio Value: ₹" << std::fixed
              << std::setprecision(2) << getTotalValue() << std::endl;
    std::cout << "Total Gain/Loss: ₹" << getTotalGainLoss() << " ("
              << std::setprecision(1) << getTotalGainLossPercentage() << "%)"
              << std::endl;

    std::cout << "\nDiversification:" << std::endl;
    auto diversification = getDiversification();
    for (const auto &pair : diversification) {
      std::cout << "  " << static_cast<int>(pair.first) << ": "
                << std::setprecision(1) << pair.second << "%" << std::endl;
    }
  }

  void updateMarketPrices() { // market change simulation
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-0.05, 0.05); // -5% to +5% change

    for (auto &inv : investments) {
      double change = dis(gen);
      double newPrice = inv->getCurrentPrice() * (1 + change);
      inv->setCurrentPrice(std::max(0.01, newPrice)); // Minimum price of 0.01rs
    }
  }
  size_t getInvestmentCount() const { return investments.size(); }

  Investment *getInvestmentAt(size_t index) {
    return (index < investments.size()) ? investments[index].get() : nullptr;
  }
};

class FinanceTracker {
private:
  std::vector<std::unique_ptr<Transaction>> transactions;
  std::unique_ptr<Portfolio> portfolio;
  std::string dataFile;
  std::string portfolioFile;

public:
  FinanceTracker(const std::string &dataFileName = "finance_data.csv",
                 const std::string &portfolioFileName = "portfolio_data.csv")
      : dataFile(dataFileName), portfolioFile(portfolioFileName) {
    portfolio = std::make_unique<Portfolio>("My Portfolio");
    loadData();
  }

  ~FinanceTracker() { saveData(); }

  void addTransaction(const std::string &description, double amount,
                      TransactionType type,
                      ExpenseCategory category = ExpenseCategory::OTHER) {
    transactions.push_back(
        std::make_unique<Transaction>(description, amount, type, category));
  }
  void addInvestment(const std::string &symbol, const std::string &name,
                     InvestmentType type, double quantity, double price) {
    auto investment =
        std::make_unique<Investment>(symbol, name, type, quantity, price);
    portfolio->addInvestment(std::move(investment));

    addTransaction("Investment: " + symbol, quantity * price,
                   TransactionType::INVESTMENT);
  }
  double getTotalIncome() const {
    return std::accumulate(
        transactions.begin(), transactions.end(), 0.0,
        [](double sum, const std::unique_ptr<Transaction> &t) {
          return sum +
                 (t->getType() == TransactionType::INCOME ? t->getAmount() : 0);
        });
  }
  double getTotalExpenses() const {
    return std::accumulate(
        transactions.begin(), transactions.end(), 0.0,
        [](double sum, const std::unique_ptr<Transaction> &t) {
          return sum + (t->getType() == TransactionType::EXPENSE
                            ? t->getAmount()
                            : 0);
        });
  }
  double getTotalInvestments() const {
    return std::accumulate(
        transactions.begin(), transactions.end(), 0.0,
        [](double sum, const std::unique_ptr<Transaction> &t) {
          return sum + (t->getType() == TransactionType::INVESTMENT
                            ? t->getAmount()
                            : 0);
        });
  }
  double getNetWorth() const {
    return getTotalIncome() - getTotalExpenses() + portfolio->getTotalValue();
  }

  // expense report
  std::map<ExpenseCategory, double> getExpenseByCategory() const {
    std::map<ExpenseCategory, double> expenses;

    for (const auto &t : transactions) {
      if (t->getType() == TransactionType::EXPENSE) {
        expenses[t->getCategory()] += t->getAmount();
      }
    }

    return expenses;
  }

  void displayTransactions() const {
    std::cout << "\n=== Transaction History ===" << std::endl;
    std::cout << std::left << std::setw(5) << "ID" << std::setw(20)
              << "Description" << std::setw(12) << "Amount" << std::setw(12)
              << "Type" << std::setw(15) << "Category" << std::setw(12)
              << "Date" << std::endl;
    std::cout << std::string(80, '-') << std::endl;

    for (const auto &t : transactions) {
      t->display();
    }
  }

  void displaySummary() const {
    std::cout << "\n=== Financial Summary ===" << std::endl;
    std::cout << "Total Income: ₹" << std::fixed << std::setprecision(2)
              << getTotalIncome() << std::endl;
    std::cout << "Total Expenses: ₹" << getTotalExpenses() << std::endl;
    std::cout << "Total Investments: ₹" << getTotalInvestments() << std::endl;
    std::cout << "Portfolio Value: ₹" << portfolio->getTotalValue()
              << std::endl;
    std::cout << "Net Worth: ₹" << getNetWorth() << std::endl;

    std::cout << "\n=== Expense Breakdown ===" << std::endl;
    auto expenseBreakdown = getExpenseByCategory();
    for (const auto &pair : expenseBreakdown) {
      std::cout << "  " << static_cast<int>(pair.first) << ": $" << pair.second
                << std::endl;
    }
  }

  void displayPortfolio() const { portfolio->display(); }

  void updateMarketPrices() {
    portfolio->updateMarketPrices();
    std::cout << "Market prices updated!" << std::endl;
  }

  void saveData() const {
    // Save transactions
    std::ofstream file(dataFile);
    if (file.is_open()) {
      for (const auto &t : transactions) {
        file << t->serialize() << std::endl;
      }
      file.close();
    }

    std::ofstream portfolioFileStream(portfolioFile);
    if (portfolioFileStream.is_open()) {
      for (size_t i = 0; i < portfolio->getInvestmentCount(); ++i) {
        Investment *inv = portfolio->getInvestmentAt(i);
        if (inv) {
          portfolioFileStream << inv->serialize() << std::endl;
        }
      }
      portfolioFileStream.close();
    }
  }

  void loadData() {
    // Load transactions
    std::ifstream file(dataFile);
    if (file.is_open()) {
      std::string line;
      while (std::getline(file, line)) {
        if (!line.empty()) {
          try {
            Transaction t = Transaction::deserialize(line);
            transactions.push_back(std::make_unique<Transaction>(t));
          } catch (const std::exception &e) {
            std::cerr << "Error loading transaction: " << e.what() << std::endl;
          }
        }
      }
      file.close();
    }

    std::ifstream portfolioFileStream(portfolioFile);
    if (portfolioFileStream.is_open()) {
      std::string line;
      while (std::getline(portfolioFileStream, line)) {
        if (!line.empty()) {
          try {
            Investment inv = Investment::deserialize(line);
            portfolio->addInvestment(std::make_unique<Investment>(inv));
          } catch (const std::exception &e) {
            std::cerr << "Error loading investment: " << e.what() << std::endl;
          }
        }
      }
      portfolioFileStream.close();
    }
  }
};

class Menu {
private:
  FinanceTracker tracker;

public:
  Menu() : tracker() {}

  void run() {
    int choice;

    do {
      displayMenu();
      std::cin >> choice;

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
        std::cout << "Saving data and exiting..." << std::endl;
        tracker.saveData();
        break;
      default:
        std::cout << "Invalid choice. Please try again." << std::endl;
      }

      if (choice != 7) {
        std::cout << "\nPress Enter to continue...";
        std::cin.ignore();
        std::cin.get();
      }

    } while (choice != 7);
  }

private:
  void displayMenu() {
    std::cout << "\n=== Personal Finance & Investment Tracker ===" << std::endl;
    std::cout << "1. Add Transaction" << std::endl;
    std::cout << "2. Add Investment" << std::endl;
    std::cout << "3. View Transactions" << std::endl;
    std::cout << "4. View Portfolio" << std::endl;
    std::cout << "5. View Financial Summary" << std::endl;
    std::cout << "6. Update Market Prices" << std::endl;
    std::cout << "7. Exit" << std::endl;
    std::cout << "Enter your choice: ";
  }

  void addTransaction() {
    std::string description;
    double amount;
    int typeChoice, categoryChoice;

    std::cout << "\nEnter transaction description: ";
    std::cin.ignore();
    std::getline(std::cin, description);

    std::cout << "Enter amount: ₹";
    std::cin >> amount;

    std::cout
        << "Select type (1-Income, 2-Expense, 3-Investment, 4-Withdrawal): ";
    std::cin >> typeChoice;

    TransactionType type = static_cast<TransactionType>(typeChoice - 1);

    ExpenseCategory category = ExpenseCategory::OTHER;
    if (type == TransactionType::EXPENSE) {
      std::cout << "Select category (1-Food, 2-Transport, 3-Utilities, "
                   "4-Entertainment, 5-Healthcare, 6-Education, 7-Other): ";
      std::cin >> categoryChoice;
      category = static_cast<ExpenseCategory>(categoryChoice - 1);
    }

    tracker.addTransaction(description, amount, type, category);
    std::cout << "Transaction added successfully!" << std::endl;
  }

  void addInvestment() {
    std::string symbol, name;
    int typeChoice;
    double quantity, price;

    std::cout << "\nEnter investment symbol: ";
    std::cin >> symbol;

    std::cout << "Enter investment name: ";
    std::cin.ignore();
    std::getline(std::cin, name);

    std::cout
        << "Select type (1-Stock, 2-Bond, 3-Mutual Fund, 4-Crypto, 5-ETF): ";
    std::cin >> typeChoice;

    std::cout << "Enter quantity: ";
    std::cin >> quantity;

    std::cout << "Enter purchase price per unit: ₹";
    std::cin >> price;

    InvestmentType type = static_cast<InvestmentType>(typeChoice - 1);

    tracker.addInvestment(symbol, name, type, quantity, price);
    std::cout << "Investment added successfully!" << std::endl;
  }
};

int main() {
  try {
    Menu menu;
    menu.run();
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
  return 0;
}
