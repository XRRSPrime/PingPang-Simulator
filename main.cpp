#include "student.h"
#include <iostream>
#include <limits>

void printMenu() {
    cout << "\n=== 学生管理系统 ===\n";
    cout << "1. 添加/更新学生信息 (学号 名字 成绩1 成绩2 成绩3)\n";
    cout << "2. 修改学生信息 (学号 成绩1 成绩2 成绩3 名字)\n";
    cout << "3. 删除学生 (学号)\n";
    cout << "4. 按学号查询学生 (学号)\n";
    cout << "5. 按姓名查询学生 (姓名或部分姓名)\n";
    cout << "6. 显示按学号排序列表\n";
    cout << "7. 显示按总分排序列表\n";
    cout << "0. 退出\n";
    cout << "请选择操作：";
}

int main() {
    int select;
    Student* numberList = new Student();
    Student* scoreList = new Student();
    numberList->next = nullptr;
    scoreList->next = nullptr;
    string number, name;

    printMenu();
    while (cin >> select && select != 0) {
        switch (select) {
            case 1:
                addStudent(numberList, scoreList);
                break;
            case 2:
                modify(numberList, scoreList);
                break;
            case 3:
                cin >> number;
                erase(number, numberList, scoreList);
                break;
            case 4:
                cin >> number;
                findByNumber(number, numberList);
                break;
            case 5:
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                getline(cin, name);
                findByName(name, numberList);
                break;
            case 6:
                CoutInformation(numberList);
                break;
            case 7:
                CoutInformation(scoreList);
                break;
            default:
                cout << "无效选择，请重新输入。" << endl;
                break;
        }
        printMenu();
    }

    clearStudentList(numberList);
    clearStudentList(scoreList);
    delete numberList;
    delete scoreList;
    cout << "已退出学生管理系统。" << endl;
    return 0;
}
