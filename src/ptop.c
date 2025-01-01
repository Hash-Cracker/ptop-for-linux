#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <unistd.h>
#include <dirent.h>
#include <ctype.h>
#include <ncurses.h>

#define CPU_INFO_FILE "/proc/stat"
#define MEM_INFO_FILE "/proc/meminfo"

// Nord theme vibrant colors
enum {
    NORD_BG = 1,
    NORD_HEADER,
    NORD_TEXT,
    NORD_HIGHLIGHT,
    NORD_RED,
    NORD_GREEN,
    NORD_YELLOW,
    NORD_BLUE,
    NORD_MAGENTA,
    NORD_CYAN
};

void init_colors() {
    start_color();
    init_pair(NORD_BG, COLOR_BLACK, COLOR_BLACK);
    init_pair(NORD_HEADER, COLOR_CYAN, COLOR_BLACK);
    init_pair(NORD_TEXT, COLOR_WHITE, COLOR_BLACK);
    init_pair(NORD_HIGHLIGHT, COLOR_BLUE, COLOR_BLACK);
    init_pair(NORD_RED, COLOR_RED, COLOR_BLACK);
    init_pair(NORD_GREEN, COLOR_GREEN, COLOR_BLACK);
    init_pair(NORD_YELLOW, COLOR_YELLOW, COLOR_BLACK);
    init_pair(NORD_BLUE, COLOR_BLUE, COLOR_BLACK);
    init_pair(NORD_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(NORD_CYAN, COLOR_CYAN, COLOR_BLACK);
}

void get_cpu_usage(double *cpu_usage) {
    static long prev_idle = 0, prev_total = 0;
    long user, nice, system, idle, iowait, irq, softirq, steal;
    long total, total_diff, idle_diff;
    FILE *fp = fopen(CPU_INFO_FILE, "r");

    if (fp == NULL) {
        *cpu_usage = 0.0;
        return;
    }

    fscanf(fp, "cpu %ld %ld %ld %ld %ld %ld %ld %ld",
           &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal);
    fclose(fp);

    total = user + nice + system + idle + iowait + irq + softirq + steal;
    idle_diff = idle - prev_idle;
    total_diff = total - prev_total;

    if (total_diff != 0) {
        *cpu_usage = 100.0 * (1.0 - ((double)idle_diff / total_diff));
    }

    prev_idle = idle;
    prev_total = total;
}

void get_memory_usage(long *used_memory, long *free_memory, long *total_memory) {
    FILE *fp = fopen(MEM_INFO_FILE, "r");
    char line[256];
    *total_memory = 0;
    *free_memory = 0;
    long buffers = 0, cached = 0;

    if (fp == NULL) {
        *used_memory = 0;
        return;
    }

    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "MemTotal:", 9) == 0) {
            sscanf(line + 9, "%ld", total_memory);
        } else if (strncmp(line, "MemFree:", 8) == 0) {
            sscanf(line + 8, "%ld", free_memory);
        } else if (strncmp(line, "Buffers:", 8) == 0){
            sscanf(line + 8, "%ld", &buffers);          
        } else if (strncmp(line, "Cached:", 7) == 0){
            sscanf(line + 7, "%ld", &cached);
        }
    }
    fclose(fp);

    if (*total_memory > 0) {
        *used_memory = *total_memory - *free_memory - buffers - cached;
    }
}

void get_uptime(long *uptime) {
    struct sysinfo info;
    if (sysinfo(&info) == 0) {
        *uptime = info.uptime;
    } else {
        *uptime = 0;
    }
}

void list_running_processes(int start_line) {
    DIR *proc_dir = opendir("/proc");
    struct dirent *entry;

    if (proc_dir == NULL) {
        perror("Error opening /proc");
        return;
    }

    int line = start_line;

    attron(COLOR_PAIR(NORD_HEADER));
    mvprintw(line++, 1, "PID       Process Name");
    mvprintw(line++, 1, "=======================");
    attroff(COLOR_PAIR(NORD_HEADER));

    int color_toggle = NORD_RED;
    while ((entry = readdir(proc_dir)) != NULL) {
        if (entry->d_type == DT_DIR && isdigit(entry->d_name[0])) {
            char path[256], process_name[256];
            snprintf(path, sizeof(path), "/proc/%s/comm", entry->d_name);

            FILE *fp = fopen(path, "r");
            if (fp) {
                if (fgets(process_name, sizeof(process_name), fp)) {
                    process_name[strcspn(process_name, "\n")] = 0;
                    attron(COLOR_PAIR(color_toggle));
                    mvprintw(line++, 1, "%-10s %s", entry->d_name, process_name);
                    attroff(COLOR_PAIR(color_toggle));

                    // Cycle through vibrant colors
                    color_toggle = (color_toggle == NORD_CYAN) ? NORD_RED : color_toggle + 1;
                }
                fclose(fp);
            }
        }
    }

    closedir(proc_dir);
}

int main() {
    initscr();             
    noecho();              
    curs_set(0);           
    timeout(0);            

    init_colors();         // Initialize Nord theme colors
    bkgd(COLOR_PAIR(NORD_BG));

    int refresh_rate = 2000; 

    double cpu_usage = 0.0;
    long used_memory = 0, free_memory = 0, total_memory = 0;
    long uptime = 0;

    while (1) {
        get_cpu_usage(&cpu_usage);
        get_memory_usage(&used_memory, &free_memory, &total_memory);
        get_uptime(&uptime);

        clear();

        attron(COLOR_PAIR(NORD_HEADER));
        mvprintw(1, 1, "Simple System Monitor");
        mvprintw(2, 1, "=====================");
        attroff(COLOR_PAIR(NORD_HEADER));

        attron(COLOR_PAIR(NORD_TEXT));
        mvprintw(4, 1, "CPU Usage: %.2f%%", cpu_usage);
        mvprintw(5, 1, "Memory Usage: %.2f GB / %.2f GB", used_memory / 1048576.0, total_memory / 1048576.0);
        mvprintw(6, 1, "System Uptime: %ld seconds", uptime);
        attroff(COLOR_PAIR(NORD_TEXT));

        list_running_processes(8);

        attron(COLOR_PAIR(NORD_HIGHLIGHT));
        mvprintw(LINES - 2, 1, "Press 'q' to quit.");
        attroff(COLOR_PAIR(NORD_HIGHLIGHT));

        refresh();

        int ch = getch();
        if (ch == 'q' || ch == 'Q') {
            break;
        }

        usleep(refresh_rate * 1000);
    }

    endwin();
    return 0;
}

