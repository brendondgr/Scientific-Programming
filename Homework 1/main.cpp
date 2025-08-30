#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath> // For std::sqrt
#include <numeric> // For std::accumulate and std::inner_product
#include <algorithm> // For std::minmax_element
#include <iomanip> // For std::setprecision
#include "includes/json.hpp"

// Function to read CSV file
std::vector<std::vector<std::string>> readCSV(const std::string& filename) {
    std::vector<std::vector<std::string>> data;
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open " << filename << std::endl;
        return data;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::vector<std::string> row;
        std::istringstream sstream(line);
        std::string cell;
        while (std::getline(sstream, cell, ',')) {
            row.push_back(cell);
        }
        data.push_back(row);
    }
    return data;
}

// Function to compute mean and standard deviation
std::pair<double, double> computeMeanAndStdDev(const std::vector<double>& data) {
    if (data.empty()) {
        return {0.0, 0.0};
    }
    double mean = std::accumulate(data.begin(), data.end(), 0.0) / data.size();
    double sq_sum = std::inner_product(data.begin(), data.end(), data.begin(), 0.0);
    double stddev = std::sqrt(sq_sum / data.size() - mean * mean);
    return {mean, stddev};
}

// Function to normalize a column to [0,1]
void normalizeColumn(std::vector<double>& column) {
    if (column.empty()) {
        return;
    }
    auto [minIt, maxIt] = std::minmax_element(column.begin(), column.end());
    double minVal = *minIt;
    double maxVal = *maxIt;

    if (maxVal == minVal) { // Avoid division by zero if all values are the same
        for (auto& value : column) {
            value = 0.0;
        }
    } else {
        for (auto& value : column) {
            value = (value - minVal) / (maxVal - minVal);
        }
    }
}

// Function to write CSV file
void writeCSV(const std::string& filename, const std::vector<std::string>& header, const std::vector<std::vector<double>>& data) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open " << filename << " for writing" << std::endl;
        return;
    }

    // Write header
    for (size_t i = 0; i < header.size(); ++i) {
        file << header[i];
        if (i < header.size() - 1) {
            file << ",";
        }
    }
    file << "\n";

    // Write data
    for (size_t r = 0; r < data[0].size(); ++r) { // Iterate over rows
        for (size_t c = 0; c < data.size(); ++c) { // Iterate over columns
            file << data[c][r];
            if (c < data.size() - 1) {
                file << ",";
            }
        }
        file << "\n";
    }
}

int main(int argc, char* argv[]) {
    std::string data_directory;
    std::string json_file;

    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <data_directory> <json_file>" << std::endl;
        return 1;
    }

    data_directory = argv[1];
    json_file = argv[2];

    std::cout << "Data Directory Specified: " << data_directory << std::endl;
    std::cout << "Location of JSON File Specified: " << json_file << std::endl;

    // Parse JSON file
    std::ifstream ifs(json_file);
    if (!ifs.is_open()) {
        std::cerr << "Error: Unable to open JSON file " << json_file << std::endl;
        return 1;
    }
    nlohmann::json json_data;
    ifs >> json_data;

    // Process each CSV file specified in the JSON
    for (auto const& [key, val] : json_data.items()) {
        std::string csv_file_name = val["file_name"];
        int lines_to_read = val["lines_to_read"];
        std::vector<std::string> column_names = val["columns"].get<std::vector<std::string>>();

        std::string full_csv_path = data_directory + "/" + csv_file_name;
        std::cout << "Processing file: " << full_csv_path << " with " << lines_to_read << " lines." << std::endl;

        std::vector<std::vector<std::string>> csv_data = readCSV(full_csv_path);

        if (csv_data.empty()) {
            std::cerr << "Error: No data read from " << full_csv_path << std::endl;
            continue;
        }

        // Map column names to their indices in the CSV header
        std::vector<std::string> csv_header = csv_data[0];
        std::unordered_map<std::string, size_t> header_index;
        for (size_t i = 0; i < csv_header.size(); ++i) {
            header_index[csv_header[i]] = i;
        }
        csv_data.erase(csv_data.begin()); // Remove header row

        // Store numerical data for calculations, sized by columns from JSON
        std::vector<std::vector<double>> numerical_data(column_names.size());

        // Only process the columns specified in parameters.json
        for (const auto& row : csv_data) {
            for (size_t i = 0; i < column_names.size(); ++i) {
                const std::string& col_name = column_names[i];
                auto it = header_index.find(col_name);
                if (it != header_index.end() && it->second < row.size()) {
                    try {
                        numerical_data[i].push_back(std::stod(row[it->second]));
                    } catch (const std::invalid_argument& e) {
                        std::cerr << "Warning: Could not convert \"" << row[it->second] << "\" to double in file " << csv_file_name << ", skipping value." << std::endl;
                    } catch (const std::out_of_range& e) {
                        std::cerr << "Warning: Value \"" << row[it->second] << "\" out of range in file " << csv_file_name << ", skipping value." << std::endl;
                    }
                } else {
                    // Column not found in CSV header
                    std::cerr << "Warning: Column '" << col_name << "' not found in CSV file '" << csv_file_name << "', skipping column for this row." << std::endl;
                }
            }
        }

        // Compute mean and standard deviation for each column
        std::cout << std::fixed << std::setprecision(2);
        for (size_t i = 0; i < numerical_data.size(); ++i) {
            if (!numerical_data[i].empty()) {
                auto [mean, stddev] = computeMeanAndStdDev(numerical_data[i]);
                std::cout << "Column " << column_names[i] << ": Mean = " << mean << ", StdDev = " << stddev << std::endl;
                // Normalize the column
                normalizeColumn(numerical_data[i]);
            }
        }

        // Write summary statistics to a CSV file
        std::string summary_file_name = csv_file_name.substr(0, csv_file_name.find(".csv")) + "_summary.csv";
        std::string full_summary_path = data_directory + "/" + summary_file_name;
        std::ofstream summary_file(full_summary_path);
        if (!summary_file.is_open()) {
            std::cerr << "Error: Unable to open summary file " << full_summary_path << " for writing." << std::endl;
        } else {
            // Write CSV header
            summary_file << "column_name,mean,stddev\n";
            // summary_file << std::fixed << std::setprecision(2);
            for (size_t i = 0; i < numerical_data.size(); ++i) {
                if (!numerical_data[i].empty()) {
                    auto [mean, stddev] = computeMeanAndStdDev(numerical_data[i]);
                    summary_file << column_names[i] << "," << mean << "," << stddev << "\n";
                }
            }
            summary_file.close();
            std::cout << "Summary statistics written to: " << full_summary_path << std::endl;
        }

        // Write transformed data to a new CSV file
        std::string transformed_file_name = csv_file_name.substr(0, csv_file_name.find(".csv")) + "_transformed.csv";
        std::string full_transformed_path = data_directory + "/" + transformed_file_name;
        writeCSV(full_transformed_path, column_names, numerical_data);
        std::cout << "Transformed data written to: " << full_transformed_path << std::endl;
    }

    return 0;
}