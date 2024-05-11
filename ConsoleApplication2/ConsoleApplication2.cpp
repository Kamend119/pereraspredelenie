#include <iostream>
#include <vector>
#include <numeric> // Для функции accumulate
#include <limits>

using namespace std;

// Структура для представления элемента опорного плана
struct Cell {
    int row;
    int col;
    int value;
};

// Функция для выполнения алгебраического суммирования коэффициентов по всему контуру
int sumCoefficients(const vector<Cell>& contour) {
    int sum = 0;
    for (const auto& cell : contour) {
        sum += cell.value;
    }
    return sum;
}

// Функция для проверки оптимальности контура
bool checkOptimality(const vector<Cell>& contour, bool minimize) {
    int sum = sumCoefficients(contour);
    return (minimize && sum >= 0) || (!minimize && sum <= 0);
}

// Функция для перераспределения ресурсов в контуре
void redistributeResources(vector<vector<int>>& cost_matrix, vector<Cell>& contour) {
    // Найдем минимальное отрицательное значение в контуре
    int min_negative_value = numeric_limits<int>::max();
    for (const auto& cell : contour) {
        if (cell.value < 0 && cell.value < min_negative_value) {
            min_negative_value = cell.value;
        }
    }

    // Уменьшим значения только в отрицательных ячейках
    for (auto& cell : contour) {
        if (cell.value < 0) {
            cost_matrix[cell.row][cell.col] -= min_negative_value;
        }
    }
}

// Функция для поиска замкнутого контура, начиная с указанной клетки
vector<Cell> findClosedContour(vector<vector<int>>& cost_matrix, vector<vector<bool>>& visited, int start_row, int start_col) {
    vector<Cell> contour;
    int row = start_row;
    int col = start_col;
    int dir = 0; // Направление: 0 - вправо, 1 - вниз, 2 - влево, 3 - вверх

    int initial_row = start_row;
    int initial_col = start_col;
    int iterations = 0; // Счетчик итераций для контроля выхода из цикла

    int m = cost_matrix.size(); // Количество строк
    int n = cost_matrix[0].size(); // Количество столбцов

    while (iterations < m * n) { // Выход из цикла после обхода всей матрицы
        visited[row][col] = true;
        contour.push_back({ row, col, cost_matrix[row][col] });

        if (dir == 0) {
            if (col + 1 < n && !visited[row][col + 1]) {
                col++;
            }
            else {
                dir = 1;
            }
        }
        else if (dir == 1) {
            if (row + 1 < m && !visited[row + 1][col]) {
                row++;
            }
            else {
                dir = 2;
            }
        }
        else if (dir == 2) {
            if (col - 1 >= 0 && !visited[row][col - 1]) {
                col--;
            }
            else {
                dir = 3;
            }
        }
        else if (dir == 3) {
            if (row - 1 >= 0 && !visited[row - 1][col]) {
                row--;
            }
            else {
                dir = 0;
            }
        }

        iterations++;

        // Проверяем, вернулись ли мы в начальную клетку и что контур больше одной клетки
        if (row == initial_row && col == initial_col && contour.size() > 1) {
            break; // Достигнута начальная клетка и контур замкнут
        }
        else if (row == start_row && col == start_col && contour.size() == 1) {
            // Вернулись в начальную клетку, но контур состоит только из одной клетки, продолжаем движение
            if (dir == 0) {
                dir = 1;
            }
            else if (dir == 1) {
                dir = 2;
            }
            else if (dir == 2) {
                dir = 3;
            }
            else if (dir == 3) {
                dir = 0;
            }
            initial_row = row;
            initial_col = col;
        }
    }

    return contour;
}


// Функция для поиска начального пустого элемента и заполнения опорного плана с помощью алгоритма сеток
void initializeBasicPlan(vector<vector<int>>& cost_matrix, vector<int>& supply, vector<int>& demand, bool minimize) {
    int m = cost_matrix.size(); // Количество поставщиков
    int n = cost_matrix[0].size(); // Количество потребителей

    // Переменные для хранения оставшихся объемов поставок и потребностей
    vector<int> remaining_supply(supply);
    vector<int> remaining_demand(demand);

    // Инициализация опорного плана нулями
    vector<vector<int>> basic_plan(m, vector<int>(n, 0));

    // Проходим по строкам и столбцам, заполняя опорный план
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            // Вычисляем минимум из оставшихся поставок и потребностей для данной ячейки
            int min_value = min(remaining_supply[i], remaining_demand[j]);

            // Заполняем ячейку опорного плана этим минимумом
            basic_plan[i][j] = min_value;

            // Уменьшаем оставшиеся поставки и потребности на использованный объем
            remaining_supply[i] -= min_value;
            remaining_demand[j] -= min_value;
        }
    }

    // Обновляем стоимости перевозок с учетом опорного плана
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            cost_matrix[i][j] -= basic_plan[i][j];
        }
    }

    // Выводим опорный план
    cout << "Опорный план перевозок:" << endl;
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            cout << basic_plan[i][j] << "\t";
        }
        cout << endl;
    }
}

// Основная функция для решения транспортной задачи
void solveTransportationProblem(vector<vector<int>>& cost_matrix, vector<int>& supply, vector<int>& demand, bool minimize) {
    cout << "Решение транспортной задачи..." << endl;

    // Проверка на балансировку спроса и предложения
    int total_supply = accumulate(supply.begin(), supply.end(), 0);
    int total_demand = accumulate(demand.begin(), demand.end(), 0);
    if (total_supply != total_demand) {
        cerr << "Ошибка: Несбалансированная транспортная задача!" << endl;
        return;
    }

    cout << "Балансировка спроса и предложения..." << endl;

    int m = cost_matrix.size(); // Количество поставщиков
    int n = cost_matrix[0].size(); // Количество потребителей

    // Инициализация опорного плана
    initializeBasicPlan(cost_matrix, supply, demand, minimize);

    // Поиск оптимального плана
    cout << "Поиск оптимального плана..." << endl;

    // Освобождаем память для массива посещенных клеток
    vector<vector<bool>> visited(m, vector<bool>(n, false));

    // Обходим каждую клетку опорного плана и ищем контур
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            if (!visited[i][j]) {
                vector<Cell> contour = findClosedContour(cost_matrix, visited, i, j);
                if (!checkOptimality(contour, minimize)) {
                    cout << "Перераспределение ресурсов..." << endl;
                    redistributeResources(cost_matrix, contour);
                    // Повторная проверка оптимальности после перераспределения
                    if (!checkOptimality(contour, minimize)) {
                        cerr << "Ошибка: Перераспределение не привело к оптимальному решению." << endl;
                        return;
                    }
                }
            }
        }
    }

    // Вывод оптимального плана
    cout << "Оптимальный план перевозок:" << endl;
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            cout << cost_matrix[i][j] << "\t";
        }
        cout << endl;
    }
}

int main() {
    setlocale(LC_ALL, "Russian");

    // Пример входных данных: стоимости перевозок
    vector<vector<int>> cost_matrix = { {6, 5, 8, 7},
                                         {3, 6, 4, 2},
                                         {9, 1, 3, 6} };

    // Пример входных данных: доступное предложение и спрос
    vector<int> supply = { 14, 12, 8 };
    vector<int> demand = { 10, 14, 6, 4 };

    bool minimize = true; // Решение с минимальными затратами

    // Решение транспортной задачи
    solveTransportationProblem(cost_matrix, supply, demand, minimize);

    return 0;
}
