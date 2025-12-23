#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#include <io.h>
#endif

#define MAX_LEARNERS 100
#define NAME_LEN 100

typedef struct {
    int id;
    char name[NAME_LEN];
    int age;
    float gpa;
} Learner;

/* Global flag: is stdout a terminal? (not redirected) */
static int g_output_is_tty = 1;

/* ========== HÀM HIỂN THỊ ========== */

static void printHeader(void) {
    printf("+------+------------------------------+-----+-----+\n");
    printf("| %-4s | %-28s | %-3s | %-3s |\n", "ID", "Name", "Age", "GPA");
    printf("+------+------------------------------+-----+-----+\n");
}

static void printRow(const Learner *s) {
    printf("| %-4d | %-28s | %-3d | %-3.2f |\n", s->id, s->name, s->age, s->gpa);
}

static void printFooter(void) {
    printf("+------+------------------------------+-----+-----+\n");
}

static void displayLearnerList(const Learner *learners, int count) {
    if (count <= 0) {
        printf("\nDanh sách sinh viên rỗng.\n");
        return;
    }
    printf("\n");
    printHeader();
    for (int i = 0; i < count; i++) {
        printRow(&learners[i]);
    }
    printFooter();
}

static void displayMenu(void) {
    printf("\n");
    if (g_output_is_tty) {
        printf("╔══════════════════════════════════════════════╗\n");
        printf("║     CHƯƠNG TRÌNH QUẢN LÝ DAN SÁCH SINH VIÊN  ║\n");
        printf("╚══════════════════════════════════════════════╝\n");
    } else {
        printf("==============================================\n");
        printf("  CHUONG TRINH QUAN LY DANH SACH SINH VIEN    \n");
        printf("==============================================\n");
    }
    printf("\n1. Nhập sinh viên từ bàn phím\n");
    printf("2. In danh sách sinh viên\n");
    printf("3. Ghi danh sách vào file (students.txt)\n");
    printf("4. Đọc danh sách từ file lớp (classA/B/C.txt)\n");
    printf("5. Tìm kiếm sinh viên theo ID\n");
    printf("6. Sắp xếp sinh viên theo GPA giảm dần\n");
    printf("7. Thêm sinh viên mới\n");
    printf("8. Xóa sinh viên\n");
    printf("9. Tính GPA trung bình lớp\n");
    printf("0. Thoát chương trình\n");
    printf("\nChọn chức năng (0-9): ");
}

/* ========== HÀM FILE I/O ========== */

static int saveToFile(const Learner *learners, int n) {
    FILE *f = fopen("d:\\ss8\\students.txt", "w");
    if (!f) {
        fprintf(stderr, "Loi: Khong the tao/mo file students.txt.\n");
        return 0;
    }

    fprintf(f, "+------+------------------------------+-----+-----+\n");
    fprintf(f, "| %-4s | %-28s | %-3s | %-3s |\n", "ID", "Name", "Age", "GPA");
    fprintf(f, "+------+------------------------------+-----+-----+\n");
    for (int i = 0; i < n; i++) {
        fprintf(f, "| %-4d | %-28s | %-3d | %-3.2f |\n",
                learners[i].id, learners[i].name, learners[i].age, learners[i].gpa);
    }
    fprintf(f, "+------+------------------------------+-----+-----+\n");
    fclose(f);
    return 1;
}

static const char* promptClassFilename(void) {
    static char filename[100];
    printf("\n=== CHỌN FILE LỚP ===\n");
    printf("Sẵn có: classA.txt, classB.txt, classC.txt\n");
    printf("Nhập tên file (ví dụ: classA.txt): ");
    if (fgets(filename, sizeof(filename), stdin) == NULL) {
        return NULL;
    }
    size_t len = strlen(filename);
    if (len > 0 && filename[len - 1] == '\n') {
        filename[len - 1] = '\0';
    }
    return filename;
}

/*
 * readLearnerDataFromFile - Đọc dữ liệu sinh viên từ file
 * Path: đường dẫn file (VD: "d:\\ss8\\classA.txt")
 * Format: id;name;age;gpa
 * Trả về: số lượng sinh viên đọc được hoặc -1 nếu lỗi
 */
static int readLearnerDataFromFile(const char *path, Learner *learners, int max) {
    FILE *f = fopen(path, "r");
    if (!f) {
        fprintf(stderr, "Loi: Khong the mo file '%s'.\n", path);
        return -1;
    }

    char line[256];
    int count = 0;

    while (fgets(line, sizeof(line), f) != NULL && count < max) {
        int id = 0, age = 0;
        float gpa = 0.0f;
        char namebuf[NAME_LEN] = {0};

        size_t L = strlen(line);
        if (L > 0 && line[L - 1] == '\n') {
            line[L - 1] = '\0';
        }

        int matched = sscanf(line, "%d;%99[^;];%d;%f", &id, namebuf, &age, &gpa);
        if (matched == 4) {
            learners[count].id = id;
            strncpy(learners[count].name, namebuf, NAME_LEN - 1);
            learners[count].name[NAME_LEN - 1] = '\0';
            learners[count].age = age;
            learners[count].gpa = gpa;
            count++;
        }
    }

    fclose(f);
    if (g_output_is_tty)
        printf("✓ Đọc thành công %d sinh viên từ file '%s'.\n", count, path);
    else
        printf("[OK] Đã đọc %d sinh viên từ file '%s'.\n", count, path);
    return count;
}

/* ========== HÀM TÍNH TOÁN ========== */

static float calculateAverageGPA(const Learner *learners, int count) {
    if (count <= 0) return 0.0f;
    
    float sum = 0.0f;
    for (int i = 0; i < count; i++) {
        sum += learners[i].gpa;
    }
    return sum / count;
}

static void printAverageGPA(const Learner *learners, int count) {
    if (count <= 0) {
        printf("\nThông báo: Danh sách sinh viên rỗng.\n");
        return;
    }
    float avg = calculateAverageGPA(learners, count);
    printf("\nGPA trung bình của lớp: %.2f\n", avg);
}

/* ========== HÀM SẮPXẾP ========== */

static void sortLearnersByGPADesc(Learner *learners, int count) {
    if (count <= 1) return;
    
    for (int i = 0; i < count - 1; i++) {
        int maxIdx = i;
        for (int j = i + 1; j < count; j++) {
            if (learners[j].gpa > learners[maxIdx].gpa) {
                maxIdx = j;
            }
        }
        if (maxIdx != i) {
            Learner temp = learners[i];
            learners[i] = learners[maxIdx];
            learners[maxIdx] = temp;
        }
    }
}

static void sortAndDisplay(Learner *learners, int count) {
    if (count <= 0) {
        printf("\nDanh sách sinh viên rỗng, không thể sắp xếp.\n");
        return;
    }
    
    Learner tempList[MAX_LEARNERS];
    for (int i = 0; i < count; i++) {
        tempList[i] = learners[i];
    }
    
    printf("\nDanh sách trước sắp xếp:\n");
    displayLearnerList(tempList, count);
    
    sortLearnersByGPADesc(tempList, count);
    
    printf("\nDanh sách sau sắp xếp (GPA giảm dần):\n");
    displayLearnerList(tempList, count);
}

/* ========== HÀM TÌM KIẾM ========== */

static int searchLearnerByID(const Learner *learners, int count, int id) {
    if (id < 0) return -1;
    for (int i = 0; i < count; i++) {
        if (learners[i].id == id) return i;
    }
    return -1;
}

static int promptSearchID(void) {
    int id;
    printf("\nNhập ID cần tìm: ");
    if (scanf("%d", &id) != 1) {
        while (getchar() != '\n');
        return -1;
    }
    while (getchar() != '\n');
    return id;
}

static void findLearnerInteractive(const Learner *learners, int count) {
    int searchId = promptSearchID();
    if (searchId >= 0) {
        int found = searchLearnerByID(learners, count, searchId);
        printf("\nKết quả tìm kiếm:\n");
        if (found >= 0) {
            printf("✓ Tìm thấy:\n");
            printf("  ID: %d | Tên: %s | Tuổi: %d | GPA: %.2f\n",
                   learners[found].id, learners[found].name, learners[found].age, learners[found].gpa);
        } else {
            printf("✗ Không tìm thấy sinh viên với ID %d.\n", searchId);
        }
    } else {
        printf("ID không hợp lệ.\n");
    }
}

/* ========== HÀM THÊM/XÓA ========== */

static int addLearnerByID(Learner *learners, int *count, const Learner *newLearner, int maxSize) {
    if (count == NULL || newLearner == NULL) return -2;
    
    /* Kiểm tra ID trùng */
    for (int i = 0; i < *count; i++) {
        if (learners[i].id == newLearner->id) {
            fprintf(stderr, "Loi: ID %d da ton tai trong danh sach.\n", newLearner->id);
            return -1;
        }
    }
    
    /* Kiểm tra dung lượng */
    if (*count >= maxSize) {
        fprintf(stderr, "Loi: Danh sach da day (toi da %d sinh vien).\n", maxSize);
        return -2;
    }
    
    /* Thêm sinh viên mới vào cuối danh sách */
    learners[*count] = *newLearner;
    (*count)++;
    if (g_output_is_tty)
        printf("✓ Them sinh vien ID %d thanh cong.\n", newLearner->id);
    else
        printf("[OK] Them sinh vien ID %d thanh cong.\n", newLearner->id);
    return 1;
}

static int deleteLearnerByID(Learner *learners, int *count, int idToDelete) {
    if (count == NULL) return -2;
    
    /* Tìm vị trí sinh viên cần xóa */
    int foundIdx = -1;
    for (int i = 0; i < *count; i++) {
        if (learners[i].id == idToDelete) {
            foundIdx = i;
            break;
        }
    }
    
    /* Nếu không tìm thấy */
    if (foundIdx == -1) {
        fprintf(stderr, "Loi: ID %d khong ton tai trong danh sach.\n", idToDelete);
        return -1;
    }
    
    /* Dịch chuyển các phần tử sau lên trước */
    for (int i = foundIdx; i < *count - 1; i++) {
        learners[i] = learners[i + 1];
    }
    
    (*count)--;
    if (g_output_is_tty)
        printf("✓ Xoa sinh vien ID %d thanh cong.\n", idToDelete);
    else
        printf("[OK] Xoa sinh vien ID %d thanh cong.\n", idToDelete);
    return 1;
}

/* ========== HÀM NHẬP LIỆU ========== */

static void inputNewLearner(Learner *learners, int *count, int maxSize) {
    if (count == NULL || *count >= maxSize) {
        fprintf(stderr, "Loi: Danh sach day hoac tham so khong hop le.\n");
        return;
    }
    
    Learner newLearner;
    printf("\n--- Nhập thông tin sinh viên mới ---\n");
    
    printf("ID: ");
    if (scanf("%d", &newLearner.id) != 1) {
        fprintf(stderr, "Loi: ID khong hop le.\n");
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');
    
    printf("Tên: ");
    if (fgets(newLearner.name, NAME_LEN, stdin) == NULL) {
        newLearner.name[0] = '\0';
    } else {
        size_t len = strlen(newLearner.name);
        if (len > 0 && newLearner.name[len - 1] == '\n')
            newLearner.name[len - 1] = '\0';
    }
    
    printf("Tuổi: ");
    if (scanf("%d", &newLearner.age) != 1) {
        fprintf(stderr, "Loi: Tuoi khong hop le.\n");
        while (getchar() != '\n');
        return;
    }
    
    printf("GPA: ");
    if (scanf("%f", &newLearner.gpa) != 1) {
        fprintf(stderr, "Loi: GPA khong hop le.\n");
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');
    
    addLearnerByID(learners, count, &newLearner, maxSize);
}

static void deleteLearnerInteractive(Learner *learners, int *count) {
    if (count == NULL) return;
    
    printf("\nNhập ID sinh viên cần xóa: ");
    int idToDelete;
    if (scanf("%d", &idToDelete) != 1) {
        fprintf(stderr, "Loi: ID khong hop le.\n");
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');
    
    deleteLearnerByID(learners, count, idToDelete);
}

static void loadLearnerFromFile(Learner *learners, int *count, int maxSize) {
    if (count == NULL) return;
    
    const char *classFile = promptClassFilename();
    if (classFile == NULL || strlen(classFile) == 0) {
        fprintf(stderr, "Tên file không hợp lệ.\n");
        return;
    }
    
    char fullPath[256];
    snprintf(fullPath, sizeof(fullPath), "d:\\ss8\\%s", classFile);
    
    int m = readLearnerDataFromFile(fullPath, learners, maxSize);
    if (m >= 0) {
        *count = m;
    }
}

/* ========== HÀM KHỞI TẠO ========== */

static void initializeSampleLearners(Learner *learners, int *maxCount) {
    if (maxCount == NULL) return;
    
    learners[0] = (Learner){1001, "Nguyen Van An", 20, 8.5f};
    learners[1] = (Learner){1002, "Tran Thi Binh", 19, 9.0f};
    learners[2] = (Learner){1003, "Le Quang Cuong", 21, 7.5f};
    learners[3] = (Learner){1004, "Pham Thi Duc", 20, 8.2f};
    learners[4] = (Learner){1005, "Hoang Van Em", 22, 7.8f};
    learners[5] = (Learner){1006, "Vu Thi Phuong", 19, 8.8f};
    learners[6] = (Learner){1007, "Dang Van Giang", 21, 6.5f};
    learners[7] = (Learner){1008, "Bui Thi Huong", 20, 8.3f};
    learners[8] = (Learner){1009, "Trinh Van Khanh", 19, 9.2f};
    learners[9] = (Learner){1010, "Nguyen Thi Linh", 22, 7.0f};
    
    *maxCount = 10;
    if (g_output_is_tty)
        printf("✓ Đã khởi tạo 10 sinh viên mẫu.\n");
    else
        printf("[OK] Đã khởi tạo 10 sinh viên mẫu.\n");
}

/* ========== HÀM CHÍNH ========== */

int main(void) {
    Learner learners[MAX_LEARNERS];
    int count = 0;
    /* Trên Windows: đặt code page sang UTF-8 để hiển thị tiếng Việt và ký tự khung */
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    /* Detect if stdout is a terminal (not redirected). If it is redirected, disable buffering
       so piped/captured runs receive immediate output and use ASCII fallbacks. */
#ifdef _WIN32
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wimplicit-function-declaration"
    {
        int fd = _fileno(stdout);
        g_output_is_tty = (fd >= 0) ? _isatty(fd) : 0;
        if (!g_output_is_tty) {
            setvbuf(stdout, NULL, _IONBF, 0);
        }
    }
#pragma GCC diagnostic pop
#else
    /* On non-Windows systems use isatty if available */
    /* default to true for safety */
#endif

    if (g_output_is_tty) {
        printf("╔════════════════════════════════════════════════╗\n");
        printf("║   HỆ THỐNG QUẢN LÝ DAN SÁCH SINH VIÊN v2.0   ║\n");
        printf("╚════════════════════════════════════════════════╝\n");
    } else {
        printf("===============================================\n");
        printf("  HE THONG QUAN LY DANH SACH SINH VIEN v2.0   \n");
        printf("===============================================\n");
    }
    
    initializeSampleLearners(learners, &count);
    
    int choice = -1;
    while (choice != 0) {
        displayMenu();
        
        if (scanf("%d", &choice) != 1) {
            fprintf(stderr, "Loi: Lua chon khong hop le.\n");
            while (getchar() != '\n');
            continue;
        }
        while (getchar() != '\n');
        
        switch (choice) {
            case 1:
                inputNewLearner(learners, &count, MAX_LEARNERS);
                break;
            case 2:
                displayLearnerList(learners, count);
                break;
            case 3:
                if (saveToFile(learners, count)) {
                    printf("✓ Danh sách đã được lưu vào students.txt.\n");
                } else {
                    fprintf(stderr, "Loi: Khong the luu danh sach.\n");
                }
                break;
            case 4:
                loadLearnerFromFile(learners, &count, MAX_LEARNERS);
                break;
            case 5:
                findLearnerInteractive(learners, count);
                break;
            case 6:
                sortAndDisplay(learners, count);
                break;
            case 7:
                inputNewLearner(learners, &count, MAX_LEARNERS);
                break;
            case 8:
                deleteLearnerInteractive(learners, &count);
                break;
            case 9:
                printAverageGPA(learners, count);
                break;
            case 0:
                printf("\n");
                if (g_output_is_tty) {
                    printf("╔════════════════════════════════════════════════╗\n");
                    printf("║         Cảm ơn đã sử dụng chương trình!         ║\n");
                    printf("║             Tạm biệt và hẹn gặp lại!            ║\n");
                    printf("╚════════════════════════════════════════════════╝\n");
                } else {
                    printf("===============================================\n");
                    printf("  Cam on da su dung chuong trinh! Tam biet.     \n");
                    printf("===============================================\n");
                }
                break;
            default:
                fprintf(stderr, "Loi: Lua chon khong hop le. Vui long chon 0-9.\n");
                break;
        }
    }
    
    return 0;
}

