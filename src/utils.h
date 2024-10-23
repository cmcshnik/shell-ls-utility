#pragma once 
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

// Замена всех последовательностей из * на *
void RemoveExtraAsterisks(char* str);

// Функция проверки на соответствие шаблону
bool PatternCheck(char* pattern, char* candidate);