#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <bitset>

// Define a class for Combined Branch Prediction
class CombinedBranchPredictor
{
private:
    std::bitset<15> bhr;            // 15-bit Branch History Register (BHR)
    std::vector<unsigned char> pht; // Pattern History Table (PHT) with 2-bit counters for each entry
    bool usePAp;                    // Flag to switch between Per-Address Prediction (PAp) and Global Address Prediction (GAp)

public:
    // Constructor for the CombinedBranchPredictor
    CombinedBranchPredictor(bool usePAp = true) : pht(1 << 15, 0b00), usePAp(usePAp) {}

    // Method to predict the outcome of a branch at a given address
    bool predict(unsigned int address)
    {
        // Calculate the index for PHT based on the mode (PAp or GAp)
        unsigned int index = usePAp ? (address ^ bhr.to_ulong()) % pht.size() : bhr.to_ulong();
        // Return the prediction based on the PHT entry (taken if MSB is 1)
        return (pht[index] >> 1) & 1;
    }

    // Method to update the predictor after the actual outcome is known
    void update(unsigned int address, bool outcome)
    {
        // Calculate the index for PHT update
        unsigned int index = usePAp ? (address ^ bhr.to_ulong()) % pht.size() : bhr.to_ulong();
        // Update the PHT entry based on the actual outcome
        if (outcome)
        {
            if (pht[index] < 0b11)
                pht[index]++;
        }
        else
        {
            if (pht[index] > 0b00)
                pht[index]--;
        }
        // Update the BHR with the actual outcome
        bhr <<= 1;
        bhr[0] = outcome;
    }
};

int main() {
    // Set the prediction mode: true for Per-Address Prediction (PAp), false for Global Address Prediction (GAp)
    bool usePAp = true; 
    // Create an instance of the CombinedBranchPredictor
    CombinedBranchPredictor predictor(usePAp);
    // Open the branch trace file for reading
    std::ifstream traceFile("gccSmall.trace");
    // Open a file for writing the output
    std::ofstream outputFile("output.txt");
    // Variables for storing each line of the trace file, the branch address, and prediction counts
    std::string line;
    unsigned int address;
    int totalPredictions = 0, correctPredictions = 0;

    // Read each line from the trace file
    while (std::getline(traceFile, line)) {
        std::istringstream iss(line);
        std::string outcomes;
        // Parse the branch address and outcomes from the line
        if (!(iss >> std::hex >> address >> outcomes)) {
            std::cerr << "Error reading line: " << line << std::endl;
            continue;
        }

        // Process each outcome in the line
        for (char outcome : outcomes) {
            // Determine the actual outcome (taken or not taken)
            bool actualOutcome = (outcome == '+');
            // Use the predictor to predict the outcome
            bool predictedOutcome = predictor.predict(address);

            // Compare the prediction with the actual outcome
            if (actualOutcome == predictedOutcome) {
                correctPredictions++;
            }

            // Update the predictor with the actual outcome
            predictor.update(address, actualOutcome);
            totalPredictions++;

            // Write the address, outcome, prediction, and correctness to the output file
            outputFile << std::hex << address << "\t"  // Branch address in hexadecimal
                       << (actualOutcome ? "+" : "-") << "\t"  // Actual outcome
                       << (predictedOutcome ? "+" : "-") << "\t"  // Predicted outcome
                       << (actualOutcome == predictedOutcome ? "correct" : "miss") << std::endl;  // Correct or miss
        }
    }

    // Display the overall prediction accuracy
    std::cout << "Prediction Accuracy: " 
              << static_cast<double>(correctPredictions) / totalPredictions * 100 
              << "%" << std::endl;

    // Close the input and output file streams
    traceFile.close();
    outputFile.close();
    return 0;
}