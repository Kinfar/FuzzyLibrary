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
 * @brief Example and test file for fzzlib library
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
    fzz_init(2, 1);
    printf(" completed");

    //init of sets of input and output fuzzy sets
    printf("\nInitialization of sets of input and output fuzzy sets ...");
    fzz_initInputFcns(0, 3, "distance");
    fzz_initInputFcns(1, 3, "speed");
    fzz_initOutputFcns(0, 5, "throttle");
    printf(" completed");    
    
    //creation of input and output fuzzy sets
    printf("\nCreating input and output fuzzy sets ...");
    //first input (distance)
    fzz_setInputFcn(0, 0, -0.5, 0.0, 0.5, "small");
    fzz_setInputFcn(1, 0,  0.0, 0.5, 1.0, "medium");
    fzz_setInputFcn(2, 0,  0.5, 1.0, 1.5, "big");
    //second input (speed)
    fzz_setInputFcn(0, 1, -1.0, 0.0, 1.0, "slow");
    fzz_setInputFcn(1, 1,  0.0, 1.0, 2.0, "medium");
    fzz_setInputFcn(2, 1,  1.0, 2.0, 3.0, "fast");
    //first output (throttle)
    fzz_setOutputFcn(0, 0, -1.5, -1.0, -0.5, "negativeBig");
    fzz_setOutputFcn(1, 0, -1.0, -0.5,  0.0, "negative");
    fzz_setOutputFcn(2, 0, -0.5,  0.0,  0.5, "zero");
    fzz_setOutputFcn(3, 0,  0.0,  0.5,  1.0, "pozitive");
    fzz_setOutputFcn(4, 0,  0.5,  1.0,  1.5, "pozitiveBig");
    //input and output sets are defined
    printf(" completed");
    
    //adding rules for inferential mechanism
    printf("\nAdding rules for inferential mechanism ...");
    //rules for first output (throttle)
    fzz_addRule("if distance is small and speed is slow then throttle is zero");
    fzz_addRule("if distance is small and speed is medium then throttle is negative");
    fzz_addRule("if distance is small and speed is fast then throttle is negativeBig");
    fzz_addRule("if distance is medium and speed is slow then throttle is pozitive");
    fzz_addRule("if distance is medium and speed is medium then throttle is zero");
    fzz_addRule("if distance is medium and speed is fast then throttle is negative");
    fzz_addRule("if distance is big and speed is slow then throttle is pozitiveBig");
    fzz_addRule("if distance is big and speed is medium then throttle is pozitive");
    fzz_addRule("if distance is big and speed is fast then throttle is zero");
    //all rules added
    printf(" completed\n");
}

/**
 * @brief Example of printing info about fuzzy system to string
 */
void printExample(){
    //prints input set of second input
    printf("\n");
    fzz_printInputSet(1);
    
    //prints input set of first output
    printf("\n");
    fzz_printOutputSet(0);
    
    //prints rules of inferential mechanism
    printf("\n");
    fzz_printRules();
    
    //prints entire system
    fzz_printSystem();
}

/**
 * @brief Output calculation example
 */
void gettingOutputExample(){
    int i = 0;
    
    //sets system input
    printf("\nSetting input to (0.2, 1.25)");
    fzz_setInput(0, 0.2);
    fzz_setInput(1, 1.25);
    printf(" ... completed\n");
    
    //output calculation
    printf("\nCalculating output ...");
    fzz_calculateOutput();
    printf(" completed\n");
     
    //output getting
    printf("\nGetting output\n");
    printf("output%d: %f\n", i, fzz_getOutput(0));
    printf("... completed\n");
    
    //fuzzy system deinit
    fzz_deinit();
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
    
    //tests run
    fzz_test1();
    fzz_test2();
    fzz_test3();
    
    //all done
    printf("\n");
    return 0;
}
