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

int main() {
    Circuit system_input0(yes_Gate, vector<bool>{0});
    Circuit system_input1(yes_Gate, vector<bool>{0});
    Circuit system_input2(yes_Gate, vector<bool>{0});
    Circuit system_input3(yes_Gate, vector<bool>{1});

    Circuit system_input4(yes_Gate, vector<bool>{1});
    Circuit system_input5(yes_Gate, vector<bool>{0});
    Circuit system_input6(yes_Gate, vector<bool>{0});
    Circuit system_input7(yes_Gate, vector<bool>{1});

    Circuit system_input8(yes_Gate, vector<bool>{0});

        //first adder
    
    // Circuit input0_0(yes_Gate, vector<bool>{0});
    // Circuit input1_0(yes_Gate, vector<bool>{0});
    // Circuit input2_0(yes_Gate, vector<bool>{0});

    Circuit firstXorGate_0(xor_Gate, vector<Circuit*>{&system_input3, &system_input7});
    Circuit secondXorGate_0(xor_Gate, vector<Circuit*>{&firstXorGate_0, &system_input8});

    Circuit firstAndGate_0(and_Gate, vector<Circuit*>{&system_input3, &system_input7});
    Circuit secondAndGate_0(and_Gate, vector<Circuit*>{&firstXorGate_0, &system_input8}); 
    
    Circuit firstOrGate_0(or_Gate, vector<Circuit*>{&firstAndGate_0, &secondAndGate_0});

    Circuit output0_0(yes_Gate, vector<Circuit*>{&secondXorGate_0});
    Circuit output1_0(yes_Gate, vector<Circuit*>{&firstOrGate_0});

    vector<Circuit*> circuits0{&system_input3, &system_input7, &system_input8, &firstXorGate_0, &secondXorGate_0, &firstAndGate_0, &secondAndGate_0, &firstOrGate_0, &output0_0, &output1_0};

    CompoundCircuit miniAdder0(3, 2, circuits0);

        //second adder
    
    // Circuit input0_1(yes_Gate, vector<bool>{0});
    // Circuit input1_1(yes_Gate, vector<bool>{0});
    // Circuit input2_1

    Circuit firstXorGate_1(xor_Gate, vector<Circuit*>{&system_input2, &system_input6});
    Circuit secondXorGate_1(xor_Gate, vector<Circuit*>{&firstXorGate_1, miniAdder0.getOutput(1)});

    Circuit firstAndGate_1(and_Gate, vector<Circuit*>{&system_input2, &system_input6});
    Circuit secondAndGate_1(and_Gate, vector<Circuit*>{&firstXorGate_1, miniAdder0.getOutput(1)}); 
    
    Circuit firstOrGate_1(or_Gate, vector<Circuit*>{&firstAndGate_1, &secondAndGate_1});

    Circuit output0_1(yes_Gate, vector<Circuit*>{&secondXorGate_1});
    Circuit output1_1(yes_Gate, vector<Circuit*>{&firstOrGate_1});

    vector<Circuit*> circuits1{&system_input2, &system_input6, miniAdder0.getOutput(1), &firstXorGate_1, &secondXorGate_1, &firstAndGate_1, &secondAndGate_1, &firstOrGate_1, &output0_1, &output1_1};

    CompoundCircuit miniAdder1(3, 2, circuits1);

        //third adder

    // Circuit input0_2(yes_Gate, vector<bool>{0});
    // Circuit input1_2(yes_Gate, vector<bool>{0});
    // Circuit input2_2(yes_Gate, vector<bool>{0});

    Circuit firstXorGate_2(xor_Gate, vector<Circuit*>{&system_input1, &system_input5});
    Circuit secondXorGate_2(xor_Gate, vector<Circuit*>{&firstXorGate_2, miniAdder1.getOutput(1)});

    Circuit firstAndGate_2(and_Gate, vector<Circuit*>{&system_input1, &system_input5});
    Circuit secondAndGate_2(and_Gate, vector<Circuit*>{&firstXorGate_2, miniAdder1.getOutput(1)}); 
    
    Circuit firstOrGate_2(or_Gate, vector<Circuit*>{&firstAndGate_2, &secondAndGate_2});

    Circuit output0_2(yes_Gate, vector<Circuit*>{&secondXorGate_2});
    Circuit output1_2(yes_Gate, vector<Circuit*>{&firstOrGate_2});

    vector<Circuit*> circuits2{&system_input1, &system_input5, miniAdder1.getOutput(1), &firstXorGate_2, &secondXorGate_2, &firstAndGate_2, &secondAndGate_2, &firstOrGate_2, &output0_2, &output1_2};

    CompoundCircuit miniAdder2(3, 2, circuits2);

        //fourth adder

    // Circuit input0_3(yes_Gate, vector<bool>{0});
    // Circuit input1_3(yes_Gate, vector<bool>{0});
    // Circuit input2_3(yes_Gate, vector<bool>{0});

    Circuit firstXorGate_3(xor_Gate, vector<Circuit*>{&system_input0, &system_input4});
    Circuit secondXorGate_3(xor_Gate, vector<Circuit*>{&firstXorGate_3, miniAdder2.getOutput(1)});

    Circuit firstAndGate_3(and_Gate, vector<Circuit*>{&system_input0, &system_input4});
    Circuit secondAndGate_3(and_Gate, vector<Circuit*>{&firstXorGate_2, miniAdder2.getOutput(1)}); 
    
    Circuit firstOrGate_3(or_Gate, vector<Circuit*>{&firstAndGate_2, &secondAndGate_2});

    Circuit output0_3(yes_Gate, vector<Circuit*>{&secondXorGate_2});
    Circuit output1_3(yes_Gate, vector<Circuit*>{&firstOrGate_2});

    vector<Circuit*> circuits3{&system_input0, &system_input4, miniAdder2.getOutput(1), &firstXorGate_3, &secondXorGate_3, &firstAndGate_3, &secondAndGate_3, &firstOrGate_3, &output0_3, &output1_3};

    CompoundCircuit miniAdder3(3, 2, circuits3);

    // system outputs

    Circuit output0(yes_Gate, vector<Circuit*>{miniAdder3.getOutput(0)});
    Circuit output1(yes_Gate, vector<Circuit*>{miniAdder2.getOutput(0)});
    Circuit output2(yes_Gate, vector<Circuit*>{miniAdder1.getOutput(0)});
    Circuit output3(yes_Gate, vector<Circuit*>{miniAdder0.getOutput(0)});

    Circuit output4(yes_Gate, vector<Circuit*>{miniAdder3.getOutput(1)}); //overflow 

    std::cout << output0.getOutput() << " " << output1.getOutput() << " " << output2.getOutput() << " " << output3.getOutput() << "   " << output4.getOutput() << endl;

    
    
}
