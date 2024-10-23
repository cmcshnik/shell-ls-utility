#include "utils.h"

void RemoveExtraAsterisks(char* str) {
    int len = strlen(str);
    char* result = (char*)malloc(len + 1);  
    int j = 0, i = 0; 
    
    while (i < len) {
        if (str[i] == '*') {
            result[j++] = '*';  
            
            while (i < len && str[i] == '*') {
                i++;
            }
            i--; 
        } else {
            result[j++] = str[i];
        }
        i++;
    }

    result[j] = '\0';  
    strcpy(str, result); 
    free(result);
}

const bool PatternCheckHelp(char* pattern, char* candidate, bool first_launch) {
    bool res = false;
    int pattern_idx = 0, candidate_idx = 0, remembered_candidate_idx, remembered_pattern_idx;

    if (strcmp(pattern, "\0") == 0) {
        return true;
    }

    while (candidate[candidate_idx] != '\0' && !res) {
        if (candidate[candidate_idx] == pattern[pattern_idx] ||
                                 pattern[pattern_idx] == '?' || 
                                 first_launch) {

            remembered_candidate_idx = candidate_idx;
            remembered_pattern_idx = pattern_idx;

            if (first_launch) {
                candidate_idx = -1;
                pattern_idx = -1;
            }

            do {
                candidate_idx++;      
                pattern_idx++; 

                if (pattern[pattern_idx] == '*') {
                    res = PatternCheckHelp(pattern + pattern_idx + 1, candidate + candidate_idx, 0);

                    if (!res) {
                        break;
                    }
                } else if (pattern[pattern_idx] != candidate[candidate_idx] && pattern[pattern_idx] != '?') {
                    break;
                }
            } while (candidate[candidate_idx] != '\0' && !res && pattern[pattern_idx] != '\0');

            if (candidate[candidate_idx] == '\0' && pattern[pattern_idx] == '\0') {
                res = true;
            }

            if (!res) {
                candidate_idx = remembered_candidate_idx;
                pattern_idx = remembered_pattern_idx;
            }
        }
        candidate_idx++;
    }

    return res;
}

// Функция проверки на соответствие шаблону
bool PatternCheck(char* pattern, char* candidate) {
    return PatternCheckHelp(pattern, candidate, 1);
}