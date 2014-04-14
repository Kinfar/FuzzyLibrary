/**
 * @brief Fuzzy logic library header
 * @author Petr Kacer <kacerpetr@gmail.com> 
 * @date 12.4.2014
 */

///////////////////////////////////////////////////
//////// Main functions ///////////////////////////
///////////////////////////////////////////////////

/**
 * @brief Initialization of fuzzy system
 * @param inputs number of system inputs
 * @param outputs number of system outputs
 */
void fzz_init(int inputs, int outputs);

/**
 * @brief Fuzzy logic system deinitialization
 * Clears input and output fuzzy sets, input and output 
 * values and deallocates all allocated memory
 */
void fzz_deinit();

/**
 * @brief Initialization of membership functions input set
 * @param index index of input membership functions set
 * @param length required number of membership functions in set
 * @param name membership functions set name
 */
void fzz_initInputFcns(int index, int length, char* name);

/**
 * @brief Initialization of membership functions output set
 * @param index index of output membership functions set
 * @param length required number of membership functions in set
 * @param name membership functions set name
 */
void fzz_initOutputFcns(int index, int length, char* name);

/**
 * @brief Sets membership function of fuzzy set in input set of membership functions
 * @param index index of fuzzy set in set of membership functions
 * @param fcSet index of input membership functions set
 * @param left x-axis position of first bottom point of triangular membership function 
 * @param top x-axis position of top point of triangular membership function 
 * @param right x-axis position of second bottom point of triangular membership function 
 * @param name name of fuzzy set
 */
void fzz_setInputFcn(int index, int fcSet, double left, double top, double right, char* name);

/**
 * @brief Sets membership function of fuzzy set in output set of membership functions
 * @param index index of fuzzy set in set of membership functions
 * @param fcSet index of output membership functions set
 * @param left x-axis position of first bottom point of triangular membership function 
 * @param top x-axis position of top point of triangular membership function 
 * @param right x-axis position of second bottom point of triangular membership function 
 * @param name name of fuzzy set
 */
void fzz_setOutputFcn(int index, int fcSet, double left, double top, double right, char* name);

/**
 * @brief Adds rule for inferential mechanism
 * Rule is in format "if input1 is big and input2 is medium then output is slow"
 * where "if", "is", "and", "then" are keywords in rule, "input1" and "input2"
 * are names of input sets of membership function, "output" is name of output set 
 * of membership functions, "big" and "medium" are names of one of input fuzzy sets
 * and "slow" is name of one of output fuzzy set.
 */
void fzz_addRule(char* rule);

/**
 * @brief Sets value of input for output calculation
 * @param index index of input
 * @param value value of input
 */
void fzz_setInput(int index, double value);

/**
 * @brief Getter of calculated output
 * @param index index of output
 * @return value of required output
 */
double fzz_getOutput(int index);

/**
 * @brief Calculates output of fuzzy system
 * Output calculation is composed from fuzzyfication,
 * inferential mechanism and defuzzyfication
 */
void fzz_calculateOutput();
