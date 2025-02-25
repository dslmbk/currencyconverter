#include <windows.h>
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <iomanip>
#include <algorithm> // для transform (toupper)

using namespace std;

// Глобальный словарь с курсами валют (по отношению к USD = 1.0)
map<string, double> exchangeRates = {
    {"USD", 1.0},   // базовая
    {"KZT", 498.0},
    {"EUR", 0.95},
    {"RUB", 86.0}
};

// Выводим дату (жёстко заданную)
const string FIXED_DATE = "25.02.2025";

// ---------------------------------------------------------------------------
// ФУНКЦИИ

// Запись конвертации в history.txt
void saveToHistory(double amount, const string& from, double result, const string& to) {
    ofstream fout("history.txt", ios::app);
    if (!fout.is_open()) {
        cerr << "Не удалось открыть файл history.txt для записи!\n";
        return;
    }
    fout << fixed << setprecision(2);
    fout << amount << " " << from << " -> " << result << " " << to << endl;
    fout.close();
}

// Показать историю конвертаций
void showHistory() {
    ifstream fin("history.txt");
    if (!fin.is_open()) {
        cout << "История пуста или файл history.txt недоступен.\n";
        return;
    }
    cout << "\n===== ИСТОРИЯ КОНВЕРТАЦИЙ =====\n";
    string line;
    while (getline(fin, line)) {
        cout << line << endl;
    }
    fin.close();
    cout << "================================\n\n";
}

// Показать текущие курсы (относительно USD)
void showRates() {
    cout << "\n===== ТЕКУЩИЕ КУРСЫ (за 1 USD) =====\n";
    for (auto& kv : exchangeRates) {
        // Сам USD = 1.0, но выведем тоже
        cout << kv.first << " = " << kv.second << endl;
    }
    cout << "====================================\n\n";
}

// Установить новый курс для валюты
void setRate() {
    cout << "Введите код валюты, для которой меняем курс (USD, KZT, EUR, RUB и т.д.): ";
    string currency;
    cin >> currency;
    // Переведём в верхний регистр
    transform(currency.begin(), currency.end(), currency.begin(), ::toupper);

    cout << "Введите новый курс относительно USD: ";
    double newRate;
    cin >> newRate;
    if (!cin.good()) {
        cin.clear();
        cin.ignore(1000, '\n');
        cout << "Ошибка: некорректный ввод числа.\n\n";
        return;
    }

    // Добавим валюту, даже если её раньше не было, или обновим
    exchangeRates[currency] = newRate;
    cout << "Курс для " << currency << " успешно обновлён.\n\n";
}

// Функция для конвертации
double convert(double amount, const string& from, const string& to) {
    // Проверяем, есть ли такие валюты
    if (exchangeRates.find(from) == exchangeRates.end() ||
        exchangeRates.find(to) == exchangeRates.end()) {
        return -1.0; // ошибка
    }
    return (amount / exchangeRates[from]) * exchangeRates[to];
}

// Считывание суммы с проверкой
double readAmount() {
    double amount;
    while (true) {
        cin >> amount;
        if (!cin.fail() && amount >= 0) {
            // всё ок
            break;
        }
        // Если не ок, очищаем поток и просим заново
        cin.clear();
        cin.ignore(1000, '\n');
        cout << "Ошибка: введите положительное число: ";
    }
    return amount;
}

// ---------------------------------------------------------------------------
// MAIN

int main() {
    // Устанавливаем OEM-кодировку консоли (CP866) для русских букв
    // Если у тебя CP1251 — поставь 1251
    SSetConsoleCP(1251);
    SetConsoleOutputCP(1251);


    cout << "Сегодняшняя дата: " << FIXED_DATE << "\n\n";
    cout << "========== КОНВЕРТЕР ВАЛЮТ ==========\n"
        << "Поддерживаемые команды:\n"
        << "1) 'exit'    - Выход из программы.\n"
        << "2) 'history' - Посмотреть историю конвертаций.\n"
        << "3) 'rates'   - Показать текущие курсы.\n"
        << "4) 'set'     - Изменить/добавить курс валюты.\n"
        << "5) Или введите код валюты (USD, RUB и т.д.) - Начать конвертацию.\n\n";

    while (true) {
        cout << "Введите валюту-источник или команду: ";
        string from;
        cin >> from;

        // Переведём ввод в верхний регистр, чтобы usd и USD считались одинаково
        transform(from.begin(), from.end(), from.begin(), ::toupper);

        // Обработка команд
        if (from == "EXIT") {
            cout << "Выход из программы...\n";
            break;
        }
        if (from == "HISTORY") {
            showHistory();
            continue;
        }
        if (from == "RATES") {
            showRates();
            continue;
        }
        if (from == "SET") {
            setRate();
            continue;
        }

        // Иначе предполагаем, что это код валюты "from"
        cout << "Введите валюту, в которую конвертировать: ";
        string to;
        cin >> to;
        transform(to.begin(), to.end(), to.begin(), ::toupper);

        cout << "Введите сумму: ";
        double amount = readAmount();

        double result = convert(amount, from, to);
        if (result < 0) {
            cout << "Ошибка: Валюта не найдена в справочнике!\n\n";
            continue;
        }

        cout << fixed << setprecision(2);
        cout << amount << " " << from << " = "
            << result << " " << to << "\n\n";

        // Сохраняем в history
        saveToHistory(amount, from, result, to);
    }

    return 0;
}
