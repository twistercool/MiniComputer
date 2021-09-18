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
    private:
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

class Adder {
    private:
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
        Adder() {}

        Adder(Circuit * input_input0, Circuit * input_input1, Circuit * input_input2) {
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

        Circuit * getOutput(int nb) {
            if (nb == 0) return &output0;
            else if (nb == 1) return &output1;
            else {
                throw std::invalid_argument("possible inputs: 0 or 1");
            }
        }
};

int main() {
    Circuit system_input0(yes_Gate, vector<bool>{0});
    Circuit system_input1(yes_Gate, vector<bool>{0});
    Circuit system_input2(yes_Gate, vector<bool>{1});
    Circuit system_input3(yes_Gate, vector<bool>{1});

    Circuit system_input4(yes_Gate, vector<bool>{1});
    Circuit system_input5(yes_Gate, vector<bool>{1});
    Circuit system_input6(yes_Gate, vector<bool>{1});
    Circuit system_input7(yes_Gate, vector<bool>{0});

    Circuit system_input8(yes_Gate, vector<bool>{0});

    Adder miniAdder0(&system_input3, &system_input7, &system_input8);

    Adder miniAdder1(&system_input2, &system_input6, miniAdder0.getOutput(1));

    Adder miniAdder2(&system_input1, &system_input5, miniAdder1.getOutput(1));

    Adder miniAdder3(&system_input0, &system_input4, miniAdder2.getOutput(1));

    // system outputs

    Circuit output0(yes_Gate, vector<Circuit*>{miniAdder3.getOutput(0)});
    Circuit output1(yes_Gate, vector<Circuit*>{miniAdder2.getOutput(0)});
    Circuit output2(yes_Gate, vector<Circuit*>{miniAdder1.getOutput(0)});
    Circuit output3(yes_Gate, vector<Circuit*>{miniAdder0.getOutput(0)});

    Circuit output4(yes_Gate, vector<Circuit*>{miniAdder3.getOutput(1)}); //overflow 

    std::cout << output0.getOutput() << " " << output1.getOutput() << " " << output2.getOutput() << " " << output3.getOutput() << "   " << output4.getOutput() << endl;    
}
