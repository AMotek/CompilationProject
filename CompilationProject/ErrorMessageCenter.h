#pragma once

const char* duplicateErrorMsg = "Error occured in line %d, lexeme: %s is already declared in the current scope\n";

const char* missMatchSignatureSize = "Mismatch in size of functions at line %d. Expected to have %d, but got %d\n";

const char* missMatchSignatureType = "Mismatch in paremets type used at line %d at paremets index %d. Expected %s but got %s\n";

const char* missMatchReturnType = "Missmatch in return type used at line %d. Expected %s but got %s\n";

const char* undeclaredId = "An error occured in line %d, used an undelacred id %s\n";

const char* notAllWasUsed = "Not all variables or paremeres were used. %s was not used\n";

const char* missMatchFunctionCall = "Mismatch between number of argument in function call in line %d, expected %d, but got %d\n";

const char* missMatchArgumentToParemeter = "Missmatch between argument function call to the parameter in line %d, expected %s but got %s\n";

const char* unsupportedAssigmentRole = "Error occured at line %d, unsupported assigment to role of %s\n";

const char* unknownRightValue = "Error occured at line %d, right side has unknown value\n";

const char* intLeftFloatRight = "Error occured at line %d, left side is of type int while right side is of type float\n";