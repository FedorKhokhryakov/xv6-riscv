#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define SECONDS_PER_MINUTE 60ULL
#define MINUTES_PER_HOUR   60ULL
#define HOURS_PER_DAY      24ULL
#define DAYS_PER_YEAR      365ULL
#define NS_PER_MS          1000000ULL
#define NS_PER_SEC         1000000000ULL
#define EPOCH_YEAR         1970ULL

static const uint days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

static int is_leap_year(int year) {
    if (year % 400 == 0) return 1;
    if (year % 100 == 0) return 0;
    return (year % 4 == 0);
}

static uint32 get_year(uint32 *days_remaining) {
    uint32 year = EPOCH_YEAR;
    while (*days_remaining >= DAYS_PER_YEAR + is_leap_year(year)) {
        *days_remaining -= DAYS_PER_YEAR + is_leap_year(year);
        year++;
    }
    return year;
}

static uint32 get_month(uint32 *days_remaining, uint32 year) {
    uint32 month = 0;
    while (month < 12) {
        uint32 days_in_current_month = days_in_month[month];
        if (month == 1 && is_leap_year(year)) {
            ++days_in_current_month;
        }
        
        if (*days_remaining >= days_in_current_month) {
            *days_remaining -= days_in_current_month;
            ++month;
        } else {
            break;
        }
    }
    return month;
}

static void decompose_time(uint64 nanos, uint32 *sec, uint32 *min, uint32 *hour, uint32 *day, uint32 *nsec, uint32 *msec) {
    *nsec = nanos % NS_PER_SEC;
    *msec = *nsec / NS_PER_MS;
    *sec  = (nanos / NS_PER_SEC) % SECONDS_PER_MINUTE;
    *min  = (nanos / (NS_PER_SEC * SECONDS_PER_MINUTE)) % MINUTES_PER_HOUR;
    *hour = (nanos / (NS_PER_SEC * SECONDS_PER_MINUTE * MINUTES_PER_HOUR)) % HOURS_PER_DAY;
    *day  = nanos / (NS_PER_SEC * SECONDS_PER_MINUTE * MINUTES_PER_HOUR * HOURS_PER_DAY);
}

static void print_two_digits(uint32 num) {
    if (num < 10) printf("0");
    printf("%d", num);
}

static void print_milliseconds(uint32 msec) {
    if (msec < 10)
        printf("00%d", msec);
    else if (msec < 100)
        printf("0%d", msec);
    else
        printf("%d", msec);
}

int main(int argc, char *argv[]) {
    uint64 time_ns = rtc();

    uint32 seconds, minutes, hours, days, nanoseconds, milliseconds;
    decompose_time(time_ns, &seconds, &minutes, &hours, &days, &nanoseconds, &milliseconds);

    uint32 year = get_year(&days);
    uint32 month = get_month(&days, year);

    printf("Date: ");
    print_two_digits(days + 1);
    printf(".");
    print_two_digits(month + 1);
    printf(".%d\n", year);

    printf("Time: ");
    print_two_digits(hours);
    printf(":");
    print_two_digits(minutes);
    printf(":");
    print_two_digits(seconds);
    printf(".");
    print_milliseconds(milliseconds);
    printf("\n");

    exit(0);
}