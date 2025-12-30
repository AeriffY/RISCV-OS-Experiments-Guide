#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TOTAL_ADDR 320     // 虚拟地址数量 320 条
#define PAGE_SIZE 10       // 页面大小，一个页中包含 10 条地址，地址 /10 = 页号
#define PAGE_COUNT 32      // 虚拟页总数 = 320 / 10
#define FRAME_COUNT 4      // 物理块数（内存工作区）

// 虚拟存储区：存放 320 条指令产生的页面号，320 页号
int page_stream[TOTAL_ADDR];

// 内存工作区：8 个物理块，每个块存放一个页号
int memory[FRAME_COUNT];

// 初始化内存：将所有物理块置为空（-1）
void init_memory() {
    for (int i = 0; i < FRAME_COUNT; i++)
        memory[i] = -1; // -1 表示该物理块为空
}

// 打印当前状态 (用于详细展示)
// status: 0=命中, 1=缺页(有空位), 2=缺页(置换)
void print_step(int step, int page, int status, int replaced_page) {

    if (step > 15 && status == 0) return; 

    printf("Step %03d [页%02d] | 内存: [", step, page);
    for (int i = 0; i < FRAME_COUNT; i++) {
        if (memory[i] == -1) printf("  ");
        else printf("%02d", memory[i]);
        if (i < FRAME_COUNT - 1) printf("|");
    }
    printf("] -> ");

    if (status == 0) printf("命中\n");
    else if (status == 1) printf("缺页 (填入)\n");
    else printf("缺页 (置换 %02d)\n", replaced_page);
}


// 生成符合局部性原理的指令流
void generate_address_stream() {
    int pc = rand() % TOTAL_ADDR; // 初始随机位置
    // srand((unsigned)time(NULL));
    
    // 这里的逻辑遵循经典的 Workload 生成模型：
    // 50% 顺序执行，25% 均匀跳转到前面，25% 均匀跳转到后面
    
    for (int i = 0; i < TOTAL_ADDR; i++) {
        page_stream[i] = pc / PAGE_SIZE; // 计算页号 = 地址 / 页面大小

        int branch = rand() % 10; // 0~9，用于决定执行流向
        
        if (branch < 5) { 
            // 50% 概率：顺序执行下一条指令
            pc = (pc + 1) % TOTAL_ADDR;
        } else if (branch < 8) {
            // 25% 概率：向前跳转（在 0 到当前地址之间随机跳转）
            pc = rand() % (pc + 1);
        } else {
            // 25% 概率：向后跳转（在当前地址到末尾之间随机跳转）
            pc = (pc + 1 + (rand() % (TOTAL_ADDR - pc)));
            pc %= TOTAL_ADDR; // 确保不越界 
        }
    }
}

// 判断页面是否在内存中
int is_in_memory(int page) {
    for (int i = 0; i < FRAME_COUNT; i++)
        if (memory[i] == page)
            return i;// 返回页面在内存中的位置
    return -1;// -1 表示页面不在内存中
}

// OPT（最佳置换算法）

int OPT() {
    printf("\n>>> 执行 OPT 算法 (最佳置换)\n");
    init_memory();
    int faults = 0; // 缺页次数统计 

    for (int i = 0; i < TOTAL_ADDR; i++) {
        int page = page_stream[i];

        if (is_in_memory(page) != -1){
            print_step(i, page, 0, -1); // 命中
            continue;  
        }
            

        faults++;  // 缺页

        // 查看是否有空位
        int empty = -1;
        for (int j = 0; j < FRAME_COUNT; j++) {
            if (memory[j] == -1) {
                empty = j;
                break;
            }
        }

        if (empty != -1) {
            // 有空位，直接填入
            memory[empty] = page;
            print_step(i, page, 1, -1);
        }

        else {
            // 找最远才会用到的页
            int max_dist = -1;
            int replace_idx = -1;

            for (int j = 0; j < FRAME_COUNT; j++) {
                int dist = 99999; // 假设不再使用，给一个很大的值
                // 向后查找最近的一次使用
                for (int k = i + 1; k < TOTAL_ADDR; k++) {
                    if (page_stream[k] == memory[j]) {
                        dist = k;// 记录下一次使用的位置
                        break;
                    }
                }
                // 选择最远才会被使用的页面（dist值最大的）
                if (dist > max_dist) {
                    max_dist = dist;
                    replace_idx = j;
                }
            }
            int replaced_val = memory[replace_idx];
            memory[replace_idx] = page;
            print_step(i, page, 2, replaced_val);
        }
    }
    return faults;
}

//  FIFO（先进先出）
int FIFO() {
    printf("\n>>> 执行 FIFO 算法\n");
    init_memory();
    int faults = 0;
    int head = 0;  // FIFO 头部指针，指向最先进入的页（下一个要被置换的页）

    for (int i = 0; i < TOTAL_ADDR; i++) {
        int page = page_stream[i];

        if (is_in_memory(page) != -1){
            print_step(i, page, 0, -1);
            continue;  // 命中
        }

        faults++;

        // 检查空位 (其实 FIFO 只要填满了，后面逻辑是一样的，这里为了展示分开写)
        int empty_idx = -1;
        for(int k=0; k<FRAME_COUNT; k++){
            if(memory[k] == -1) { empty_idx = k; break;}
        }

        if (empty_idx != -1) {
            // 有空位，直接填入
            memory[empty_idx] = page;
            // 此时 head 不需要动，还是指向 0，直到填满
            print_step(i, page, 1, -1);
        } else {
             // 需要置换：置换最先进入的页（head指向的页）
            int replaced_val = memory[head];
            memory[head] = page;
            head = (head + 1) % FRAME_COUNT; // 循环队列，头指针后移 
            print_step(i, page, 2, replaced_val);
        }
    }
    return faults;
}

//  LRU（最近最久未使用）
// 使用 last_used[] 记录每个位置上次使用时间
int LRU() {
    printf("\n>>> 执行 LRU 算法\n");
    init_memory();
    int faults = 0;

    // 记录每个物理块最后一次被访问的时间(指令序号 i)
    int last_used[FRAME_COUNT];
    for (int i = 0; i < FRAME_COUNT; i++)
        last_used[i] = -1;

    for (int i = 0; i < TOTAL_ADDR; i++) {

        int page = page_stream[i];
        int pos = is_in_memory(page);

        if (pos != -1) {
            // 更新最近使用时间
            last_used[pos] = i;
            print_step(i, page, 0, -1);
            continue;
        }

        faults++;

        int empty_idx = -1;
        for (int j = 0; j < FRAME_COUNT; j++) {
            if (memory[j] == -1) {
                empty_idx = j;
                break;
            }
        }

        if (empty_idx != -1) {
            // 有空位，直接填入并记录使用时间
            memory[empty_idx] = page;
            last_used[empty_idx] = i;
            print_step(i, page, 1, -1);
        } else {
            // 需要置换：找最近最久未使用的页（last_used值最小的）
            int min_time = 999999;
            int replace_idx = -1;
            
            for (int j = 0; j < FRAME_COUNT; j++) {
                if (last_used[j] < min_time) {
                    min_time = last_used[j];
                    replace_idx = j;
                }
            }
            
            int replaced_val = memory[replace_idx];
            memory[replace_idx] = page;
            last_used[replace_idx] = i;// 更新新页面的使用时间 
            print_step(i, page, 2, replaced_val);
        }
    }
    return faults;
}

//  主程序
int main() {

    printf("生成模拟数据中...\n");
    printf("参数配置: 总指令=320, 页大小=10 (总页数32), 物理块=4\n");
    generate_address_stream();

    printf("页面序列前20项: ");
    for(int i=0; i<20; i++) printf("%d ", page_stream[i]);
    printf("...\n");

    int f_opt = OPT();
    int f_fifo = FIFO();
    int f_lru = LRU();

    printf("\n==== 页面置换实验结果 ====\n");
    printf("总指令数: %d\n", TOTAL_ADDR);
    printf("OPT  缺页次数: %d   缺页率: %.2f%%   命中率: %.2f%%\n",
           f_opt, (float)f_opt / TOTAL_ADDR * 100, 100 - (float)f_opt / TOTAL_ADDR * 100);

    printf("FIFO 缺页次数: %d   缺页率: %.2f%%   命中率: %.2f%%\n",
           f_fifo, (float)f_fifo / TOTAL_ADDR * 100, 100 - (float)f_fifo / TOTAL_ADDR * 100);

    printf("LRU  缺页次数: %d   缺页率: %.2f%%   命中率: %.2f%%\n",
           f_lru, (float)f_lru / TOTAL_ADDR * 100, 100 - (float)f_lru / TOTAL_ADDR * 100);

    return 0;
}
