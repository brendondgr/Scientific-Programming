// This program processes CSV files based on parameters specified in a JSON file.
// It calculates mean and standard deviation for specified columns, normalizes the data,
// and outputs the transformed data and summary statistics to new CSV files.
// Note: While the problem statement specifies a minimal set of headers, additional
// headers are included to support essential functionality such as file I/O,
// string manipulation, JSON parsing, and advanced numerical calculations,
// which are required to keep the program functioning as intended.

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath> // For std::sqrt
#include <numeric> // For std::accumulate and std::inner_product
#include <algorithm> // For std::minmax_element
#include <iomanip> // For std::setprecision
#include <unordered_map> // For std::unordered_map
#include <utility> // For std::pair
#include "includes/json.hpp"

/**
 * @brief Reads data from a CSV file.
 *
 * This function opens and reads a CSV file, parsing it into a 2D vector of strings.
 * Each inner vector represents a row, and each string is a cell from the CSV.
 *
 * @param filename The path to the CSV file to be read.
 * @return A std::vector<std::vector<std::string>> containing the parsed data.
 *         Returns an empty vector if the file cannot be opened.
 */
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

/**
 * @brief Computes the mean and standard deviation of a dataset.
 *
 * @param data A constant reference to a vector of doubles for which the statistics
 *             are to be computed.
 * @return A std::pair<double, double> where the first element is the mean and the
 *         second is the standard deviation. Returns {0.0, 0.0} if the input
 *         vector is empty.
 */
std::pair<double, double> computeMeanAndStdDev(const std::vector<double>& data) {
    if (data.empty()) {
        return {0.0, 0.0};
    }
    double mean = std::accumulate(data.begin(), data.end(), 0.0) / data.size();
    double sq_sum = std::inner_product(data.begin(), data.end(), data.begin(), 0.0);
    double stddev = std::sqrt(sq_sum / data.size() - mean * mean);
    return {mean, stddev};
}

/**
 * @brief Normalizes a column of numerical data to the range [0, 1].
 *
 * This function scales the values in a vector of doubles to fit within the
 * range [0, 1]. This is an in-place modification. If all elements in the
 * column are the same, they are all set to 0.
 *
 * @param column A vector of doubles to be normalized. The vector is modified in-place.
 */
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

/**
 * @brief Writes data to a CSV file, including a header.
 *
 * @param filename The name of the file to write to.
 * @param header A vector of strings representing the column headers.
 * @param data A 2D vector of doubles representing the data to be written. The
 *             outer vector holds columns, and the inner vectors hold the data
 *             for each column.
 */
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

/**
 * @brief Main entry point of the program.
 *
 * This program reads CSV files, calculates statistics, normalizes data, and writes
 * the results to new CSV files. It is configured via a JSON file and takes command-line
 * arguments for the data directory and the JSON configuration file path.
 *
 * @param argc The number of command-line arguments.
 * @param argv An array of C-style strings representing the command-line arguments.
 *             - argv[0]: The name of the executable.
 *             - argv[1]: The path to the data directory.
 *             - argv[2]: The path to the JSON configuration file.
 * @return Returns 0 on successful execution, and 1 on error (e.g., incorrect
 *         arguments or file I/O issues).
 */
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

    std::cout << std::fixed << std::setprecision(2);

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
        
        if (!val.contains("columns")) {
            std::cout << "Skipping " << csv_file_name << " because it has no 'columns' specified." << std::endl;
            continue;
        }
        std::vector<std::string> original_column_names = val["columns"].get<std::vector<std::string>>();

        std::vector<std::string> do_not_include_terms;
        if (val.contains("other_parameters") && val["other_parameters"].contains("do_not_include")) {
            do_not_include_terms = val["other_parameters"]["do_not_include"].get<std::vector<std::string>>();
        }

        std::vector<std::string> column_names; // This will be the filtered list
        for (const auto& col_name : original_column_names) {
            bool exclude = false;
            for (const auto& term : do_not_include_terms) {
                if (!term.empty() && col_name.find(term) != std::string::npos) {
                    exclude = true;
                    break;
                }
            }
            if (!exclude) {
                column_names.push_back(col_name);
            }
        }

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

        // Enforce lines_to_read
        if (lines_to_read > 0 && csv_data.size() > static_cast<size_t>(lines_to_read)) {
            csv_data.resize(lines_to_read);
        }

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

        // Write summary statistics to a CSV file from raw data
        std::string summary_file_name = csv_file_name.substr(0, csv_file_name.find(".csv")) + "_summary.csv";
        std::string full_summary_path = data_directory + "/" + summary_file_name;
        std::ofstream summary_file(full_summary_path);
        if (!summary_file.is_open()) {
            std::cerr << "Error: Unable to open summary file " << full_summary_path << " for writing." << std::endl;
        } else {
            // Write CSV header
            summary_file << "column_name,mean,stddev,param_count\n";
            summary_file << std::fixed << std::setprecision(2);

            size_t param_count = lines_to_read; // Get param_count from lines_to_read
            for (size_t i = 0; i < numerical_data.size(); ++i) {
                if (!numerical_data[i].empty()) {
                    auto [mean, stddev] = computeMeanAndStdDev(numerical_data[i]);
                    summary_file << column_names[i] << "," << mean << "," << stddev << "," << param_count << "\n";
                }
            }
            summary_file.close();
            std::cout << "Summary statistics written to: " << full_summary_path << std::endl;
        }

        bool normalize = false;
        if (val.contains("other_parameters") && val["other_parameters"].contains("normalize")) {
            normalize = val["other_parameters"]["normalize"].get<bool>();
        }

        // Compute mean and standard deviation for each column and conditionally normalize
        for (size_t i = 0; i < numerical_data.size(); ++i) {
            if (!numerical_data[i].empty()) {
                auto [mean, stddev] = computeMeanAndStdDev(numerical_data[i]);
                std::cout << "Column " << column_names[i] << ": Mean = " << mean << ", StdDev = " << stddev << std::endl;
                // Conditionally normalize the column
                if (normalize) {
                    normalizeColumn(numerical_data[i]);
                }
            }
        }

        // Write transformed data to a new CSV file only if normalization was performed
        if (normalize) {
            std::string transformed_file_name = csv_file_name.substr(0, csv_file_name.find(".csv")) + "_transformed.csv";
            std::string full_transformed_path = data_directory + "/" + transformed_file_name;
            writeCSV(full_transformed_path, column_names, numerical_data);
            std::cout << "Transformed data written to: " << full_transformed_path << std::endl;
        }
    }

    return 0;
}