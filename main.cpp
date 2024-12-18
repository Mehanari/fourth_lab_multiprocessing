#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <omp.h>

using namespace std;

void generateMatrix(vector<vector<int>>& matrix, int rows, int cols) {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            matrix[i][j] = rand() % 2001;
        }
    }
}

void printMatrix(const vector<vector<int>>& matrix) {
    cout << "Matrix:" << endl;
    for (const auto& row : matrix) {
        for (int val : row) {
            cout << val << " ";
        }
        cout << endl;
    }
}

int getMinColumnsSum(const vector<vector<int>>& matrix, int rows, int cols, int threads) {
    int minSum = INT_MAX;
    #pragma omp parallel for num_threads(threads) reduction(min:minSum)
    for (int j = 0; j < cols; ++j) {
        int columnSum = 0;
        for (int i = 0; i < rows; ++i) {
            columnSum += matrix[i][j];
        }
        if (columnSum < minSum) {
            minSum = columnSum;
        }
    }

    return minSum;
}

void writeToCSV(const string& filename, const vector<vector<string>>& data) {
    ofstream file(filename);

    if (!file.is_open()) {
        cerr << "Error: Could not open file " << filename << endl;
        return;
    }

    for (const auto& row : data) {
        for (size_t i = 0; i < row.size(); ++i) {
            file << row[i];
            if (i < row.size() - 1) {
                file << ";";
            }
        }
        file << "\n";
    }

    file.close();
    if (file.fail()) {
        cerr << "Error: Could not write to file " << filename << endl;
    } else {
        cout << "Data successfully written to " << filename << endl;
    }
}


struct Experiment {
    int size;
    int threads;
};

int main()
{
    int num_experiments = 8;
    Experiment* experiments = new Experiment[num_experiments];
    experiments[0] = {10000, 2};
    experiments[1] = {10000, 4};
    experiments[2] = {10000, 6};
    experiments[3] = {10000, 8};
    experiments[4] = {10000, 10};
    experiments[5] = {10000, 12};
    experiments[6] = {10000, 14};
    experiments[7] = {10000, 16};

    /*experiments[0] = {1000, 4};
    experiments[1] = {2000, 4};
    experiments[2] = {4000, 4};
    experiments[3] = {8000, 4};
    experiments[4] = {10000, 4};
    experiments[5] = {20000, 4};
    experiments[6] = {50000, 4};
    experiments[7] = {1000, 8};
    experiments[8] = {2000, 8};
    experiments[9] = {4000, 8};
    experiments[10] = {8000, 8};
    experiments[11] = {10000, 8};
    experiments[12] = {20000, 8};
    experiments[13] = {50000, 8};*/
    vector<string> multi_thread_times = {"Tp ticks"};
    vector<string> single_thread_times = {"T1 ticks"};
    vector<vector<string>> data = {multi_thread_times, single_thread_times};

    for (int experiment = 0; experiment < num_experiments; ++experiment) {
        const int size = experiments[experiment].size;
        const int rows = size;
        const int cols = size;
        vector<vector<int>> matrix(rows, vector<int>(cols));

        generateMatrix(matrix, rows, cols);

        //Calculating time for given number of threads
        double start = omp_get_wtime();
        int minSum = getMinColumnsSum(matrix, rows, cols, experiments[experiment].threads);
        double end = omp_get_wtime();

        //Calculating time for 1 thread
        double start_single_thread = omp_get_wtime();
        int minSum_single_thread = getMinColumnsSum(matrix, rows, cols, 1);
        double end_single_thread = omp_get_wtime();


        cout << "---------------------------------" << endl;
        cout << "Experiment number " << experiment + 1 << endl;
        cout << "Matrix size: " << size << endl;
        cout << "Threads used: " << experiments[experiment].threads << endl;
        cout << "Min column sum: " << minSum << endl;
        cout << "Time: " << end - start << " s" << endl;
        cout << "Min column sum (single thread): " << minSum_single_thread << endl;
        cout << "Time (single thread): " << end_single_thread - start_single_thread << " s" << endl;


        std::ostringstream oss;
        oss.precision(10); // Set the desired precision
        oss << fixed << (end - start);
        string multi_thread_time = oss.str();
        replace(multi_thread_time.begin(), multi_thread_time.end(), '.', ',');
        multi_thread_times.push_back(multi_thread_time);

        oss.str("");
        oss << fixed << (end_single_thread - start_single_thread);
        string single_thread_time = oss.str();
        replace(single_thread_time.begin(), single_thread_time.end(), '.', ',');
        single_thread_times.push_back(single_thread_time);
    }
    data = {multi_thread_times, single_thread_times};
    writeToCSV("results.csv", data);
    delete[] experiments;
    return 0;
}