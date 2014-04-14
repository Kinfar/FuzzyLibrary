/**
 * @brief Example file for fzzlib library
 * @author Petr Kacer <kacerpetr@gmail.com>
 * @date 12.4.2014
 */

///////////////////////////////////////////////////
//////// Includes /////////////////////////////////
///////////////////////////////////////////////////

#include "fzzlib.h"
#include <stdio.h>

///////////////////////////////////////////////////
//////// Functions ////////////////////////////////
///////////////////////////////////////////////////

/**
 * @brief Example of creating fuzzy system (system initialiaztion included)
 * System to control speed when following moving object
 */
void createExample(){
    //fuzzy system init
    printf("Fuzzy system init ...");
    fzz_init(2, 2);
    printf(" completed");

    //init of sets of input and output fuzzy sets
    printf("\nInitialization of sets of input and output fuzzy sets ...");
    fzz_initInputFcns(0, 3, "distance");
    fzz_initInputFcns(1, 3, "speed");
    fzz_initOutputFcns(0, 3, "throttle");
    fzz_initOutputFcns(1, 3, "brake");
    printf(" completed");    
    
    //creation if input and output fuzzy sets
    printf("\nCreating input and output fuzzy sets ...");
    //first input (distance)
    fzz_setInputFcn(0, 0, -1.0, -0.5,  0.0, "small");
    fzz_setInputFcn(1, 0, -0.5,  0.0,  0.5, "medium");
    fzz_setInputFcn(2, 0,  0.0,  0.5,  1.0, "big");
    //second input (speed)
    fzz_setInputFcn(0, 1, -2.0, -1.0, 0.0, "slow");
    fzz_setInputFcn(1, 1, -1.0,  0.0, 1.0, "medium");
    fzz_setInputFcn(2, 1,  0.0,  1.0, 2.0, "fast");
    //first output (throttle)
    fzz_setOutputFcn(0, 0, -2.0, -1.0, 0.0, "full");
    fzz_setOutputFcn(1, 0, -1.0,  0.0, 1.0, "slight");
    fzz_setOutputFcn(2, 0,  0.0,  1.0, 2.0, "zero");
    //second output (brake)
    fzz_setOutputFcn(0, 1, -2.0, -1.0, 0.0, "hard");
    fzz_setOutputFcn(1, 1, -1.0,  0.0, 1.0, "slight");
    fzz_setOutputFcn(2, 1,  0.0,  1.0, 2.0, "zero");
    //input and output sets are defined
    printf(" completed");
    
    //adding rules for inferential mechanism
    printf("\nAdding rules for inferential mechanism ...");
    //rules for first output (throttle)
    fzz_addRule("if distance is small and speed is slow then throttle is slight");
    fzz_addRule("if distance is small and speed is medium then throttle is zero");
    fzz_addRule("if distance is small and speed is fast then throttle is zero");
    fzz_addRule("if distance is medium and speed is slow then throttle is slight");
    fzz_addRule("if distance is medium and speed is medium then throttle is slight");
    fzz_addRule("if distance is medium and speed is fast then throttle is zero");
    fzz_addRule("if distance is big and speed is slow then throttle is full");
    fzz_addRule("if distance is big and speed is medium then throttle is slight");
    fzz_addRule("if distance is big and speed is fast then throttle is zero");
    //rules for second output (brake)
    fzz_addRule("if distance is small and speed is slow then brake is zero");
    fzz_addRule("if distance is small and speed is medium then brake is slight");
    fzz_addRule("if distance is small and speed is fast then brake is hard");
    fzz_addRule("if distance is medium and speed is slow then brake is zero");
    fzz_addRule("if distance is medium and speed is medium then brake is zero");
    fzz_addRule("if distance is medium and speed is fast then brake is slight");
    fzz_addRule("if distance is big and speed is slow then brake is zero");
    fzz_addRule("if distance is big and speed is medium then brake is zero");
    fzz_addRule("if distance is big and speed is fast then brake is zero");
    //all rules added
    printf(" completed\n");
}

/**
 * @brief Example of printing info about fuzzy system to string
 */
void printExample(){
    //prints intput set of first input
    printf("\n");
    fzz_printInputSet(0);
    
    //prints intput set of second output
    printf("\n");
    fzz_printOutputSet(1);
    
    //prints rules of inferential mechanism
    printf("\n");
    fzz_printRules();
    
    //prints entire system
    fzz_printSystem();
}

/**
 * @brief Output calculation example (system deinitialiaztion included)
 */
void gettingOutputExample(){
    //fuzzy system deinit
    printf("\nFuzzy system deinit ...");
    fzz_deinit();
    printf(" completed");   
}

/**
 * @brief Main function
 * Contains demonstration of fzzlib functionality
 */ 
int main(int argc, const char* argv[]){
    //example of creating fuzzy system
    createExample();
    
    //example of printing info about fuzzy system to string
    printExample();
    
    //output calculation example
    gettingOutputExample();
        
    //all done
    printf("\n");
    return 0;
}
