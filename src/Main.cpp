#include "../headers/header.h"


using namespace std;


class TruthTable {
    private:
        map<vector<bool>, bool> table;

    public:
        TruthTable() {}
        TruthTable(map<vector<bool>, bool> inputTable) {
            table = inputTable;
        }

        bool getValue(vector<bool> input) {
            return table[input];
        }
};


class LogicGate {
    private: 
        TruthTable table;

    public:
        LogicGate() {}
        LogicGate(TruthTable input) {
            table = input;
        }

        bool getOutput(vector<bool> input) {
            return table.getValue(input);
        }
};

LogicGate yes_Gate(TruthTable (map<vector<bool>, bool> {{vector<bool>{1}, 1}, {vector<bool>{0}, 0}}));

LogicGate not_Gate(TruthTable (map<vector<bool>, bool> {{vector<bool>{1}, 0}, {vector<bool>{0}, 1}}));

LogicGate and_Gate(TruthTable (map<vector<bool>, bool> {{vector<bool>{1, 1}, 1}, {vector<bool>{1, 0}, 0}, {vector<bool>{0, 1}, 0}, {vector<bool>{0, 0}, 0}}));

LogicGate or_Gate(TruthTable (map<vector<bool>, bool> {{vector<bool>{1, 1}, 1}, {vector<bool>{1, 0}, 1}, {vector<bool>{0, 1}, 1}, {vector<bool>{0, 0}, 0}}));

LogicGate xor_Gate(TruthTable (map<vector<bool>, bool> {{vector<bool>{1, 1}, 0}, {vector<bool>{1, 0}, 1}, {vector<bool>{0, 1}, 1}, {vector<bool>{0, 0}, 0}}));

LogicGate nor_Gate(TruthTable (map<vector<bool>, bool> {{vector<bool>{1, 1}, 0}, {vector<bool>{1, 0}, 0}, {vector<bool>{0, 1}, 0}, {vector<bool>{0, 0}, 1}}));

LogicGate nand_Gate(TruthTable (map<vector<bool>, bool> {{vector<bool>{1, 1}, 0}, {vector<bool>{1, 0}, 1}, {vector<bool>{0, 1}, 1}, {vector<bool>{0, 0}, 1}}));

LogicGate xnor_Gate(TruthTable (map<vector<bool>, bool> {{vector<bool>{1, 1}, 1}, {vector<bool>{1, 0}, 1}, {vector<bool>{0, 1}, 1}, {vector<bool>{0, 0}, 0}}));

class Circuit {
    public:
        LogicGate gate;
        vector<Circuit*> parents;
        vector<bool> staticValues;

    public:
        Circuit() {}

        Circuit(LogicGate inputGate, vector<Circuit*> inputParents) {
            gate = inputGate;
            parents = inputParents;
        }

        Circuit(LogicGate inputGate, vector<bool> inputValues) {
            gate = inputGate;
            staticValues = inputValues;
        }
        
        bool getOutput() {
            if (staticValues.size() > 0) return gate.getOutput(staticValues);
            vector<bool> inputs;
            for (Circuit* parent_ptr: parents) {
                Circuit parent = * parent_ptr;
                inputs.push_back(parent.getOutput());
            }
            return gate.getOutput(inputs);
        }

        void changeValues(vector<bool> inputValues) {
            staticValues = inputValues;
        }
};


// The first circuits have to be inputs
// The last circuits have to be outputs
class CompoundCircuit {
    private:
        int nbInputs;
        int nbOutputs;

    public:
        vector<Circuit*> components;

        CompoundCircuit(int inputs, int outputs, vector<Circuit*> inputComponents) {
            nbInputs = inputs;
            nbOutputs = outputs;
            components = inputComponents;
        }

        vector<bool> getOutput(vector<bool> input) {
            for (int i = 0; i < input.size(); i++) {
                (*components[i]).changeValues(vector<bool>{input[i]});
            }

            vector<bool> output;
            for (int i = components.size() - nbOutputs; i < components.size(); i++) {
                Circuit component = *components[i];
                output.push_back(component.getOutput());
            }
            return output;
        }

        void printOutput(vector<bool> input) {
            vector<bool> output = CompoundCircuit::getOutput(input);
            for (auto out:output) {
                cout << out << " ";
            }
            cout << endl;
        }

        Circuit * getOutput(int nb) {
            return components[components.size() - nbOutputs + nb];
        }
};


// Takes 3 bits as inputs, 2 bits to sum up and a carry bit
// It outputs 2 bits, the sum and a carry signal.
// 0-0-0 will output 0-0
// 1-0-0, 0-1-0 and 0-0-1 will output 1-0
// 0-1-1, 1-0-1 and 1-1-0 will output 0-1
// 1-1-1 will output 1-1
class Adder {
    public:
        Circuit * input_ptr0;
        Circuit * input_ptr1;
        Circuit * input_ptr2;

        Circuit firstXorGate;
        Circuit secondXorGate;

        Circuit firstAndGate;
        Circuit secondAndGate;

        Circuit firstOrGate;

        Circuit output0;
        Circuit output1;
    
    public:
        void computeOutput(Circuit * input_input0, Circuit * input_input1, Circuit * input_input2) {
            input_ptr0 = input_input0;
            input_ptr1 = input_input1;
            input_ptr2 = input_input2;


            firstXorGate = Circuit(xor_Gate, vector<Circuit*>{input_ptr0, input_ptr1});
            secondXorGate = Circuit(xor_Gate, vector<Circuit*>{&firstXorGate, input_ptr2});

            firstAndGate = Circuit(and_Gate, vector<Circuit*>{input_ptr0, input_ptr1});
            secondAndGate = Circuit(and_Gate, vector<Circuit*>{&firstXorGate, input_ptr2}); 
            
            firstOrGate = Circuit(or_Gate, vector<Circuit*>{&firstAndGate, &secondAndGate});


            output0 = Circuit(yes_Gate, vector<Circuit*>{&secondXorGate});
            output1 = Circuit(yes_Gate, vector<Circuit*>{&firstOrGate});
        }

        Adder() {
            computeOutput(nullptr, nullptr, nullptr);
        }

        Adder(Circuit * input_input0, Circuit * input_input1, Circuit * input_input2) {
            computeOutput(input_input0, input_input1, input_input2);
        }

        Circuit * getOutput(int nb) {
            if (nb == 0) return &output0;
            else if (nb == 1) return &output1;
            else {
                throw std::invalid_argument("possible inputs: 0 or 1");
            }
        }

        void printOutput(int nb) {
            cout << getOutput(nb)->getOutput() << endl;
        }
};


// Inputs: 8 or 9 bits
// First 4 bits represent number A, bits 5-8 represent number B and nit 9 represents optionally a carry
// Outputs
class Adder4Bit {
    private:
    // Input A: 4 bit number
        Circuit * inputA0;
        Circuit * inputA1;
        Circuit * inputA2;
        Circuit * inputA3;

    // Input B: 4 bit number
        Circuit * inputB0;
        Circuit * inputB1;
        Circuit * inputB2;
        Circuit * inputB3;

    //Input C: Carry signal, optional
        Circuit * inputC;
        Circuit inputCstatic = Circuit(yes_Gate, vector<bool>{0});

        Adder adder0;
        Adder adder1;
        Adder adder2;
        Adder adder3;

        Circuit output0 = Circuit(yes_Gate, vector<bool>{0});
        Circuit output1 = Circuit(yes_Gate, vector<bool>{0});
        Circuit output2 = Circuit(yes_Gate, vector<bool>{0});
        Circuit output3 = Circuit(yes_Gate, vector<bool>{0});

        //Overflow bit
        Circuit output4;

    public:
        Adder4Bit() {}

        void computeOutput(Circuit * inputA_0,
                Circuit * inputA_1,
                Circuit * inputA_2,
                Circuit * inputA_3,
                Circuit * inputB_0,
                Circuit * inputB_1,
                Circuit * inputB_2,
                Circuit * inputB_3,
                Circuit * input_C = nullptr) {
            inputA0 = inputA_0;
            inputA1 = inputA_1;
            inputA2 = inputA_2;
            inputA3 = inputA_3;

            inputB0 = inputB_0;
            inputB1 = inputB_1;
            inputB2 = inputB_2;
            inputB3 = inputB_3;

            if (input_C == nullptr) {
                inputC = &inputCstatic;
            }
            else {
                inputC = input_C;
            }

            adder0.computeOutput(inputA3, inputB3, inputC);
            adder1.computeOutput(inputA2, inputB2, adder0.getOutput(1));
            adder2.computeOutput(inputA1, inputB1, adder1.getOutput(1));
            adder3.computeOutput(inputA0, inputB0, adder2.getOutput(1));


            output0.changeValues(vector<bool>{adder3.getOutput(0)->getOutput()});
            output1.changeValues(vector<bool>{adder2.getOutput(0)->getOutput()});
            output2.changeValues(vector<bool>{adder1.getOutput(0)->getOutput()});
            output3.changeValues(vector<bool>{adder0.getOutput(0)->getOutput()});

            output4.changeValues(vector<bool>{adder3.getOutput(1)->getOutput()});
        }

        Adder4Bit(Circuit * inputA_0,
                Circuit * inputA_1,
                Circuit * inputA_2,
                Circuit * inputA_3,
                Circuit * inputB_0,
                Circuit * inputB_1,
                Circuit * inputB_2,
                Circuit * inputB_3,
                Circuit * input_C = nullptr) {
            computeOutput(inputA_0, inputA_1, inputA_2, inputA_3, inputB_0, inputB_1, inputB_2, inputB_3, input_C);
        }

        vector<Circuit*> getResult() {
            return {&output0, &output1, &output2, &output3, &output4};
        }

        void printResults() {
            vector<Circuit*> outputBits = getResult();
            cout << outputBits[0]->getOutput() << " " << outputBits[1]->getOutput() 
                << " " << outputBits[2]->getOutput() << " " << outputBits[3]->getOutput()
                << "  " << outputBits[4]->getOutput() << endl;
        }
};

int main() {
    //System inputs
    Circuit sys_input0(yes_Gate, vector<bool>{0});
    Circuit sys_input1(yes_Gate, vector<bool>{1});
    Circuit sys_input2(yes_Gate, vector<bool>{1});
    Circuit sys_input3(yes_Gate, vector<bool>{1});

    Circuit sys_input4(yes_Gate, vector<bool>{0});
    Circuit sys_input5(yes_Gate, vector<bool>{1});
    Circuit sys_input6(yes_Gate, vector<bool>{1});
    Circuit sys_input7(yes_Gate, vector<bool>{1});

    Circuit sys_input8(yes_Gate, vector<bool>{0});

    Adder4Bit bigAdder(&sys_input0, &sys_input1, &sys_input2, &sys_input3, &sys_input4, &sys_input5, &sys_input6, &sys_input7, &sys_input8);

    bigAdder.printResults();
 
}
