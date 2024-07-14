#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

const char VALID_VALUES[] = { '2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K', 'A' };
const char VALID_COLORS[] = { 'h', 'd', 's', 'c' };
const int PLAYER_TOTAL = 2;
const int COMMON_TOTAL = 5;
const int ONE_CARD = 3;
const int FIVE_TOTAL = 5;
const int SEVEN_TOTAL = 7;
const int ALL_TOTAL = 9;
const int STRAIGHT_FLUSH = 8;
const int FOUR_OF_A_KIND = 7;
const int FULL_HOUSE = 6;
const int FLUSH = 5;
const int STRAIGHT = 4;
const int THREE_OF_A_KIND = 3;
const int TWO_PAIR = 2;
const int PAIR = 1;
const int HIGH_CARD = 0;

int get_cards(int count, int card_set, char cards[][ONE_CARD]);
int check_card_value(char card[]);
int check_card_color(char card[]);
int check_value_and_color(char card[]);
int check_duplications(int count, char cards[][ONE_CARD]);
void add_to_array(int count, int diff, char cards[][ONE_CARD], char all_cards[][ONE_CARD]);
int flush(char five_cards[][ONE_CARD]);
int get_int_repr(char symbol);
void sort_cards(char five_cards[][ONE_CARD], int int_repr[]);
int straight(char five_cards[][ONE_CARD]);
int cmp_int(const void *a, const void *b);
int same_value(char five_cards[][ONE_CARD]);
int find_max(int size, int array[]);
int evaluate_cards(char five_cards[][ONE_CARD]);
void index_combinations(int combinations[][FIVE_TOTAL]);
int card_maximum(int max, char card_combination[][ONE_CARD], char max_cards[][ONE_CARD]);
int player_combination_check(char cards_player[][ONE_CARD], char cards_common[][ONE_CARD], char max_cards[][ONE_CARD]);
int lowest_straight(int int_repr[]);
void find_card_with_type(int type, int result[], int counter[]);
void same_value_which_card(char five_cards[][ONE_CARD], int result[]);
void find_card(char five_cards[][ONE_CARD], int same_card, int result[]);
int compare_two_cards(int fst_int, int snd_int);
int compare_straight(char fst_comb[][ONE_CARD], char snd_comb[][ONE_CARD]);
int compare_high_card(int count, int high_card_fst[], int high_card_snd[]);
int compare_high_card_char(char fst_comb[][ONE_CARD], char snd_comb[][ONE_CARD]);
int compare_full_house_two_pair(int type, char fst_comb[][ONE_CARD], char snd_comb[][ONE_CARD]);
int compare_two_three_four(int count, char fst_comb[][ONE_CARD], char snd_comb[][ONE_CARD]);
int compare_two_sets(int type, char fst_comb[][ONE_CARD], char snd_comb[][ONE_CARD]);
void compare_players(char cards_fst[][ONE_CARD], char cards_snd[][ONE_CARD], char cards_com[][ONE_CARD]);

int main(void)
{
    char cards_fst[PLAYER_TOTAL][ONE_CARD];
    char cards_snd[PLAYER_TOTAL][ONE_CARD];
    char cards_com[COMMON_TOTAL][ONE_CARD];
    char all_cards[ALL_TOTAL][ONE_CARD];
    while (true) {
        if (get_cards(PLAYER_TOTAL, 1, cards_fst) == 1) {
            break;
        }
        if (get_cards(PLAYER_TOTAL, 2, cards_snd) == 1) {
            break;
        }
        if (get_cards(COMMON_TOTAL, 3, cards_com) == 1) {
            break;
        }
        add_to_array(PLAYER_TOTAL, 0, cards_fst, all_cards);
        add_to_array(PLAYER_TOTAL, 2, cards_snd, all_cards);
        add_to_array(COMMON_TOTAL, 4, cards_com, all_cards);
        if (check_duplications(ALL_TOTAL, all_cards) == 1) {
            break;
        }
        compare_players(cards_fst, cards_snd, cards_com);
    }
    return 0;
}

int get_cards(int count, int card_set, char cards[][ONE_CARD])
{
    int counter = 0;
    for (int i = 0; i < count; i++) {
        if (scanf(" %2s", cards[i]) == EOF) {
            break;
        }
        if (check_value_and_color(cards[i]) == 1) {
            return 1;
        }
        counter++;
    }
    if (card_set == 1 && counter == 0) {
        return 1;
    }
    if (counter != count) {
        fprintf(stderr, "invalid number of input cards in set\n");
        return 1;
    }
    char new_line = getchar();
    if (new_line != '\n') {
        fprintf(stderr, "invalid card set delimiter\n");
        return 1;
    }
    return check_duplications(count, cards);
}

int check_card_value(char card[])
{
    for (int i = 0; i < 13; i++) {
        if (card[0] == VALID_VALUES[i]) {
            return 0;
        }
    }
    return 1;
}

int check_card_color(char card[])
{
    for (int i = 0; i < 4; i++) {
        if (card[1] == VALID_COLORS[i]) {
            return 0;
        }
    }
    return 1;
}

int check_value_and_color(char card[])
{
    if (check_card_value(card) == 1 || check_card_color(card) == 1) {
        fprintf(stderr, "invalid card\n");
        return 1;
    }
    return 0;
}

int check_duplications(int count, char cards[][ONE_CARD])
{
    for (int i = 0; i < count; i++) {
        for (int j = i + 1; j < count; j++) {
            if (cards[i][0] == cards[j][0] && cards[i][1] == cards[j][1]) {
                fprintf(stderr, "duplicate card detected\n");
                return 1;
            }
        }
    }
    return 0;
}

void add_to_array(int count, int diff, char cards[][ONE_CARD], char all_cards[][ONE_CARD])
{
    for (int i = 0; i < count; i++) {
        for (int j = 0; j < ONE_CARD; j++) {
            all_cards[i + diff][j] = cards[i][j];
        }
    }
}

int flush(char five_cards[][ONE_CARD])
{
    int counter[4] = { 0 };
    for (int i = 0; i < FIVE_TOTAL; i++) {
        switch (five_cards[i][1]) {
        case 'h':
            counter[0]++;
            break;
        case 'd':
            counter[1]++;
            break;
        case 's':
            counter[2]++;
            break;
        case 'c':
            counter[3]++;
            break;
        }
    }
    for (int i = 0; i < 4; i++) {
        if (counter[i] == 5) {
            return FLUSH;
        }
    }
    return -1;
}

int get_int_repr(char symbol)
{
    if (symbol >= '2' && symbol <= '9') {
        return symbol - '0';
    }
    switch (symbol) {
    case 'T':
        return 10;
    case 'J':
        return 11;
    case 'Q':
        return 12;
    case 'K':
        return 13;
    case 'A':
        return 14;
    }
    return 0;
}

void sort_cards(char five_cards[][ONE_CARD], int int_repr[])
{
    for (int i = 0; i < FIVE_TOTAL; i++) {
        int_repr[i] = get_int_repr(five_cards[i][0]);
    }
    qsort(int_repr, 5, sizeof(int), cmp_int);
}

int straight(char five_cards[][ONE_CARD])
{
    int int_repr[FIVE_TOTAL];
    sort_cards(five_cards, int_repr);
    int counter = 0;
    for (int i = 1; i < FIVE_TOTAL; i++) {
        if (int_repr[i - 1] == int_repr[i] + 1) {
            counter++;
        }
    }
    if (counter == 4) {
        return STRAIGHT;
    }
    if (counter == 3 && int_repr[0] == get_int_repr('A') && int_repr[1] == get_int_repr('5')) {
        return STRAIGHT;
    }
    return -1;
}

int cmp_int(const void *a, const void *b)
{
    int arg1 = *(const int *) a;
    int arg2 = *(const int *) b;
    if (arg1 < arg2) {
        return 1;
    }
    if (arg1 > arg2) {
        return -1;
    }
    return 0;
}

int same_value(char five_cards[][ONE_CARD])
{
    int counter[13] = { 0 };
    for (int i = 0; i < FIVE_TOTAL; i++) {
        int index = get_int_repr(five_cards[i][0]) - 2;
        counter[index]++;
    }
    int new_counter[2] = { 0 };
    int index = 0;
    for (int i = 0; i < 13; i++) {
        if (counter[i] >= 2) {
            new_counter[index] = counter[i];
            index++;
        }
    }
    qsort(new_counter, 2, sizeof(int), cmp_int);
    if (new_counter[0] == 4) {
        return FOUR_OF_A_KIND;
    }
    if (new_counter[0] == 3) {
        if (new_counter[1] == 2) {
            return FULL_HOUSE;
        }
        return THREE_OF_A_KIND;
    }
    if (new_counter[0] == 2) {
        if (new_counter[1] == 2) {
            return TWO_PAIR;
        }
        return PAIR;
    }
    return -1;
}

int find_max(int size, int array[])
{
    int max = INT_MIN;
    for (int i = 0; i < size; i++) {
        if (array[i] > max) {
            max = array[i];
        }
    }
    return max;
}

int evaluate_cards(char five_cards[][ONE_CARD])
{
    int results[4];
    results[0] = straight(five_cards);
    results[1] = flush(five_cards);
    results[3] = same_value(five_cards);
    if (results[0] == STRAIGHT && results[1] == FLUSH) {
        results[2] = STRAIGHT_FLUSH;
    } else {
        results[2] = -1;
    }
    int max = find_max(4, results);
    if (max > 0) {
        return max;
    }
    return HIGH_CARD;
}

void index_combinations(int combinations[][FIVE_TOTAL])
{
    int skip;
    for (int i = 0; i < 10; i++) {
        if (i < 5) {
            combinations[i][0] = 0;
            skip = i + 2;
        } else {
            combinations[i][0] = 1;
            skip = i - 3;
        }
        int value = 2;
        for (int j = 1; j < FIVE_TOTAL; j++) {
            if (value == skip) {
                value++;
            }
            combinations[i][j] = value;
            value++;
        }
    }
    int values[5] = { 2, 3, 4, 5, 6 };
    int index[3] = { 0, 1, 2 };
    for (int i = 10; i < 20; i++) {
        combinations[i][0] = 0;
        combinations[i][1] = 1;
        for (int j = 0; j < 3; j++) {
            combinations[i][j + 2] = values[index[j]];
        }
        if (index[2] < 5) {
            index[2]++;
        }
        if (index[2] == 5) {
            if (index[1] < 4) {
                index[1]++;
                index[2] = index[1] + 1;
            }
            if (index[1] == 4) {
                index[0]++;
                index[1] = index[0] + 1;
                index[2] = index[1] + 1;
            }
        }
    }
}

int card_maximum(int max, char card_combination[][ONE_CARD], char max_cards[][ONE_CARD])
{
    int result = evaluate_cards(card_combination);
    if (result > max) {
        max = result;
        add_to_array(FIVE_TOTAL, 0, card_combination, max_cards);
    } else if (result == max) {
        if (compare_two_sets(result, card_combination, max_cards) == 1) {
            add_to_array(FIVE_TOTAL, 0, card_combination, max_cards);
        }
    }
    return max;
}

int player_combination_check(char cards_player[][ONE_CARD], char cards_common[][ONE_CARD], char max_cards[][ONE_CARD])
{
    char seven_cards[SEVEN_TOTAL][ONE_CARD];
    add_to_array(PLAYER_TOTAL, 0, cards_player, seven_cards);
    add_to_array(COMMON_TOTAL, 2, cards_common, seven_cards);
    int combinations[20][FIVE_TOTAL];
    index_combinations(combinations);
    char card_combination[FIVE_TOTAL][ONE_CARD];
    add_to_array(COMMON_TOTAL, 0, cards_common, max_cards);
    int max = evaluate_cards(max_cards);
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < FIVE_TOTAL; j++) {
            int index = combinations[i][j];
            for (int k = 0; k < ONE_CARD; k++) {
                card_combination[j][k] = seven_cards[index][k];
            }
        }
        max = card_maximum(max, card_combination, max_cards);
    }
    return max;
}

int lowest_straight(int int_repr[])
{
    int zero = int_repr[0];
    int one = int_repr[1];
    int ace = get_int_repr('A');
    int five = get_int_repr('5');
    if (zero == ace && one == five) {
        return 1;
    }
    return -1;
}

void find_card_with_type(int type, int result[], int counter[])
{
    int target;
    if (type == FOUR_OF_A_KIND) {
        target = 4;
    } else if (type == THREE_OF_A_KIND || type == FULL_HOUSE) {
        target = 3;
    } else if (type == PAIR || type == TWO_PAIR) {
        target = 2;
    } else {
        target = -1;
    }
    for (int i = 0; i < 13; i++) {
        if (counter[i] == target) {
            result[0] = i;
            break;
        }
    }
    if (type == TWO_PAIR || type == FULL_HOUSE) {
        for (int i = 0; i < 13; i++) {
            if (counter[i] == 2) {
                result[1] = i;
            }
        }
    }
}

void same_value_which_card(char five_cards[][ONE_CARD], int result[])
{
    int counter[13] = { 0 };
    int counter_copy[13] = { 0 };
    for (int i = 0; i < FIVE_TOTAL; i++) {
        int index = get_int_repr(five_cards[i][0]) - 2;
        counter[index]++;
        counter_copy[index]++;
    }
    int new_counter[2] = { 0 };
    int index = 0;
    for (int i = 0; i < 13; i++) {
        if (counter[i] >= 2) {
            new_counter[index] = counter[i];
            result[index] = i;
            index++;
        }
    }
    qsort(new_counter, 2, sizeof(int), cmp_int);
    if (new_counter[0] == 4) {
        find_card_with_type(FOUR_OF_A_KIND, result, counter_copy);
    }
    if (new_counter[0] == 3) {
        if (new_counter[1] == 2) {
            find_card_with_type(FULL_HOUSE, result, counter_copy);
        } else {
            find_card_with_type(THREE_OF_A_KIND, result, counter_copy);
        }
    }
    if (new_counter[0] == 2) {
        if (new_counter[1] == 2) {
            find_card_with_type(TWO_PAIR, result, counter_copy);
        } else {
            find_card_with_type(PAIR, result, counter_copy);
        }
    }
}

void find_card(char five_cards[][ONE_CARD], int same_card, int result[])
{
    int counter = 0;
    for (int i = 0; i < FIVE_TOTAL; i++) {
        int card = get_int_repr(five_cards[i][0]);
        if (card != same_card + 2) {
            result[counter] = card;
            counter++;
        }
    }
}

int compare_two_cards(int fst_int, int snd_int)
{
    if (fst_int > snd_int) {
        return 1;
    }
    if (fst_int < snd_int) {
        return -1;
    }
    return 0;
}

int compare_straight(char fst_comb[][ONE_CARD], char snd_comb[][ONE_CARD])
{
    int fst_int[FIVE_TOTAL];
    int snd_int[FIVE_TOTAL];
    sort_cards(fst_comb, fst_int);
    sort_cards(snd_comb, snd_int);
    int ls_fst = lowest_straight(fst_int);
    int ls_snd = lowest_straight(snd_int);
    if (ls_fst == 1 && ls_snd == 1) {
        return 0;
    }
    if (ls_snd == 1) {
        return 1;
    }
    if (ls_fst == 1) {
        return -1;
    }
    return compare_two_cards(fst_int[0], snd_int[0]);
}

int compare_high_card(int count, int high_card_fst[], int high_card_snd[])
{
    qsort(high_card_fst, count, sizeof(int), cmp_int);
    qsort(high_card_snd, count, sizeof(int), cmp_int);
    for (int i = 0; i < count; i++) {
        int return_value = compare_two_cards(high_card_fst[i], high_card_snd[i]);
        if (return_value != 0) {
            return return_value;
        }
    }
    return 0;
}

int compare_high_card_char(char fst_comb[][ONE_CARD], char snd_comb[][ONE_CARD])
{
    int fst_int[5];
    int snd_int[5];
    for (int i = 0; i < FIVE_TOTAL; i++) {
        fst_int[i] = get_int_repr(fst_comb[i][0]);
        snd_int[i] = get_int_repr(snd_comb[i][0]);
    }
    return compare_high_card(5, fst_int, snd_int);
}

int compare_full_house_two_pair(int type, char fst_comb[][ONE_CARD], char snd_comb[][ONE_CARD])
{
    int result_fst[2];
    int result_snd[2];
    same_value_which_card(fst_comb, result_fst);
    same_value_which_card(snd_comb, result_snd);
    if (type == FULL_HOUSE) {
        for (int i = 0; i < 2; i++) {
            int return_value = compare_two_cards(result_fst[i], result_snd[i]);
            if (return_value != 0) {
                return return_value;
            }
        }
        return 0;
    }
    for (int i = 1; i > -1; i--) {
        int return_value = compare_two_cards(result_fst[i], result_snd[i]);
        if (return_value != 0) {
            return return_value;
        }
    }
    int high_card[2];
    int card[2];
    for (int i = 0; i < FIVE_TOTAL; i++) {
        for (int j = 0; j < 2; j++) {
            if (j == 0) {
                card[j] = get_int_repr(fst_comb[i][0]);
            } else {
                card[j] = get_int_repr(snd_comb[i][0]);
            }
            if (card[j] != result_fst[0] + 2 && card[j] != result_fst[1] + 2) {
                high_card[j] = card[j];
            }
        }
    }
    return compare_two_cards(high_card[0], high_card[1]);
}

int compare_two_three_four(int count, char fst_comb[][ONE_CARD], char snd_comb[][ONE_CARD])
{
    int result_fst[2];
    int result_snd[2];
    same_value_which_card(fst_comb, result_fst);
    same_value_which_card(snd_comb, result_snd);
    int return_value = compare_two_cards(result_fst[0], result_snd[0]);
    if (return_value != 0) {
        return return_value;
    }
    int fc_res_fst[5];
    int fc_res_snd[5];
    find_card(fst_comb, result_fst[0], fc_res_fst);
    find_card(snd_comb, result_snd[0], fc_res_snd);
    return compare_high_card(count, fc_res_fst, fc_res_snd);
}

int compare_two_sets(int type, char fst_comb[][ONE_CARD], char snd_comb[][ONE_CARD])
{
    if (type == STRAIGHT_FLUSH || type == STRAIGHT) {
        return compare_straight(fst_comb, snd_comb);
    }
    if (type == FOUR_OF_A_KIND) {
        return compare_two_three_four(1, fst_comb, snd_comb);
    }
    if (type == FULL_HOUSE) {
        return compare_full_house_two_pair(FULL_HOUSE, fst_comb, snd_comb);
    }
    if (type == THREE_OF_A_KIND) {
        return compare_two_three_four(2, fst_comb, snd_comb);
    }
    if (type == TWO_PAIR) {
        return compare_full_house_two_pair(TWO_PAIR, fst_comb, snd_comb);
    }
    if (type == PAIR) {
        return compare_two_three_four(3, fst_comb, snd_comb);
    }
    if (type == HIGH_CARD || type == FLUSH) {
        return compare_high_card_char(fst_comb, snd_comb);
    }
    return 0;
}

void compare_players(char cards_fst[][ONE_CARD], char cards_snd[][ONE_CARD], char cards_com[][ONE_CARD])
{
    char max_cards_fst[FIVE_TOTAL][ONE_CARD];
    char max_cards_snd[FIVE_TOTAL][ONE_CARD];
    int fst = player_combination_check(cards_fst, cards_com, max_cards_fst);
    int snd = player_combination_check(cards_snd, cards_com, max_cards_snd);
    if (fst > snd) {
        putchar('W');
    } else if (fst < snd) {
        putchar('L');
    } else {
        int result = compare_two_sets(fst, max_cards_fst, max_cards_snd);
        switch (result) {
        case 1:
            putchar('W');
            break;
        case -1:
            putchar('L');
            break;
        case 0:
            putchar('D');
            break;
        }
    }
    putchar('\n');
}
