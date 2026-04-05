#include <iostream>
#include <iomanip>
#include <random>
#include <string>
#include <vector>

using namespace std;

enum class GameResult {
    InProgress,
    AWin,
    BWin,
    Illegal
};

GameResult judgeGameState(int scoreA, int scoreB, int targetScore) {
    if (scoreA < 0 || scoreB < 0) {
        return GameResult::Illegal;
    }
    if (scoreA >= targetScore || scoreB >= targetScore) {
        if (abs(scoreA - scoreB) >= 2) {
            return scoreA > scoreB ? GameResult::AWin : GameResult::BWin;
        }
    }
    return GameResult::InProgress;
}

bool isGameOver(int scoreA, int scoreB, int targetScore) {
    return judgeGameState(scoreA, scoreB, targetScore) != GameResult::InProgress;
}

string formatRuleName(int targetScore) {
    return targetScore == 11 ? "11分制" : "21分制";
}

string createProgressBar(double rate, int width = 40) {
    int filled = static_cast<int>(rate * width + 0.5);
    if (filled > width) filled = width;
    return string(filled, '#') + string(width - filled, '-');
}

double simulateGames(double p, int targetScore, int totalGames, mt19937_64& rng) {
    bernoulli_distribution winA(p);
    int wins = 0;

    for (int game = 0; game < totalGames; ++game) {
        int scoreA = 0;
        int scoreB = 0;

        while (!isGameOver(scoreA, scoreB, targetScore)) {
            if (winA(rng)) {
                scoreA++;
            } else {
                scoreB++;
            }
        }

        if (scoreA > scoreB) {
            wins++;
        }
    }

    return static_cast<double>(wins) / totalGames;
}

void printHeader() {
    cout << "============================================" << endl;
    cout << "       乒乓球赛制模拟器（概率与胜率分析）" << endl;
    cout << "============================================" << endl;
}

void printMenu() {
    cout << "\n菜单：" << endl;
    cout << "1. 模拟 11 分制胜率" << endl;
    cout << "2. 模拟 21 分制胜率" << endl;
    cout << "3. 同时比较 11 分制与 21 分制" << endl;
    cout << "4. 生成一组概率变化结果" << endl;
    cout << "0. 退出" << endl;
    cout << "请选择操作：";
}

double promptProbability() {
    double p;
    while (true) {
        cout << "请输入中国队单分获胜概率 p (0.50 - 0.95): ";
        if (!(cin >> p)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "输入无效，请输入数字。" << endl;
            continue;
        }
        if (p < 0.50 || p > 0.95) {
            cout << "概率必须在 0.50 到 0.95 之间。" << endl;
            continue;
        }
        break;
    }
    return p;
}

int promptTrials() {
    int totalGames;
    while (true) {
        cout << "请输入模拟次数（建议 10000 - 500000）：";
        if (!(cin >> totalGames)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "输入无效，请输入整数。" << endl;
            continue;
        }
        if (totalGames < 1000 || totalGames > 2000000) {
            cout << "模拟次数应在 1000 到 2000000 之间。" << endl;
            continue;
        }
        break;
    }
    return totalGames;
}

void showResult(double winRate, int targetScore) {
    cout << formatRuleName(targetScore) << " 中国队胜率：" << fixed << setprecision(4)
         << winRate * 100 << "%" << endl;
    cout << "胜率条形图： [" << createProgressBar(winRate) << "]" << endl;
}

void compareRules(double p, int totalGames, mt19937_64& rng) {
    double winRate11 = simulateGames(p, 11, totalGames, rng);
    double winRate21 = simulateGames(p, 21, totalGames, rng);

    cout << "\n比较结果：" << endl;
    showResult(winRate11, 11);
    showResult(winRate21, 21);

    double diff = (winRate11 - winRate21) * 100;
    cout << "胜率差距：" << (diff >= 0 ? "+" : "") << fixed << setprecision(4) << diff << " 个百分点" << endl;

    if (abs(diff) < 1e-6) {
        cout << "结论：两种赛制对当前概率影响几乎一致。" << endl;
    } else if (diff > 0) {
        cout << "结论：11分制对中国队更有利。" << endl;
    } else {
        cout << "结论：21分制对中国队更有利。" << endl;
    }
}

void runProbabilitySweep(int totalGames, mt19937_64& rng) {
    vector<double> probabilities = {0.55, 0.60, 0.65, 0.70, 0.75, 0.80};
    cout << "\n=== 不同概率下的模拟结果 ===" << endl;
    cout << setw(10) << "p" << setw(18) << "11分制胜率" << setw(18) << "21分制胜率" << setw(18) << "差距(百分点)" << endl;
    cout << string(64, '-') << endl;

    for (double p : probabilities) {
        double winRate11 = simulateGames(p, 11, totalGames, rng);
        double winRate21 = simulateGames(p, 21, totalGames, rng);
        cout << setw(10) << fixed << setprecision(2) << p
             << setw(18) << fixed << setprecision(4) << winRate11 * 100 << "%"
             << setw(18) << fixed << setprecision(4) << winRate21 * 100 << "%"
             << setw(18) << fixed << setprecision(4) << (winRate11 - winRate21) * 100 << "%" << endl;
    }
}

int main() {
    printHeader();
    mt19937_64 rng(static_cast<unsigned long long>(random_device{}()));

    double p = promptProbability();
    int totalGames = promptTrials();

    while (true) {
        printMenu();
        int choice;
        cin >> choice;
        if (!cin) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "输入无效，请重新选择。" << endl;
            continue;
        }

        if (choice == 0) {
            cout << "程序已退出，感谢使用。" << endl;
            break;
        }

        switch (choice) {
            case 1: {
                double winRate = simulateGames(p, 11, totalGames, rng);
                showResult(winRate, 11);
                break;
            }
            case 2: {
                double winRate = simulateGames(p, 21, totalGames, rng);
                showResult(winRate, 21);
                break;
            }
            case 3:
                compareRules(p, totalGames, rng);
                break;
            case 4:
                runProbabilitySweep(totalGames, rng);
                break;
            default:
                cout << "无效选择，请重新输入。" << endl;
                break;
        }
    }
    return 0;
}
