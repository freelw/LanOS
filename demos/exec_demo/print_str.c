#define MAX_L 25
#define SCREEN_LEN 2000
char screen_buff[SCREEN_LEN];
int cur_l = 0;
extern void write_char(char ch, int pos);
void validate_screen()
{
    for (int i = 0; i < SCREEN_LEN; ++ i) {
        write_char(screen_buff[i], i);
    }
}

void print_str(char *s)
{
    if (cur_l >= MAX_L) {
        for (int i = 0; i < 24; ++ i) {
            for (int j = 0; j < 80; ++ j) {
                screen_buff[i*80+j] = screen_buff[(i+1)*80+j];
            }
        }
        for (int j = 0; j < 80; ++ j) {
            screen_buff[24*80 + j] = 0;
        }
        cur_l = MAX_L-1;
    }
    int y = cur_l;
    int x = 0;
    while (s[x] != 0) {
        screen_buff[y*80+x] = s[x];
        x++;
    }
    cur_l ++;
    validate_screen();
}

void print_num(int num)
{
    char buf[64];
    int index = 0;
    for (int i = 0; i < 64; ++ i) {
        buf[i] = 0;
    }
    if (num == 0) {
        buf[index++] = '0';
    } else if (num < 0) {
        num = -num;
        buf[index++] = '-';
    }
    char tmp[64];
    int index_tmp = 0;
    while (num > 0) {
        tmp[index_tmp ++] = num%10 + '0';
        num /= 10;
    }
    for (int i = index_tmp-1; i >= 0; -- i) {
        buf[index++] = tmp[i];
    }
    print_str(buf);
}

