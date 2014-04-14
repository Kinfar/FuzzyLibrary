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

#define NAME_LENGTH 32
#define MAX_RULES 512

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
    TFuzzySet* fSet;
    int length;
    char name[NAME_LENGTH];
}TFcnsSet;

/**
 * @brief Fuzzy system data structure
 */
typedef struct{
    int inLen;
    int outLen;
    TFcnsSet* inSet;
    TFcnsSet* outSet;
    double* input;
    double* output;
    int ruLen;
    char* rule[MAX_RULES];
}TFzzSystem;

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
  
    //number of inputs and outputs
    fzzSystem.inLen = inputs;
    fzzSystem.outLen = outputs;
    
    //creation of input set of fuzzy sets
    fzzSystem.inSet = (TFcnsSet*)malloc(inputs * sizeof(TFcnsSet));
    assert(fzzSystem.inSet != NULL && "Memory allocation error in fzz_init: fzzSystem.inSet");
    
    for(i = 0; i < inputs; i++){
        fzzSystem.inSet[i].fSet = NULL;
        fzzSystem.inSet[i].length = 0;
        fzzSystem.inSet[i].name[0] = '\0';
    }
    
    //creation of output set of fuzzy sets
    fzzSystem.outSet = (TFcnsSet*)malloc(outputs * sizeof(TFcnsSet));
    assert(fzzSystem.outSet != NULL && "Memory allocation error in fzz_init: fzzSystem.outSet");
    for(i = 0; i < inputs; i++){
        fzzSystem.outSet[i].fSet = NULL;
        fzzSystem.outSet[i].length = 0;
        fzzSystem.outSet[i].name[0] = '\0';
    }
    
    //array for storing system inputs given by user
    fzzSystem.input = (double*)malloc(inputs * sizeof(double));
    assert(fzzSystem.input != NULL && "Memory allocation error in fzz_init: fzzSystem.input");
    for(i = 0; i < inputs; i++) fzzSystem.input[i] = 0;
    
    //array for storing calculated system outputs
    fzzSystem.output = (double*)malloc(outputs * sizeof(double));
    assert(fzzSystem.output != NULL && "Memory allocation error in fzz_init: fzzSystem.output");
    for(i = 0; i < outputs; i++) fzzSystem.output[i] = 0;
    
    //list of fuzzy inference rules
    for(i = 0; i < MAX_RULES; i++){
        fzzSystem.rule[i] = NULL;
    }
    fzzSystem.ruLen = 0;
}

void fzz_deinit(){
    int i = 0;
    
    //dealocation of input set of fuzzy sets
    for(i = 0; i < fzzSystem.inLen; i++){
        free(fzzSystem.inSet[i].fSet);
        fzzSystem.inSet[i].fSet = NULL;
    }
    free(fzzSystem.inSet);
    fzzSystem.inSet = NULL;

    //dealocation of output set of fuzzy sets
    for(i = 0; i < fzzSystem.outLen; i++){
        free(fzzSystem.outSet[i].fSet);
        fzzSystem.outSet[i].fSet = NULL;
    }
    free(fzzSystem.outSet);
    fzzSystem.outSet = NULL;
    
    //array for storing system inputs given by user
    free(fzzSystem.input);
    fzzSystem.input = NULL;
    
    //array for storing calculated system outputs
    free(fzzSystem.output);
    fzzSystem.output = NULL;
    
    //list of fuzzy inference rules
    for(i = 0; i < MAX_RULES; i++){
        free(fzzSystem.rule[i]);
        fzzSystem.rule[i] = NULL;
    }
}

void fzz_initInputFcns(int index, int length, char* name){
    int i = 0;
   
    //stores length information
    fzzSystem.inSet[index].length = length;
    fzzSystem.inSet[index].name[0] = '\0';
    
    //creates array for fuzzy sets
    fzzSystem.inSet[index].fSet = (TFuzzySet*)malloc(length * sizeof(TFuzzySet));
    assert(fzzSystem.inSet[index].fSet != NULL && "Memory allocation error in fzz_initInputFcns: fzzSystem.inSet[index].fSet");
    for(i = 0; i < length; i++){
        fzzSystem.inSet[index].fSet[i].left = 0;
        fzzSystem.inSet[index].fSet[i].top = 0;
        fzzSystem.inSet[index].fSet[i].right = 0;
        fzzSystem.inSet[index].fSet[i].name[0] = '\0';
    }
    
    //stores name of set of fuzzy sets
     strncpy(fzzSystem.inSet[index].name, name, NAME_LENGTH);
}

void fzz_initOutputFcns(int index, int length, char* name){
    int i = 0;
    
    //stores length information
    fzzSystem.outSet[index].length = length;
    fzzSystem.outSet[index].name[0] = '\0';
    
    //creates array for fuzzy sets
    fzzSystem.outSet[index].fSet = (TFuzzySet*)malloc(length * sizeof(TFuzzySet));
    assert(fzzSystem.outSet[index].fSet != NULL && "Memory allocation error in fzz_initOutputFcns: fzzSystem.outSet[index].fSet");
    for(i = 0; i < length; i++){
        fzzSystem.outSet[index].fSet[i].left = 0;
        fzzSystem.outSet[index].fSet[i].top = 0;
        fzzSystem.outSet[index].fSet[i].right = 0;
        fzzSystem.outSet[index].fSet[i].name[0] = '\0';
    }
    
    //stores name of set of fuzzy sets
     strncpy(fzzSystem.outSet[index].name, name, NAME_LENGTH);
}

void fzz_setInputFcn(int index, int fcSet, double left, double top, double right, char* name){
    //membership function
    fzzSystem.inSet[fcSet].fSet[index].left = left;
    fzzSystem.inSet[fcSet].fSet[index].top = top;
    fzzSystem.inSet[fcSet].fSet[index].right = right;
    
    //fuzzy set name
    strncpy(fzzSystem.inSet[fcSet].fSet[index].name, name, NAME_LENGTH);
}

void fzz_setOutputFcn(int index, int fcSet, double left, double top, double right, char* name){
    //membership function
    fzzSystem.outSet[fcSet].fSet[index].left = left;
    fzzSystem.outSet[fcSet].fSet[index].top = top;
    fzzSystem.outSet[fcSet].fSet[index].right = right;
    
    //fuzzy set name
    strncpy(fzzSystem.outSet[fcSet].fSet[index].name, name, NAME_LENGTH);
}

void fzz_addRule(char* rule){
    int len = strlen(rule);
    fzzSystem.rule[fzzSystem.ruLen] = (char*)malloc((len+1) * sizeof(char));
    assert(fzzSystem.rule[fzzSystem.ruLen] != NULL && "Memory allocation error in fzz_addRule: fzzSystem.rule[fzzSystem.ruLen]");
    strcpy(fzzSystem.rule[fzzSystem.ruLen], rule);
    fzzSystem.ruLen++;
}

void fzz_setInput(int index, double value){
    fzzSystem.input[index] = value;
}

double fzz_getOutput(int index){
    return fzzSystem.output[index];
}

void fzz_calculateOutput(){
    //TODO
}

///////////////////////////////////////////////////
//////// Support functions ////////////////////////
///////////////////////////////////////////////////

void fzz_printInputSet(int index){
    int i = 0;
    
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
