/*
 * This file is part of FuzzyLibrary thats implements common fuzzy system.
 * Copyright (C) 2014, Petr Kačer <kacerpetr@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @brief Fuzzy logic library source
 * @author Petr Kacer <kacerpetr@gmail.com>
 * @date 12.4.2014
 */
 
///////////////////////////////////////////////////
//////// Includes /////////////////////////////////
///////////////////////////////////////////////////
 
#include "fzzlib.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <float.h>

///////////////////////////////////////////////////
//////// Defines //////////////////////////////////
///////////////////////////////////////////////////

/**
 * @brief Maximal length of name of fuzzy set and input / output set string
 */
#define NAME_LENGTH 16

/**
 * @brief Maximal lengths of inferential mechanism rule string
 */
#define RULE_LENGTH 128

/**
 * @brief Maximal number of inferential mechanism rules
 */
#define MAX_RULES 256

/**
 * @brief Maximal number of inputs of fuzzy system
 */
#define MAX_INPUTS 4

/**
 * @brief Maximal number of outputs of fuzzy system
 */
#define MAX_OUTPUTS 2

/**
 * @brief Maximal number of fuzzy sets in input / output list
 */
#define MAX_FSETS 16

/**
 * @brief Maximal number of matching rules of inferential mechanism for one output
 */
#define MAX_INFRES 64

/**
 * @brief Length of buffer used for parsing inferential mechanism rule
 * Length can be eqal to NAME_LENGTH but its length must be >= 4 
 */
#define WBUF_LENGTH 16

/**
 * @brief Size of integration step during defuzzification
 * Used when searching for center of gravity of area
 */
#define COG_STEP 0.02

/*
 * @brief Uncomment if you want to see debug output
 * You will get more info about output calculation
 */
//#define DEBUG_MSG

///////////////////////////////////////////////////
//////// Data types ///////////////////////////////
///////////////////////////////////////////////////

/**
 * @brief Fuzzy set with triangular membership function
 */
typedef struct{
    double left;
    double top;
    double right;
    char name[NAME_LENGTH];
}TFuzzySet;

/**
 * @brief Set of input or output fuzzy sets
 */
typedef struct{
    TFuzzySet fSet[MAX_FSETS];
    int length;
    char name[NAME_LENGTH];
}TFcnsSet;

/**
 * @brief Fuzzy system data structure
 */
typedef struct{
    int inLen;
    int outLen;
    TFcnsSet inSet[MAX_INPUTS];
    TFcnsSet outSet[MAX_OUTPUTS];
    double input[MAX_INPUTS];
    double output[MAX_OUTPUTS];
    int ruLen;
    char rule[MAX_RULES][RULE_LENGTH];
}TFzzSystem;

/**
 * @brief Result of fuzzifycation for one fuzzy set
 */
typedef struct{
    double membership;
    int setIndex;
}TFuzzifyRes;

/**
 * @brief Output of fuzzifycation process for one input
 */
typedef struct{
    TFuzzifyRes res[MAX_FSETS];
    int length;
}TFuzzifyOut;

/**
 * @brief Result of inference of one matching rule
 */
typedef struct{
    double value;
    int fSet;
}TInfRes;

/**
 * @brief Result of inference for one output
 */
typedef struct{
    TInfRes res[MAX_INFRES];
    int length;
}TInfOut;

///////////////////////////////////////////////////
//////// Global variables /////////////////////////
///////////////////////////////////////////////////

///Fuzzy system structure
TFzzSystem fzzSystem;

///Fuzzyfication output
TFuzzifyOut fzfOut[MAX_INPUTS];

//Inferential mechanism output
TInfOut infOut[MAX_OUTPUTS];
    
///////////////////////////////////////////////////
//////// Main functions ///////////////////////////
///////////////////////////////////////////////////

void fzz_init(int inputs, int outputs){
    int i = 0;
    int j = 0;
    
    //maximum input / output count check
    assert(inputs <= MAX_INPUTS && "Required number of inputs exceeds maximum in fzz_init(...)");
    assert(outputs <= MAX_OUTPUTS && "Required number of outputs exceeds maximum in fzz_init(...)");
    
    //number of inputs and outputs
    fzzSystem.inLen = inputs;
    fzzSystem.outLen = outputs;
    
    //init of input set of fuzzy sets 
    for(i = 0; i < MAX_INPUTS; i++){
        fzzSystem.inSet[i].length = 0;
        fzzSystem.inSet[i].name[0] = '\0';
        //fuzzy set in input set
        for(j = 0; j < MAX_FSETS; j++){
            fzzSystem.inSet[i].fSet[j].left = 0;
            fzzSystem.inSet[i].fSet[j].top = 0;
            fzzSystem.inSet[i].fSet[j].right = 0;
            fzzSystem.inSet[i].fSet[j].name[0] = '\0';
        }
    }
    
    //init of output set of fuzzy sets
    for(i = 0; i < MAX_OUTPUTS; i++){
        fzzSystem.outSet[i].length = 0;
        fzzSystem.outSet[i].name[0] = '\0';
        //fuzzy sets in output set
        for(j = 0; j < MAX_FSETS; j++){
            fzzSystem.outSet[i].fSet[j].left = 0;
            fzzSystem.outSet[i].fSet[j].top = 0;
            fzzSystem.outSet[i].fSet[j].right = 0;
            fzzSystem.outSet[i].fSet[j].name[0] = '\0';
        }
    }
    
    //arrays for storing system inputs and output
    for(i = 0; i < MAX_INPUTS; i++) fzzSystem.input[i] = 0;
    for(i = 0; i < MAX_OUTPUTS; i++) fzzSystem.output[i] = 0;
    
    //list of fuzzy inference rules
    fzzSystem.ruLen = 0;
    for(i = 0; i < MAX_RULES; i++) fzzSystem.rule[i][0] = '\0';
}

void fzz_initInputFcns(int index, int length, char* name){
    assert(index < fzzSystem.inLen && "Index out of range in fzz_initInputFcns(...)");
    assert(length <= MAX_FSETS && "Required number of fuzzy sets exceeds maximum in fzz_initInputFcns(...)");
    fzzSystem.inSet[index].length = length;
    strncpy(fzzSystem.inSet[index].name, name, NAME_LENGTH);
}

void fzz_initOutputFcns(int index, int length, char* name){
    assert(index < fzzSystem.outLen && "Index out of range in fzz_initOutputFcns(...)");
    assert(length <= MAX_FSETS && "Required number of fuzzy sets exceeds maximum in fzz_initOutputFcns(...)");
    fzzSystem.outSet[index].length = length;
    strncpy(fzzSystem.outSet[index].name, name, NAME_LENGTH);
}

void fzz_setInputFcn(int index, int fcSet, double left, double top, double right, char* name){
    assert(fcSet < fzzSystem.inLen && "Input set index out of range in fzz_setInputFcn(...)");
    assert(index < fzzSystem.inSet[fcSet].length && "Index out of range in fzz_setInputFcn(...)");
    
    //membership function
    fzzSystem.inSet[fcSet].fSet[index].left = left;
    fzzSystem.inSet[fcSet].fSet[index].top = top;
    fzzSystem.inSet[fcSet].fSet[index].right = right;
    
    //fuzzy set name
    strncpy(fzzSystem.inSet[fcSet].fSet[index].name, name, NAME_LENGTH);
}

void fzz_setOutputFcn(int index, int fcSet, double left, double top, double right, char* name){
    assert(fcSet < fzzSystem.outLen && "Output set index out of range in fzz_setOutputFcn(...)");
    assert(index < fzzSystem.outSet[fcSet].length && "Index out of range in fzz_setOutputFcn(...)");
    
    //membership function
    fzzSystem.outSet[fcSet].fSet[index].left = left;
    fzzSystem.outSet[fcSet].fSet[index].top = top;
    fzzSystem.outSet[fcSet].fSet[index].right = right;
    
    //fuzzy set name
    strncpy(fzzSystem.outSet[fcSet].fSet[index].name, name, NAME_LENGTH);
}

void fzz_addRule(char* rule){
    assert(fzzSystem.ruLen < MAX_RULES && "Maximum number of inferential mechanism rules exceeded in addRule(...)");
    strncpy(fzzSystem.rule[fzzSystem.ruLen], rule, RULE_LENGTH);
    fzzSystem.ruLen++;
}

void fzz_setInput(int index, double value){
    assert(index < fzzSystem.inLen && "Index out of range in fzz_setInput(...)");
    fzzSystem.input[index] = value;
}

double fzz_getOutput(int index){
    assert(index < fzzSystem.outLen && "Index out of range in fzz_getOutput(...)");
    return fzzSystem.output[index];
}

/**
 * @brief Returns index of input with given name
 * Internal function
 * @param name name of input set of fuzzy sets
 * @return index if found, -1 if not
 */
int fzz_inputIndex(char* name){
    int i = 0;
    //search inputs
    for(i = 0; i < fzzSystem.inLen; i++){
        if(!strcmp(name, fzzSystem.inSet[i].name)) 
            return i;
    }
    //nothing found
    return -1;
}

/**
 * @brief Returns index of output with given name
 * Internal function
 * @param name name of output set of fuzzy sets
 * @return index if found, -1 if not
 */
int fzz_outputIndex(char* name){
    int i = 0;
    //search inputs
    for(i = 0; i < fzzSystem.outLen; i++){
        if(!strcmp(name, fzzSystem.outSet[i].name)) 
            return i;
    }
    //nothing found
    return -1;
}

/**
 * @brief Returns index of fuzzy set for given input
 * Internal function
 * @param name name of fuzzy set
 * @param index index of input
 * @return index if found, -1 if not
 */
int fzz_inputFSetIndex(int index, char* name){
    int i = 0;
    //search inputs
    for(i = 0; i < fzzSystem.inSet[index].length; i++){
        if(!strcmp(name, fzzSystem.inSet[index].fSet[i].name)) 
            return i;
    }
    //nothing found
    return -1;
}

/**
 * @brief Returns index of fuzzy set for given output
 * Internal function
 * @param namev name of fuzzy set
 * @param index index of output
 * @return index if found, -1 if not
 */
int fzz_outputFSetIndex(int index, char* name){
    int i = 0;
    //search inputs
    for(i = 0; i < fzzSystem.outSet[index].length; i++){
        if(!strcmp(name, fzzSystem.outSet[index].fSet[i].name)) 
            return i;
    }
    //nothing found
    return -1;  
}

/**
 * @brief Calculates fuzzified value of input with given index
 * Internal function
 * @param index index of input and index of input set
 * @param fzOut output data pointer
 */
void fzz_fuzzify(int in){
    int i = 0;
    
    //through all fuzzy sets of given output
    for(i = 0; i < fzzSystem.inSet[in].length; i++){
        //input value intersects fuzzy set
        if(fzzSystem.input[in] <= fzzSystem.inSet[in].fSet[i].left) continue;
        if(fzzSystem.input[in] >= fzzSystem.inSet[in].fSet[i].right) continue;
        
        //input intersects left part of fuzzy set
        if(fzzSystem.input[in] <= fzzSystem.inSet[in].fSet[i].top){
            double k = 1.0 / (fzzSystem.inSet[in].fSet[i].top - fzzSystem.inSet[in].fSet[i].left); 
            double x = fzzSystem.input[in] - fzzSystem.inSet[in].fSet[i].left;
            fzfOut[in].res[fzfOut[in].length].membership = k*x;           
        }
        //input intersects right part of fuzzy set
        else{
            double k = 1.0 / (fzzSystem.inSet[in].fSet[i].top - fzzSystem.inSet[in].fSet[i].right); 
            double x = fzzSystem.input[in] - fzzSystem.inSet[in].fSet[i].top;
            fzfOut[in].res[fzfOut[in].length].membership = k*x + 1;
        }
        
        #ifdef DEBUG_MSG
        printf("%s - %s: x=%f, A(x)=%f\n", 
            fzzSystem.inSet[in].name, 
            fzzSystem.inSet[in].fSet[i].name, 
            fzzSystem.input[in], 
            fzfOut[in].res[fzfOut[in].length].membership
        );
        #endif
        
        //fuzzy set name and index
        fzfOut[in].res[fzfOut[in].length].setIndex = i;
        fzfOut[in].length++; 
    }
}

/**
 * @brief Takes rule and process it
 * Internal function
 * @param ruleIndex index of rule
 */
void fzz_ininference(int ruleIndex){
    //state machine
    int state = 0;
    int i = 0;
    char ch = 0;
    char wbuf[WBUF_LENGTH];
    int wbufLen = 0;
    
    //parsing result variables
    int inputs[MAX_INPUTS];    
    int inSets[MAX_INPUTS];
    int inLen = 0;
    int output = 0;
    int outSet = 0;
    
    //other variables
    int var = 0;
    int j = 0;
    int k = 0;
    int match = 0;
    double min = 0;
            
    //state machine to process inferential mechanism rule
    while((ch = fzzSystem.rule[ruleIndex][i]) != '\0'){
        switch(state){
            //initial state (expecting if)
            case 0:
                if(ch == ' '){
                    wbuf[wbufLen] = '\0';
                    assert(!strcmp(wbuf, "if") && "Invalid rule syntax, expecting 'if' at the beginning of rule in fzz_ininference(...)");
                    wbufLen = 0;
                    state = 1;
                }else{
                    wbuf[wbufLen] = ch;
                    wbufLen++;
                }
                break;
            
            //expecting name of input
            case 1:
                if(ch == ' '){
                    wbuf[wbufLen] = '\0';
                    var = fzz_inputIndex(wbuf);
                    assert(var != -1 && "Input name not found in fzz_ininference(...)");
                    inputs[inLen] = var;
                    wbufLen = 0;
                    state = 2;
                }else{
                    wbuf[wbufLen] = ch;
                    wbufLen++;
                }
                break;
                
            //expecting is
            case 2:
                if(ch == ' '){
                    wbuf[wbufLen] = '\0';
                    assert(!strcmp(wbuf, "is") && "Invalid rule syntax, expecting 'is' after input name in fzz_ininference(...)");
                    wbufLen = 0;
                    state = 3;
                }else{
                    wbuf[wbufLen] = ch;
                    wbufLen++;
                }
                break;
                
            //expecting name of input fuzzy set
            case 3:
                if(ch == ' '){
                    wbuf[wbufLen] = '\0';
                    var = fzz_inputFSetIndex(inputs[inLen], wbuf);
                    assert(var != -1 && "Input fuzzy set name not found in fzz_ininference(...)");
                    inSets[inLen] = var;
                    inLen++;
                    wbufLen = 0;
                    state = 4;
                }else{
                    wbuf[wbufLen] = ch;
                    wbufLen++;
                }
                break;
                        
            //expecting if or then
            case 4:
                if(ch == ' '){
                    wbuf[wbufLen] = '\0';
                    //printf("%s\n", wbuf);
                    if(!strcmp(wbuf, "and"))  state = 1;
                    else if(!strcmp(wbuf, "then")) state = 5;
                    else assert(0 && "Invalid rule syntax, expecting 'and' or 'then' after input fuzzy set name in fzz_ininference(...)"); 
                    wbufLen = 0;
                }else{
                    wbuf[wbufLen] = ch;
                    wbufLen++;
                }
                break;
                
            //expecting name of output
            case 5:
                if(ch == ' '){
                    wbuf[wbufLen] = '\0';
                    output = fzz_outputIndex(wbuf);
                    assert(output != -1 && "Output name not found in fzz_ininference(...)");
                    wbufLen = 0;
                    state = 6;
                }else{
                    wbuf[wbufLen] = ch;
                    wbufLen++;
                }
                break;
                
            //expecting is
            case 6:
                if(ch == ' '){
                    wbuf[wbufLen] = '\0';
                    assert(!strcmp(wbuf, "is") && "Invalid rule syntax, expecting 'is' after output name in fzz_ininference(...)");
                    wbufLen = 0;
                    state = 7;
                }else{
                    wbuf[wbufLen] = ch;
                    wbufLen++;
                }
                break;
                
            //expecting name of output fuzzy set
            case 7:
                wbuf[wbufLen] = ch;
                wbufLen++;
                break;
        }
        i++;
    }
    
    //finishig state mechine run
    wbuf[wbufLen] = '\0';
    outSet = fzz_outputFSetIndex(output, wbuf);
    assert(outSet != -1 && "Output fuzzy set name not found in fzz_ininference(...)");
    wbufLen = 0;
    wbuf[0] = '\0';
    
    //evaluation of parsed rule
    match = 0;
    min = 0;
    for(i = 0; i < fzzSystem.inLen; i++){
        //search for first input in parsed data
        for(j = 0; j < inLen; j++){
            if(i == inputs[j]){
                //search for fuzzy set in fuzzyfication result
                for(k = 0; k < fzfOut[i].length; k++){
                    if(fzfOut[i].res[k].setIndex == inSets[j]){
                        if(match == 0){
                            min = fzfOut[i].res[k].membership;
                        }else if(fzfOut[i].res[k].membership < min){
                            min = fzfOut[i].res[k].membership;
                        }
                        match++;
                    }
                }
                break;
            }
        }
    }
    
    //saving evaluation result
    if(match == inLen){
        #ifdef DEBUG_MSG
        printf("%s \n -> passed (%s(%d) - %s(%d): %f)\n", 
            fzzSystem.rule[ruleIndex], 
            fzzSystem.outSet[output].name,
            output,
            fzzSystem.outSet[output].fSet[outSet].name,
            outSet,
            min
        );
        #endif
        infOut[output].res[infOut[output].length].value = min;
        infOut[output].res[infOut[output].length].fSet = outSet;
        infOut[output].length++;
    }
}

/**
 * @brief Used for defuzzyfication
 * Internal function
 */
double fzz_outputValue(int output, double x){
    double max = 0;
    int i = 0;
    int j = 0;
    double k = 0;
    double xNorm = 0;
    double memb = 0;
    
    //membership of center
     for(i = 0; i < infOut[output].length; i++){
        j = infOut[output].res[i].fSet;
        //x value intersects fuzzy set
        if(x <= fzzSystem.outSet[output].fSet[j].left) continue;
        if(x >= fzzSystem.outSet[output].fSet[j].right) continue;
        //input intersects left part of fuzzy set
        if(x <= fzzSystem.outSet[output].fSet[j].top){
            k = 1.0 / (fzzSystem.outSet[output].fSet[j].top - fzzSystem.outSet[output].fSet[j].left); 
            xNorm = x - fzzSystem.outSet[output].fSet[j].left;
            memb = k*xNorm;
            if(memb > infOut[output].res[i].value)
                memb = infOut[output].res[i].value;
            if(memb > max)
                max = memb;
        }
        //input intersects right part of fuzzy set
        else{
            k = 1.0 / (fzzSystem.outSet[output].fSet[j].top - fzzSystem.outSet[output].fSet[j].right); 
            xNorm = x - fzzSystem.outSet[output].fSet[j].top;
            memb = k*xNorm + 1;
            if(memb > infOut[output].res[i].value)
                memb = infOut[output].res[i].value;
            if(memb > max)
                max = memb;
        }
    }
    
    //returns result
    return max;
}

/**
 * @brief Calculates crisp output values of system
 * Internal function, center of area / gravity method
 * @param output index of output
 */
void fzz_defuzzify(int output){
    int i = 0;
    int j = 0;
    double from = DBL_MAX;
    double to = -DBL_MAX;
    double x = 0;
    double numerator = 0;
    double denominator = 0;   
    double max = 0;
    
    //search for range
    for(i = 0; i < infOut[output].length; i++){
        j = infOut[output].res[i].fSet;
        if(fzzSystem.outSet[output].fSet[j].left < from)
            from = fzzSystem.outSet[output].fSet[j].left;
        if(fzzSystem.outSet[output].fSet[j].right > to)
            to = fzzSystem.outSet[output].fSet[j].right;
    }

    #ifdef DEBUG_MSG
    printf("%s: range(%f to %f)\n", fzzSystem.outSet[output].name, from, to);
    #endif
    
    //integration 
    for(x = from; x < to+COG_STEP; x+=COG_STEP){
        max = fzz_outputValue(output, x);
        numerator += x*max;
        denominator += max;
    }

    //x coord of center of gravity
    fzzSystem.output[output] = numerator / denominator;
}

void fzz_calculateOutput(){
    int i = 0;
    int j = 0;

    #ifdef DEBUG_MSG
    printf("\nOutput calculation\n------------------\nFuzzyfication:\n");
    #endif
    
    //fuzzifycation process
    for(i = 0; i < fzzSystem.inLen; i++){
        fzfOut[i].length = 0;
        fzz_fuzzify(i);
    }
    
    #ifdef DEBUG_MSG
    printf("\nInference:\n");
    #endif
    
    //inferential mechanism
    for(i = 0; i < fzzSystem.outLen; i++)
        infOut[i].length = 0;
    for(i = 0; i < fzzSystem.ruLen; i++)
        fzz_ininference(i);
    
    #ifdef DEBUG_MSG
    printf("\nDefuzzyfication:\n");
    #endif
    
    //defuzzifycation process
    for(i = 0; i < fzzSystem.outLen; i++)
        fzz_defuzzify(i);
}

///////////////////////////////////////////////////
//////// Support functions ////////////////////////
///////////////////////////////////////////////////

void fzz_printInputSet(int index){
    int i = 0;
    
    //index check
    assert(index < fzzSystem.inLen && "Index out of range in fzz_printInputSet(...)");
    
    //header
    printf("Input set for input %d named \"%s\":\n", index, fzzSystem.inSet[index].name);
    
    //fuzzy sets
    for(i = 0; i < fzzSystem.inSet[index].length; i++){
        printf(
            "Fuzzy set %d named \"%s\": [%f,0],[%f,1],[%f,0]\n", 
            i,
            fzzSystem.inSet[index].fSet[i].name,
            fzzSystem.inSet[index].fSet[i].left,
            fzzSystem.inSet[index].fSet[i].top,
            fzzSystem.inSet[index].fSet[i].right
        );
    }
}

void fzz_printOutputSet(int index){
    int i = 0;
    
    //index check
    assert(index < fzzSystem.outLen && "Index out of range in fzz_printOutputSet(...)");
    
    //header
    printf("Output set for output %d named \"%s\":\n", index, fzzSystem.outSet[index].name);
    
    //fuzzy sets
    for(i = 0; i < fzzSystem.outSet[index].length; i++){
        printf(
            "Fuzzy set %d named \"%s\": [%f,0],[%f,1],[%f,0]\n", 
            i,
            fzzSystem.outSet[index].fSet[i].name,
            fzzSystem.outSet[index].fSet[i].left,
            fzzSystem.outSet[index].fSet[i].top,
            fzzSystem.outSet[index].fSet[i].right
        );
    }  
}

void fzz_printRules(){
    int i = 0;
    
    //header
    printf("System contains %d rules of inferential mechanism:\n", fzzSystem.ruLen);
    
    //rules
    for(i = 0; i < fzzSystem.ruLen; i++){
        printf("%3d: %s\n", i, fzzSystem.rule[i]);
    }
}

void fzz_printSystem(){
    int i = 0;
    
    printf("\n");
    printf("+----------------------------------------+\n");
    printf("|              Fuzzy system              |\n");
    printf("+----------------------------------------+\n\n");

    //input sets
    for(i = 0; i < fzzSystem.inLen; i++){
        fzz_printInputSet(i);
        printf("\n");
    }
    
    //output sets
    for(i = 0; i < fzzSystem.outLen; i++){
        fzz_printOutputSet(i);
        printf("\n");
    }
    
    //system rules
    fzz_printRules();
}

///////////////////////////////////////////////////
//////// Tests ////////////////////////////////////
///////////////////////////////////////////////////

void fzz_test1(){
    double x1 = 0;

    //fuzzy system init
    fzz_init(1, 1);

    //init of sets of input and output fuzzy sets
    fzz_initInputFcns(0, 3, "input");
    fzz_initOutputFcns(0, 3, "output");
    
    //input fuzzy sets
    fzz_setInputFcn(0, 0, -2.0, -1.0,  0.0, "negative");
    fzz_setInputFcn(1, 0, -1.0,  0.0,  1.0, "zero");
    fzz_setInputFcn(2, 0,  0.0,  1.0,  2.0, "pozitive");
    
    //output fuzzy sets
    fzz_setOutputFcn(0, 0, -2.0, -1.0, 0.0, "negative");
    fzz_setOutputFcn(1, 0, -1.0,  0.0, 1.0, "zero");
    fzz_setOutputFcn(2, 0,  0.0,  1.0, 2.0, "pozitive");
       
    //rules for output
    fzz_addRule("if input is negative then output is pozitive");
    fzz_addRule("if input is zero then output is zero");
    fzz_addRule("if input is pozitive then output is negative");
    
    //prints system
    fzz_printSystem();
    
    //output calculation
    printf("\nCalculated outputs:\n");
    for(x1 = -1; x1 <= 1; x1 += 0.1){
        fzz_setInput(0, x1);
        fzz_calculateOutput();
        printf("%+f => %+f\n", x1, fzz_getOutput(0));
    }
}

void fzz_test2(){
    double x1 = 0;

    //fuzzy system init
    fzz_init(1, 2);

    //init of sets of input and output fuzzy sets
    fzz_initInputFcns(0, 3, "input");
    fzz_initOutputFcns(0, 3, "output1");
    fzz_initOutputFcns(1, 3, "output2");
    
    //input fuzzy sets
    fzz_setInputFcn(0, 0, -2.0, -1.0,  0.0, "negative");
    fzz_setInputFcn(1, 0, -1.0,  0.0,  1.0, "zero");
    fzz_setInputFcn(2, 0,  0.0,  1.0,  2.0, "pozitive");
    
    //output1 fuzzy sets
    fzz_setOutputFcn(0, 0, -2.0, -1.0, 0.0, "negative");
    fzz_setOutputFcn(1, 0, -1.0,  0.0, 1.0, "zero");
    fzz_setOutputFcn(2, 0,  0.0,  1.0, 2.0, "pozitive");
   
    //output2 fuzzy sets
    fzz_setOutputFcn(0, 1, -4.0, -2.0, 0.0, "negative");
    fzz_setOutputFcn(1, 1, -2.0,  0.0, 2.0, "zero");
    fzz_setOutputFcn(2, 1,  0.0,  2.0, 4.0, "pozitive");
    
    //rules for output
    fzz_addRule("if input is negative then output1 is pozitive");
    fzz_addRule("if input is zero then output1 is zero");
    fzz_addRule("if input is pozitive then output1 is negative");
    fzz_addRule("if input is negative then output2 is negative");
    fzz_addRule("if input is zero then output2 is negative");
    fzz_addRule("if input is pozitive then output2 is pozitive");
    
    //prints system
    fzz_printSystem();
    
    //output calculation
    printf("\nCalculated outputs:\n");
    for(x1 = -1; x1 <= 1; x1 += 0.1){
        fzz_setInput(0, x1);
        fzz_calculateOutput();
        printf("%+f => %+f %+f\n", x1, fzz_getOutput(0), fzz_getOutput(1));
    }
}

void fzz_test3(){
    double x1 = 0;
    double x2 = 0;

    //fuzzy system init
    fzz_init(2, 1);

    //init of sets of input and output fuzzy sets
    fzz_initInputFcns(0, 3, "input1");
    fzz_initInputFcns(1, 3, "input2");
    fzz_initOutputFcns(0, 3, "output");
    
    //input1 fuzzy sets
    fzz_setInputFcn(0, 0, -2.0, -1.0,  0.0, "negative");
    fzz_setInputFcn(1, 0, -1.0,  0.0,  1.0, "zero");
    fzz_setInputFcn(2, 0,  0.0,  1.0,  2.0, "pozitive");

    //input1 fuzzy sets
    fzz_setInputFcn(0, 1, -2.0, -1.0,  0.0, "negative");
    fzz_setInputFcn(1, 1, -1.0,  0.0,  1.0, "zero");
    fzz_setInputFcn(2, 1,  0.0,  1.0,  2.0, "pozitive");
    
    //output fuzzy sets
    fzz_setOutputFcn(0, 0, -2.0, -1.0, 0.0, "negative");
    fzz_setOutputFcn(1, 0, -1.0,  0.0, 1.0, "zero");
    fzz_setOutputFcn(2, 0,  0.0,  1.0, 2.0, "pozitive");
       
    //rules for output
    fzz_addRule("if input1 is negative and input2 is negative then output is negative");
    fzz_addRule("if input1 is negative and input2 is zero then output is negative");
    fzz_addRule("if input1 is negative and input2 is pozitive then output is zero");
    fzz_addRule("if input1 is zero and input2 is negative then output is negative");
    fzz_addRule("if input1 is zero and input2 is zero then output is zero");
    fzz_addRule("if input1 is zero and input2 is pozitive then output is pozitive");
    fzz_addRule("if input1 is pozitive and input2 is negative then output is zero");
    fzz_addRule("if input1 is pozitive and input2 is zero then output is pozitive");
    fzz_addRule("if input1 is pozitive and input2 is pozitive then output is pozitive");
    
    //prints system
    fzz_printSystem();
    
    //output calculation
    printf("\nCalculated outputs:\n x1\\x2 |");
    for(x2 = -1; x2 < 1.1; x2 += 0.25){
        printf(" %+1.2f |",x2); 
    }
    printf("\n--------------------------------------------------------------------------------\n");
    for(x1 = -1; x1 < 1.1; x1 += 0.25){
        printf(" %+1.2f |",x1);
        for(x2 = -1; x2 < 1.1; x2 += 0.25){
            fzz_setInput(0, x1);
            fzz_setInput(1, x2);
            fzz_calculateOutput();
            printf(" %+1.2f |", fzz_getOutput(0));
        }
        printf("\n");
    }
    printf("--------------------------------------------------------------------------------\n");
}
