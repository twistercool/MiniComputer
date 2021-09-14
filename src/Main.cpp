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
    Circuit input0(yes_Gate, vector<bool>{0});
    Circuit input1(yes_Gate, vector<bool>{0});
    Circuit input2(yes_Gate, vector<bool>{0});


    Circuit firstXorGate(xor_Gate, vector<Circuit*>{&input0, &input1});
    Circuit secondXorGate(xor_Gate, vector<Circuit*>{&firstXorGate, &input2});

    Circuit firstAndGate(and_Gate, vector<Circuit*>{&input0, &input1});
    Circuit secondAndGate(and_Gate, vector<Circuit*>{&firstXorGate, &input2}); 
    
    Circuit firstOrGate(or_Gate, vector<Circuit*>{&firstAndGate, &secondAndGate});


    Circuit output0(yes_Gate, vector<Circuit*>{&secondXorGate});
    Circuit output1(yes_Gate, vector<Circuit*>{&firstOrGate});

    vector<Circuit*> circuits0{&input0, &input1, &input2, &firstXorGate, &secondXorGate, &firstAndGate, &secondAndGate, &firstOrGate, &output0, &output1};
    vector<Circuit*> circuits1{&input0, &input1, &input2, &firstXorGate, &secondXorGate, &firstAndGate, &secondAndGate, &firstOrGate, &output0, &output1};
    vector<Circuit*> circuits2{&input0, &input1, &input2, &firstXorGate, &secondXorGate, &firstAndGate, &secondAndGate, &firstOrGate, &output0, &output1};
    vector<Circuit*> circuits3{&input0, &input1, &input2, &firstXorGate, &secondXorGate, &firstAndGate, &secondAndGate, &firstOrGate, &output0, &output1};
    
    CompoundCircuit miniAdder(3, 2, circuits0);

    cout << (*miniAdder.getOutput(0)).getOutput() << " " << (*miniAdder.getOutput(1)).getOutput() << endl;

    
    
}
