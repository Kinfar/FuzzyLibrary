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

///////////////////////////////////////////////////
//////// Defines //////////////////////////////////
///////////////////////////////////////////////////

#define NAME_LENGTH 16
#define RULE_LENGTH 128
#define MAX_RULES 256
#define MAX_INPUTS 4
#define MAX_OUTPUTS 2
#define MAX_FSETS 16

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
 * @brief Output of fuzzifycation process
 */
typedef struct{
    int inputIndex;
    int setIndex;
    double membership;
}TFuzzifyOut;

///////////////////////////////////////////////////
//////// Global variables /////////////////////////
///////////////////////////////////////////////////

//Fuzzy system structure
TFzzSystem fzzSystem;

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
 * @brief Calculates fuzzified value of input with given index
 * Internal function
 * @param index index of input and index of input set
 * @param fzOut output data pointer
 */
void fzz_fuzzify(int index, TFuzzifyOut* fzOut){
    
}

void fzz_calculateOutput(){
    //TODO
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
