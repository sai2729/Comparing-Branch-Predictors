#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <bitset>

#include <vector>
#include <bitset>

class HybridBranchPredictor {
private:
    static const int LOCAL_HISTORY_BITS = 20;
    static const int GLOBAL_HISTORY_BITS = 20;
    static const int META_HISTORY_BITS = 20;

    std::bitset<GLOBAL_HISTORY_BITS> ghr; // Global History Register
    std::vector<unsigned int> lhr; // Local History Register
    std::vector<unsigned char> localPHT; // Local Pattern History Table
    std::vector<unsigned char> globalPHT; // Global Pattern History Table
    std::vector<unsigned char> metaPHT; // Meta Pattern History Table to choose between local and global

public:
    HybridBranchPredictor() : localPHT(1 << LOCAL_HISTORY_BITS, 0b00),
                              globalPHT(1 << GLOBAL_HISTORY_BITS, 0b00),
                              metaPHT(1 << META_HISTORY_BITS, 0b00),
                              lhr(1 << 20, 0) {}

    bool predict(unsigned int address) {
        unsigned int localHistory = lhr[address % lhr.size()];
        unsigned int localIndex = localHistory % (1 << LOCAL_HISTORY_BITS);
        unsigned int globalIndex = ghr.to_ulong();
        unsigned int metaIndex = address % metaPHT.size();

        bool localPrediction = (localPHT[localIndex] >> 1) & 1;
        bool globalPrediction = (globalPHT[globalIndex] >> 1) & 1;
        bool useLocal = (metaPHT[metaIndex] >> 1) & 1;

        return useLocal ? localPrediction : globalPrediction;
    }

    void update(unsigned int address, bool outcome) {
        // Update Local History
        unsigned int& localHistory = lhr[address % lhr.size()];
        localHistory = ((localHistory << 1) | static_cast<unsigned int>(outcome)) & ((1 << LOCAL_HISTORY_BITS) - 1);

        // Calculate Indices for Local and Global PHT
        unsigned int localIndex = localHistory;
        unsigned int globalIndex = ghr.to_ulong();
        unsigned int metaIndex = address % metaPHT.size();

        // Update PHTs
        updatePHT(localPHT, localIndex, outcome);
        updatePHT(globalPHT, globalIndex, outcome);

        // Update Global History
        ghr <<= 1;
        ghr[0] = outcome;

        // Update Meta Predictor
        bool localPrediction = (localPHT[localIndex] >> 1) & 1;
        bool globalPrediction = (globalPHT[globalIndex] >> 1) & 1;
        if (localPrediction != globalPrediction) {
            updatePHT(metaPHT, metaIndex, localPrediction == outcome);
        }
    }



private:
    void updatePHT(std::vector<unsigned char>& pht, unsigned int index, bool outcome) {
        if (outcome) {
            if (pht[index] < 0b11) pht[index]++;
        } else {
            if (pht[index] > 0b00) pht[index]--;
        }
    }
};


int main() {
    HybridBranchPredictor predictor; // Instantiate the hybrid predictor
    std::ifstream traceFile("gccSmall.trace"); // Open the trace file
    std::string line;
    unsigned int address;
    int totalPredictions = 0, correctPredictions = 0;

    // Read each line of the trace file
    while (std::getline(traceFile, line)) {
        std::istringstream iss(line);
        std::string outcomes;
        
        // Parse the address and the outcomes from the line
        if (!(iss >> std::hex >> address >> outcomes)) {
            std::cerr << "Error reading line: " << line << std::endl;
            continue;
        }

        // Process each outcome
        for (char outcome : outcomes) {
            bool actualOutcome = (outcome == '+');
            bool predictedOutcome = predictor.predict(address);

            // Compare the prediction with the actual outcome
            if (actualOutcome == predictedOutcome) {
                correctPredictions++;
            }

            // Update the predictor
            predictor.update(address, actualOutcome);
            totalPredictions++;
        }
    }

    // Calculate and display the prediction accuracy
    std::cout << "Prediction Accuracy: " 
              << static_cast<double>(correctPredictions) / totalPredictions * 100 
              << "%" << std::endl;

    return 0;
}
