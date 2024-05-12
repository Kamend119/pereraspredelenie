#include <iostream>
#include <vector>
#include <limits>
#include <queue>
#include <utility> // for pair
#include <cmath> // for abs
using namespace std;

void printMatrix(const vector<vector<int>>& matrix) {
    for (const auto& row : matrix) {
        for (int value : row) {
            cout << value << "\t";
        }
        cout << endl;
    }
}

void printPairs(const vector<pair<int, int>>& pairs) {
    for (const auto& p : pairs) {
        cout << "(" << p.first + 1 << ", " << p.second + 1 << ") ";
    }
    cout << endl;
}

void printVector(const vector<int>& nums) {
    for (const auto& num : nums) {
        cout << num << " ";
    }
    cout << endl;
}

// найти опорный план методом минимального элемента
vector<vector<int>> minCostMethod(vector<vector<int>>& cost_matrix, vector<int>& supply, vector<int>& demand) {
    int num_supply = supply.size();
    int num_demand = demand.size();
    int total_supply = 0, total_demand = 0;

    for (int i = 0; i < num_supply; ++i)
        total_supply += supply[i];
    for (int j = 0; j < num_demand; ++j)
        total_demand += demand[j];

    if (total_supply != total_demand)
        throw invalid_argument("Total supply and demand do not match.");

    vector<vector<int>> allocation(num_supply, vector<int>(num_demand, 0));
    vector<vector<int>> costs = cost_matrix;

    while (total_supply > 0 && total_demand > 0) {
        int min_cost = INT_MAX;
        int min_i = -1, min_j = -1;

        for (int i = 0; i < num_supply; ++i) {
            for (int j = 0; j < num_demand; ++j) {
                if (costs[i][j] < min_cost) {
                    min_cost = costs[i][j];
                    min_i = i;
                    min_j = j;
                }
            }
        }

        int min_allocation = min(supply[min_i], demand[min_j]);
        allocation[min_i][min_j] = min_allocation;
        supply[min_i] -= min_allocation;
        demand[min_j] -= min_allocation;
        costs[min_i][min_j] = INT_MAX;
        total_supply -= min_allocation;
        total_demand -= min_allocation;
    }

    return allocation;
}

// 1. найти все незаполненые элементы опорного плана
vector<pair<int, int>> findZeros(const vector<vector<int>>& matrix) {
    vector<pair<int, int>> zero_indices;

    int rows = matrix.size();
    if (rows == 0) return zero_indices;
    int cols = matrix[0].size();

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (matrix[i][j] == 0) {
                zero_indices.push_back(make_pair(i, j));
            }
        }
    }

    return zero_indices;
}

// 2. построить для каждого такого элемента свой кратчайший прямоугольный замклуный контур по заполненым элементам опорного плана 
//vector<pair<int, int>> findingContour(){}

// 3. каждой вершине контура присваивается коэффициент, рав­ный соответствующему значению элемента из матрицы стоимости
vector<int> switchElements(const vector<vector<int>>& costs, vector<pair<int, int>>& zero_elements) {
    vector<int> countuor;

    for (int i = 0; i < zero_elements.size(); i++) {
        countuor.push_back(costs[zero_elements[i].first][zero_elements[i].second]);
    }

    return countuor;
}

// 4. каждому коэффициенту в вершинах контура строго пооче­редно присваивается знак « + » или « - », начиная с пустой клетки
void assigningSign(vector<int>& countuor_new) {
    for (int i = 0; i < countuor_new.size(); i++)
        if (i % 2 != 0)
            countuor_new[i] *= -1;
}

// 5. алгебраическое суммирование коэффициентов по всему контуру для каждого свой
int summationCoefficients(vector<int>& countuor_new) {
    int sum = 0;
    for (int i = 0; i < countuor_new.size(); i++)
        sum += countuor_new[i];
    return sum;
}

// 6. проверить на оптимальность (алгебраические суммы по всем кон­турам должны быть положительными или равными нулю)
bool checkOptimal(int sum) {
    return sum >= 0;
}

// перераспределение: 
// выбирается контур, для которого нарушено условие опти­мальности. Если транспортная задача решалась на отыска­ние минимума затрат, то выбирается контур с 
// отрицатель­ным значением алгебраической суммы. Если таких контуров несколько, то выбирается тот, у которого большая по моду­лю отрицательная алгебраическая сумма;
// в вершинах выбранного контура расставляются фактические перевозки с теми знаками, которые были указаны при вы­числении коэффициентов. Рассматриваются вершины толь­ко с отрицательными значениями.Выбирается вершина сминимальным по модулю отрицательным значением.Значе­ние этой вершины алгебраически вычитается из всех вер­шин контура;
// проверяется сохранение баланса перевозок по строкам и столбцам;
void redistribution(vector<vector<int>>& optimal, const vector<pair<int, int>>& countuor) {
    int minNegative = numeric_limits<int>::max();
    pair<int, int> minCell;
    for (const auto& cell : countuor) {
        int row = cell.first;
        int col = cell.second;
        if (optimal[row][col] < 0 && optimal[row][col] < minNegative) {
            minNegative = optimal[row][col];
            minCell = cell;
        }
    }

    optimal[minCell.first][minCell.second] -= minNegative;
}


int sumOptimal(const vector<vector<int>>& costs, vector<vector<int>>& optimal) {
    vector<pair<int, int>> zero_elements;
    cout << "Пустые элементы опорного плана:" << endl;
    zero_elements = findZeros(optimal);

    printPairs(zero_elements);

    int sum_for_optimal = 0;
    vector<pair<int, int>> countuor;
    vector<int> sum_countuor;

    for (int i = 0; i < zero_elements.size(); i++) {
        vector<int> countuor_new;
        int sum = 0;

        switch (i) {
        case 0:countuor = { {0, 3}, {1, 3}, {1, 0}, {0, 0} }; break;
        case 1:countuor = { {1, 1}, {1, 0}, {0, 0}, {0, 1} }; break;
        case 2:countuor = { {1, 2}, {1, 0}, {0, 0}, {0, 2} }; break;
        case 3:countuor = { {2, 0}, {0, 0}, {0, 1}, {2, 1} }; break;
        case 4:countuor = { {2, 2}, {2, 1}, {0, 1}, {0, 2} }; break;
        case 5:countuor = { {2, 3}, {2, 1}, {0, 1}, {0, 0}, {1, 0}, {1, 3} }; break;
        default:break;
        }

        cout << endl << "Контур для элемента [" << zero_elements[i].first + 1 << ";" << zero_elements[i].second + 1 << "]:" << endl;
        printPairs(countuor);

        countuor_new = switchElements(costs, countuor);
        cout << "Присваивается коэффициент: ";
        printVector(countuor_new);

        assigningSign(countuor_new);
        cout << "Замена знака: ";
        printVector(countuor_new);

        sum = summationCoefficients(countuor_new);
        cout << "Сумма контура: " << sum << endl;
        sum_for_optimal += sum;
        sum_countuor.push_back(sum);

        if (!checkOptimal(sum)) {
            cout << "Перераспределение для контура: ";
            printPairs(countuor);
            redistribution(optimal, countuor);
            sumOptimal(costs, optimal);
        }
    }


    cout << "Оптимальность: " << sum_for_optimal << endl;
    return sum_for_optimal;
}

void tests() {
    vector<vector<int>> costs = { {6, 5, 8, 7},{3, 6, 4, 2},{9, 1, 3, 6} };
    vector<int> supply = { 14, 12, 8 };
    vector<int> demand = { 10, 14, 6, 4 };

    vector<vector<int>> oporn_plan;
    int sum_for_optimal = 0;

    cout << "Опорный план:" << endl;
    oporn_plan = minCostMethod(costs, supply, demand);

    printMatrix(oporn_plan);

    cout << "" << endl;
    sum_for_optimal = sumOptimal(costs, oporn_plan);
}

int main() {
    setlocale(LC_ALL, "Russian");

    tests();

    return 0;
}
