#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <bitset>

class BranchPredictor {
private:
    std::bitset<10> bhr; // 8-bit BHR for simplicity
    std::vector<bool> pht; // PHT with 2^8 entries

public:
    BranchPredictor() : pht(1 << 8, false) { // Initialize all entries in PHT to 'false' (not taken)
        // Optionally, you can also explicitly set the BHR to 0 if needed
        bhr.reset(); // Set all bits of BHR to 0
    }

    bool predict(unsigned int address) {
        unsigned int index = bhr.to_ulong(); // Use BHR as index
        return pht[index]; // Predict based on PHT entry
    }

    void update(bool actualOutcome) {
        // Update PHT
        unsigned int index = bhr.to_ulong();
        pht[index] = actualOutcome;

        // Update BHR
        bhr <<= 1;
        bhr.set(0, actualOutcome);
    }
};

int main() {
    BranchPredictor predictor;
    std::ifstream traceFile("gccSmall.trace");
    std::string line;
    unsigned int address;
    char outcome;
    int totalPredictions = 0, correctPredictions = 0;

    while (std::getline(traceFile, line)) {
        std::istringstream iss(line);
        std::string outcomes;
        if (!(iss >> std::hex >> address >> outcomes)) {
            std::cerr << "Error reading line: " << line << std::endl;
            continue; // Or handle the error as needed
        }

        for (char outcome : outcomes) {
            bool actualOutcome = (outcome == '+');
            bool predictedOutcome = predictor.predict(address);

            if (actualOutcome == predictedOutcome) {
                correctPredictions++;
            }

            predictor.update(actualOutcome);
            totalPredictions++;
        }
    }

    std::cout << "Prediction Accuracy: " 
              << static_cast<double>(correctPredictions) / totalPredictions * 100 
              << "%" << std::endl;

    return 0;
}
