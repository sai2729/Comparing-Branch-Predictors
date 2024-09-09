#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <bitset>

class TwoBitBranchPredictor {
private:
    std::bitset<15> bhr; // 8-bit BHR for simplicity
    std::vector<unsigned char> pht; // 2-bit counters for each entry

public:
    TwoBitBranchPredictor() : pht(1 << 15, 0b00) {} // Initialize PHT with strongly not taken

    //GAp predict
    // bool predict(unsigned int address) {
    //     unsigned int index = bhr.to_ulong();
    //     // Predict taken if MSB of the counter is 1
    //     return (pht[index] >> 1) & 1;
    // }

    bool predict(unsigned int address) {
        // Combine address and BHR for indexing
        unsigned int combinedIndex = (address ^ bhr.to_ulong()) % pht.size();
        return (pht[combinedIndex] >> 1) & 1;
    }

    //GAp update
    // void update(bool outcome) {
    //     unsigned int index = bhr.to_ulong();
    //     if (outcome) {
    //         // If outcome is taken, increment the counter up to 0b11
    //         if (pht[index] < 0b11) pht[index]++;
    //     } else {
    //         // If outcome is not taken, decrement the counter down to 0b00
    //         if (pht[index] > 0b00) pht[index]--;
    //     }
    //     // Update BHR
    //     bhr <<= 1;
    //     bhr[0] = outcome;
    // }

    void update(unsigned int address, bool outcome) {
        unsigned int combinedIndex = (address ^ bhr.to_ulong()) % pht.size();
        // ... existing logic to update pht[combinedIndex] ...
            if (outcome) {
                // If outcome is taken, increment the counter up to 0b11
                if (pht[combinedIndex] < 0b11) pht[combinedIndex]++;
            } else {
                // If outcome is not taken, decrement the counter down to 0b00
                if (pht[combinedIndex] > 0b00) pht[combinedIndex]--;
            }
        // Update BHR
        bhr <<= 1;
        bhr[0] = outcome;
    }


};

int main() {
    TwoBitBranchPredictor predictor;
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
            continue;
        }

        for (char outcome : outcomes) {

            bool actualOutcome = (outcome == '+');
            bool predictedOutcome = predictor.predict(address);

            if (actualOutcome == predictedOutcome) {
                correctPredictions++;
            }

            predictor.update(address,actualOutcome);
            totalPredictions++;
        }
    }


    std::cout << "Prediction Accuracy: " 
              << static_cast<double>(correctPredictions) / totalPredictions * 100 
              << "%" << std::endl;

    return 0;
}
