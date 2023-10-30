#include <stdio.h>
#include <stdlib.h>
 
char t;
char p;

//Состояния автомата
enum states {
    source_0,
    comment_start,
    line_comment,
    line_2_comment,
    multiline_comment,
    comment_end,
    string_state,
    char_state,
    string_symbol,
    char_symbol
};

//Сигналы, получаемые автоматом
enum signals {
    Anysign,
    sign_slash,
    sign_string,
    sign_char,
    sign_asterisk,
    sign_backslash,
    sign_newline,
};

enum states current_state;

//Таблица переходов по состояниям в зависимости от текущего состояния и полученного сигнала
enum states FSM_table[10][7] = {
        [source_0][Anysign]=source_0,
        [source_0][sign_slash]=comment_start,
        [source_0][sign_string]=string_state,
        [source_0][sign_char]=char_state,
        [comment_start][Anysign]=source_0,
        [comment_start][sign_slash]=line_comment,
        [comment_start][sign_asterisk]=multiline_comment,
        [multiline_comment][sign_asterisk]=comment_end,
        [multiline_comment][Anysign]=multiline_comment,
        [comment_end][sign_slash]=source_0,
        [comment_end][Anysign]=multiline_comment,
        [line_comment][Anysign]=line_comment,
        [line_comment][sign_backslash]=line_2_comment,
        [line_2_comment][Anysign]=line_comment,
        [line_2_comment][sign_backslash]=line_2_comment,
        [line_comment][sign_newline]=source_0,
        [string_state][Anysign]=string_state,
        [string_state][sign_backslash]=string_symbol,
        [string_symbol][Anysign]=string_state,
        [string_state][sign_string]=source_0,
        [char_state][Anysign]=char_state,
        [char_state][sign_backslash]=char_symbol,
        [char_symbol][Anysign]=char_state,
        [char_state][sign_char]=source_0
};

//Функция получения сигнала на основе входящего символа\
Для каждого состояния только определенные сигналы вызовут смену состояния, остальные будут считаться сигналом по умолчанию
enum signals getSignal() {
    switch (current_state) {
        case source_0: switch (t) {
            case '/' : return sign_slash;
            case '\"' : return sign_string;
            case '\'' : return sign_char;
            default: return Anysign;
        }
        case comment_start: switch (t) {
            case '/' : return sign_slash;
            case '*' : return sign_asterisk;
            default: return Anysign;
        }
        case multiline_comment: if (t=='*') return sign_asterisk; else return Anysign;
        case line_comment: switch (t) {
            case '\n' : return sign_newline;
            case '\\' : return sign_backslash;
            default: return Anysign;
        }
        case line_2_comment : if (t=='\\') return sign_backslash; else return Anysign;
        case comment_end: if (t=='/') return sign_slash; else return Anysign;
        case char_state: switch (t) {
            case '\'' : return sign_char;
            case '\\' : return sign_backslash;
            default : return Anysign;
        }
        case string_state: switch (t) {
                case '\"' : return sign_string;
                case '\\' : return sign_backslash;
                default : return Anysign;
        }
        default: return Anysign;
    }
};

//Автомат записывает символы, если не находится в состоянии "комментарий"
void do_FSMtable() {
    enum signals current_signal = getSignal();
    switch (current_state) {
        default: {
            if (current_signal != sign_slash) {
                p=t;
            } else p='\0'; break;
        }
        case multiline_comment: break;
        case comment_end: if (current_signal == sign_slash) p=' '; break;
        case line_2_comment: break;
        case line_comment: break;
        case comment_start: {
            if ((current_signal != sign_slash) && (current_signal != sign_asterisk)) printf("/");
            else break;
        }
        case string_state:
        case char_state:
        case char_symbol:
        case string_symbol:
            p=t;
            break;
    }
    current_state = FSM_table[current_state][current_signal];
};

int main(int argc, char **argv) {
    FILE *F1 = argc > 1 ? fopen(argv[1], "r") : stdin;
    FILE *F2 = argc > 2 ? fopen(argv[2], "w+") : stdout;
    if (F1==NULL) {
	printf("Cannot open file %s\n", argv[1]);
	exit(1);
    }
    else { 
	while ((fscanf(F1, "%c", &t)) != EOF) {
        if (t!='\r') do_FSMtable();
	    fprintf(F2, "%s", &p);
        }
	fclose(F1);
	fclose(F2);
    };
    return 0;
}
