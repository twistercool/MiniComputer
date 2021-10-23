#include "../headers/header.h"


using namespace std;


class LogicGate {
    private: 
        map<vector<bool>, bool> table;

    public:
        LogicGate() {}
        LogicGate(map<vector<bool>, bool> input) {
            table = input;
        }

        bool getOutput(vector<bool> input) {
            return table[input];
        }
};

const LogicGate yes_Gate(map<vector<bool>, bool> {{{1}, 1}});
const LogicGate not_Gate(map<vector<bool>, bool> {{{0}, 1}});
const LogicGate and_Gate(map<vector<bool>, bool> {{{1, 1}, 1}});
const LogicGate or_Gate(map<vector<bool>, bool> {{{1, 1}, 1}, {{1, 0}, 1}, {{0, 1}, 1}});
const LogicGate xor_Gate(map<vector<bool>, bool> {{{1, 0}, 1}, {{0, 1}, 1}});
const LogicGate nor_Gate(map<vector<bool>, bool> {{{0, 0}, 1}});
const LogicGate nand_Gate(map<vector<bool>, bool> {{{1, 0}, 1}, {{0, 1}, 1}, {{0, 0}, 1}});
const LogicGate xnor_Gate(map<vector<bool>, bool> {{{1, 1}, 1}, {{1, 0}, 1}, {{0, 1}, 1}});
const LogicGate triple_and_Gate(map<vector<bool>, bool> {{{1, 1, 1}, 1}}); //maps default to 0 if the value is not found
const LogicGate quad_and_Gate(map<vector<bool>, bool> {{{1, 1, 1, 1}, 1}}); //maps default to 0 if the value is not found
const LogicGate quintuple_and_Gate(map<vector<bool>, bool> {{{1, 1, 1, 1, 1}, 1}}); //maps default to 0 if the value is not found
const LogicGate triple_nor_Gate(map<vector<bool>, bool>{{{0, 0, 0}, 1}});
const LogicGate quad_nor_Gate(map<vector<bool>, bool>{{{0, 0, 0, 0}, 1}});

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

// Takes 3 bits as inputs, 2 bits to sum up and a carry bit
// It outputs 2 bits, the sum and a carry signal.
// 0-0-0 will output 0-0
// 1-0-0, 0-1-0 and 0-0-1 will output 1-0
// 0-1-1, 1-0-1 and 1-1-0 will output 0-1
// 1-1-1 will output 1-1
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

/*
This class emulates the inner workings of the 74181 ALU.
It takes 3 4-bit numbers and 2 bit signals, M and C.

C is the carry bit.

M is the bit which chooses between logic and arithmetic operations (1 => logic, 0 => arithmetic)

The first two 4-bit inputs are A and B, which represent unsigned integers.

The third 4-bit input, S, specifies the operation. 


*/
class APU_Emulator {
    private:
        Circuit inputS0 = Circuit(yes_Gate, vector<bool>{0});
        Circuit inputS1 = Circuit(yes_Gate, vector<bool>{0});
        Circuit inputS2 = Circuit(yes_Gate, vector<bool>{0});
        Circuit inputS3 = Circuit(yes_Gate, vector<bool>{0});

        Circuit inputA0 = Circuit(yes_Gate, vector<bool>{0});
        Circuit inputA1 = Circuit(yes_Gate, vector<bool>{0});
        Circuit inputA2 = Circuit(yes_Gate, vector<bool>{0});
        Circuit inputA3 = Circuit(yes_Gate, vector<bool>{0});

        Circuit inputB0 = Circuit(yes_Gate, vector<bool>{0});
        Circuit inputB1 = Circuit(yes_Gate, vector<bool>{0});
        Circuit inputB2 = Circuit(yes_Gate, vector<bool>{0});
        Circuit inputB3 = Circuit(yes_Gate, vector<bool>{0});

        Circuit inputM = Circuit(yes_Gate, vector<bool>{0});
        Circuit inputC = Circuit(yes_Gate, vector<bool>{0});


        //first row of NOT gates: starts from the "bottom" aka next to the C and M inputs on the ALU blueprint 
        Circuit notGateRow0_0 = Circuit(not_Gate, vector<Circuit*>{&inputM});
        Circuit notGateRow0_1 = Circuit(not_Gate, vector<Circuit*>{&inputB0});
        Circuit notGateRow0_2 = Circuit(not_Gate, vector<Circuit*>{&inputB1});
        Circuit notGateRow0_3 = Circuit(not_Gate, vector<Circuit*>{&inputB2});
        Circuit notGateRow0_4 = Circuit(not_Gate, vector<Circuit*>{&inputB3});


        //second row: and gates
        Circuit andGateRow1_0 = Circuit(and_Gate, vector<Circuit*>{&inputA0, &inputS0});
        Circuit andGateRow1_1 = Circuit(and_Gate, vector<Circuit*>{&inputS1, &notGateRow0_1});
        Circuit andGateRow1_2 = Circuit(triple_and_Gate, vector<Circuit*>{&notGateRow0_1, &inputS2, &inputA0});
        Circuit andGateRow1_3 = Circuit(triple_and_Gate, vector<Circuit*>{&inputA0, &inputS3, &inputB0});

        Circuit andGateRow1_4 = Circuit(and_Gate, vector<Circuit*>{&inputB1, &inputS0});
        Circuit andGateRow1_5 = Circuit(and_Gate, vector<Circuit*>{&inputS1, &notGateRow0_2});
        Circuit andGateRow1_6 = Circuit(triple_and_Gate, vector<Circuit*>{&notGateRow0_2, &inputS2, &inputA1});
        Circuit andGateRow1_7 = Circuit(triple_and_Gate, vector<Circuit*>{&inputA1, &inputS3, &inputB1});

        Circuit andGateRow1_8 = Circuit(and_Gate, vector<Circuit*>{&inputB2, &inputS0});
        Circuit andGateRow1_9 = Circuit(and_Gate, vector<Circuit*>{&inputS1, &notGateRow0_3});
        Circuit andGateRow1_10 = Circuit(triple_and_Gate, vector<Circuit*>{&notGateRow0_3, &inputS2, &inputA2});
        Circuit andGateRow1_11 = Circuit(triple_and_Gate, vector<Circuit*>{&inputA2, &inputS3, &inputB2});

        Circuit andGateRow1_12 = Circuit(and_Gate, vector<Circuit*>{&inputB3, &inputS0});
        Circuit andGateRow1_13 = Circuit(and_Gate, vector<Circuit*>{&inputS1, &notGateRow0_4});
        Circuit andGateRow1_14 = Circuit(triple_and_Gate, vector<Circuit*>{&notGateRow0_4, &inputS2, &inputA3});
        Circuit andGateRow1_15 = Circuit(triple_and_Gate, vector<Circuit*>{&inputA3, &inputS3, &inputB3});


        //third row: nor gates
        Circuit norGateRow2_0 = Circuit(triple_nor_Gate, vector<Circuit*>{&inputA0, &andGateRow1_0, &andGateRow1_1});
        Circuit norGateRow2_1 = Circuit(nor_Gate, vector<Circuit*>{&andGateRow1_2, &andGateRow1_3});
        Circuit norGateRow2_2 = Circuit(triple_nor_Gate, vector<Circuit*>{&inputA1, &andGateRow1_4, &andGateRow1_5});
        Circuit norGateRow2_3 = Circuit(nor_Gate, vector<Circuit*>{&andGateRow1_6, &andGateRow1_7});

        Circuit norGateRow2_4 = Circuit(triple_nor_Gate, vector<Circuit*>{&inputA2, &andGateRow1_8, &andGateRow1_9});
        Circuit norGateRow2_5 = Circuit(nor_Gate, vector<Circuit*>{&andGateRow1_10, &andGateRow1_11});
        Circuit norGateRow2_6 = Circuit(triple_nor_Gate, vector<Circuit*>{&inputA3, &andGateRow1_12, &andGateRow1_13});
        Circuit norGateRow2_7 = Circuit(nor_Gate, vector<Circuit*>{&andGateRow1_14, &andGateRow1_15});


        //fourth row: 
        Circuit nandGateRow3_0 = Circuit(nand_Gate, vector<Circuit*>{&inputC, &notGateRow0_0});
        Circuit xorGateRow3_1 = Circuit(xor_Gate, vector<Circuit*>{&norGateRow2_0, &norGateRow2_1});
        Circuit andGateRow3_2 = Circuit(and_Gate, vector<Circuit*>{&notGateRow0_0, &norGateRow2_0});
        Circuit andGateRow3_3 = Circuit(triple_and_Gate, vector<Circuit*>{&notGateRow0_0, &norGateRow2_1, &inputC});
        Circuit xorGateRow3_4 = Circuit(xor_Gate, vector<Circuit*>{&norGateRow2_2, &norGateRow2_3});
        Circuit andGateRow3_5 = Circuit(and_Gate, vector<Circuit*>{&notGateRow0_0, &norGateRow2_2});
        Circuit andGateRow3_6 = Circuit(triple_and_Gate, vector<Circuit*>{&notGateRow0_0, &norGateRow2_0, &norGateRow2_3});
        Circuit andGateRow3_7 = Circuit(quad_and_Gate, vector<Circuit*>{&notGateRow0_0, &inputC, &norGateRow2_1, &norGateRow2_3});
        Circuit xorGateRow3_8 = Circuit(xor_Gate, vector<Circuit*>{&norGateRow2_4, &norGateRow2_5});
        Circuit andGateRow3_9 = Circuit(and_Gate, vector<Circuit*>{&notGateRow0_0, &norGateRow2_4});
        Circuit andGateRow3_10 = Circuit(triple_and_Gate, vector<Circuit*>{&notGateRow0_0, &norGateRow2_2, &norGateRow2_5});
        Circuit andGateRow3_11 = Circuit(quad_and_Gate, vector<Circuit*>{&notGateRow0_0, &norGateRow2_0, &norGateRow2_3, &norGateRow2_5});
        Circuit andGateRow3_12 = Circuit(quintuple_and_Gate, vector<Circuit*>{&notGateRow0_0, &inputC, &norGateRow2_1, &norGateRow2_3, &norGateRow2_5});
        Circuit xorGateRow3_13 = Circuit(xor_Gate, vector<Circuit*>{&norGateRow2_6, &norGateRow2_7});
        Circuit nandGateRow3_14_0 = Circuit(quad_and_Gate, vector<Circuit*>{&norGateRow2_1, &norGateRow2_3, &norGateRow2_5, &norGateRow2_7});
        Circuit nandGateRow3_14_output = Circuit(not_Gate, vector<Circuit*>{&nandGateRow3_14_0});
        Circuit nandGateRow3_15_0 = Circuit(quintuple_and_Gate, vector<Circuit*>{&inputC, &norGateRow2_1, &norGateRow2_3, &norGateRow2_5, &norGateRow2_7});
        Circuit nandGateRow3_15_output = Circuit(not_Gate, vector<Circuit*>{&nandGateRow3_15_0});
        Circuit andGateRow3_16 = Circuit(quad_and_Gate, vector<Circuit*>{&norGateRow2_0, &norGateRow2_3, &norGateRow2_2, &norGateRow2_7});
        Circuit andGateRow3_17 = Circuit(triple_and_Gate, vector<Circuit*>{&norGateRow2_2, &norGateRow2_5, &norGateRow2_7});
        Circuit andGateRow3_18 = Circuit(and_Gate, vector<Circuit*>{&norGateRow2_4, &norGateRow2_7});


        //fifth row:
        Circuit norGateRow4_0 = Circuit(nor_Gate, vector<Circuit*>{&andGateRow3_2, &andGateRow3_3});
        Circuit norGateRow4_1 = Circuit(triple_nor_Gate, vector<Circuit*>{&andGateRow3_5, &andGateRow3_6, &andGateRow3_7});
        Circuit norGateRow4_2 = Circuit(quad_nor_Gate, vector<Circuit*>{&andGateRow3_9, &andGateRow3_10, &andGateRow3_11, &andGateRow3_12});
        Circuit norGateRow4_3 = Circuit(quad_nor_Gate, vector<Circuit*>{&andGateRow3_16, &andGateRow3_17, &andGateRow3_18, &norGateRow2_6});
        
        //sixth row:
        Circuit xorGateRow5_0 = Circuit(xor_Gate, vector<Circuit*>{&nandGateRow3_0, &xorGateRow3_1});
        Circuit xorGateRow5_1 = Circuit(xor_Gate, vector<Circuit*>{&norGateRow4_0, &xorGateRow3_4});
        Circuit xorGateRow5_2 = Circuit(xor_Gate, vector<Circuit*>{&norGateRow4_1, &xorGateRow3_8});
        Circuit xorGateRow5_3 = Circuit(xor_Gate, vector<Circuit*>{&norGateRow4_2, &xorGateRow3_13});
        Circuit notGateRow5_4_input_0 = Circuit(not_Gate, vector<Circuit*>{&nandGateRow3_14_output});
        Circuit notGateRow5_4_input_1 = Circuit(not_Gate, vector<Circuit*>{&norGateRow4_3});
        Circuit orGateRow5_4 = Circuit(or_Gate, vector<Circuit*>{&notGateRow5_4_input_0, &notGateRow5_4_input_1});

        //seventh row:
        Circuit andGateRow6_0 = Circuit(quad_and_Gate, vector<Circuit*>{&xorGateRow5_0, &xorGateRow5_1, &xorGateRow5_2, &xorGateRow5_3});

        //outputs
        Circuit output_F_0 = Circuit(yes_Gate, vector<Circuit*>{&xorGateRow5_0});
        Circuit output_F_1 = Circuit(yes_Gate, vector<Circuit*>{&xorGateRow5_1});
        Circuit output_A_equal_B = Circuit(yes_Gate, vector<Circuit*>{&andGateRow6_0});
        Circuit output_F_2 = Circuit(yes_Gate, vector<Circuit*>{&xorGateRow5_2});
        Circuit output_F_3 = Circuit(yes_Gate, vector<Circuit*>{&xorGateRow5_3});

        Circuit output_P = Circuit(yes_Gate, vector<Circuit*>{&nandGateRow3_14_output});
        Circuit output_C_n_plus_4 = Circuit(yes_Gate, vector<Circuit*>{&orGateRow5_4});
        Circuit output_G = Circuit(yes_Gate, vector<Circuit*>{&norGateRow4_3});

    public:
        APU_Emulator() {}
        
        void computeOutput(string inputA, string inputB, string inputS, bool input_M, bool input_C = 0) {
            inputA0.changeValues(vector<bool>{(bool)((int)inputA[0] - 48)});
            inputA1.changeValues(vector<bool>{(bool)((int)inputA[1] - 48)});
            inputA2.changeValues(vector<bool>{(bool)((int)inputA[2] - 48)});
            inputA3.changeValues(vector<bool>{(bool)((int)inputA[3] - 48)});
            
            inputB0.changeValues(vector<bool>{(bool)((int)inputB[0] - 48)});            
            inputB1.changeValues(vector<bool>{(bool)((int)inputB[1] - 48)});
            inputB2.changeValues(vector<bool>{(bool)((int)inputB[2] - 48)});
            inputB3.changeValues(vector<bool>{(bool)((int)inputB[3] - 48)});
            
            inputS0.changeValues(vector<bool>{(bool)((int)inputS[0] - 48)});            
            inputS1.changeValues(vector<bool>{(bool)((int)inputS[1] - 48)});
            inputS2.changeValues(vector<bool>{(bool)((int)inputS[2] - 48)});
            inputS3.changeValues(vector<bool>{(bool)((int)inputS[3] - 48)});

            inputM.changeValues(vector<bool>{input_M});
            inputC.changeValues(vector<bool>{input_C});
        }

        void printOutput(string inputA, string inputB, string inputS, bool input_M, bool input_C = 0) {
            computeOutput(inputA, inputB, inputS, input_M, input_C);
            cout << "Output F: " << output_F_0.getOutput() << output_F_1.getOutput() << output_F_2.getOutput() << output_F_3.getOutput() << endl;
            cout << "Output A=B: " << output_A_equal_B.getOutput() << endl;
            cout << "Output C_n+4: " << output_C_n_plus_4.getOutput() << endl;
            cout << "Output G: " << output_G.getOutput() << endl;
            cout << "Output P: " << output_P.getOutput() << endl;
        }

        


};

int main(int argc, char * argv[]) {
    APU_Emulator emu;
    emu.printOutput("1111", "1011", "1010", 0, 0);
    
}
