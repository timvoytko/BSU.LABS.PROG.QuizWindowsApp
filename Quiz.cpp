// Quiz.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "Quiz.h"
#pragma warning(disable:4996)
#define MAX_LOADSTRING 100

struct QuizShow {
    int currentquestion;
    // question+Uni/Multiple
    std::vector<std::pair<std::string, bool>> questions;
    // anwsers+true or false
    std::vector<std::vector<std::pair<std::string, bool>>> answers;
    std::vector<bool> useranswer;
    QuizShow(std::string filename) {
        std::ifstream in(filename);
        if (!in.is_open())abort();
        //file:
        //Question
        //Unique(0) or multiple(1)
        //Number of Anwsers 
        //anwser,1 sumb from(0,1) 1==True 0==False
        while (!in.eof()) {
            std::string tmp1, tmp2;
            std::getline(in, tmp1);
            std::getline(in, tmp2);
            questions.push_back({ tmp1,std::stoi(tmp2) });
            std::getline(in, tmp1);
            std::vector<std::pair<std::string, bool>> anwser;
            for (int i = 0; i < std::stoi(tmp1); i++) {
                std::getline(in, tmp2);
                bool is_true = tmp2[0] - '0';
                tmp2.erase(tmp2.begin());
                anwser.push_back({ tmp2,is_true });
            }
            answers.push_back(anwser);
        }
        useranswer.resize(questions.size());
    }
    void CheckAnwser(const std::vector<bool>& answer) {
        useranswer[currentquestion] = 1;
        for (int i = 0; i < answers[currentquestion].size(); i++) {
            if (answer[i] != answers[currentquestion][i].second) {
                useranswer[currentquestion] = 0;
            }
        }
    }
    double GetStat() {
        double res = 0;
        for (int i = 0; i < size(useranswer); i++) {
            res += useranswer[i];
        }
        return res / size(useranswer) * 100;
    }
};
// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна
std::vector<bool>current_answer(5);
std::string filename;
// Отправить объявления функций, включенных в этот модуль кода:
void RadioQB(HWND hDlg, const std::vector<std::pair<std::string, bool>>& answer,int quest) {
    for (int i = 0; i < 5; i++) {
        ShowWindow(GetDlgItem(hDlg, IDC_CHECK1 + i), SW_HIDE);
        ShowWindow(GetDlgItem(hDlg, IDC_RADIO1 + i), 1);
    }
    for (int i = 0; i < answer.size(); i++) {
        SetDlgItemText(hDlg, IDC_RADIO1 + i, answer[i].first.c_str());
    }
    for (int i = answer.size(); i < 5; i++) {
        ShowWindow(GetDlgItem(hDlg, IDC_RADIO1+i), SW_HIDE);
    }
}
void CheckQB(HWND hDlg, const std::vector<std::pair<std::string, bool>>& answer, int quest) {
    for (int i = 0; i < 5; i++) {
        ShowWindow(GetDlgItem(hDlg, IDC_RADIO1 + i), SW_HIDE);
        ShowWindow(GetDlgItem(hDlg, IDC_CHECK1 + i), 1);
    }
    for (int i = 0; i < answer.size(); i++) {
        SetDlgItemText(hDlg, IDC_CHECK1 + i, answer[i].first.c_str());
    }
    for (int i = answer.size(); i < 5; i++) {
        ShowWindow(GetDlgItem(hDlg, IDC_CHECK1 + i), SW_HIDE);
    }
}
INT_PTR CALLBACK    QuizMain(HWND, UINT, WPARAM, LPARAM);


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    OPENFILENAME ofn;       // common dialog box structure
    char szFile[260];       // buffer for file name
  //  HANDLE hf;              // file handle

    // Initialize OPENFILENAME
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    // Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
    // use the contents of szFile to initialize itself.
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "All\0*.*\0Text\0*.TXT\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    // Display the Open dialog box. 

    if (GetOpenFileName(&ofn) == TRUE) {
        filename = szFile;
        DialogBox(hInst, MAKEINTRESOURCE(IDD_QUIZ), NULL, QuizMain);

    }
}




INT_PTR CALLBACK QuizMain(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    setlocale(LC_ALL, 0);
    static std::string tmp;
    wchar_t tmptext[1000];
    UNREFERENCED_PARAMETER(lParam);
    static QuizShow quiz(filename);
    static std::ofstream of;
    switch (message)
    {
    case WM_INITDIALOG:
        quiz.currentquestion = 0;
        mbstowcs(tmptext, quiz.questions[quiz.currentquestion].first.c_str(), quiz.questions[quiz.currentquestion].first.length());
        SetDlgItemText(hDlg, IDC_STATIC, quiz.questions[quiz.currentquestion].first.c_str());
        ShowWindow(GetDlgItem(hDlg, IDC_LEFT), SW_HIDE);
        if (quiz.questions[quiz.currentquestion].second) {
            CheckQB(hDlg, quiz.answers[quiz.currentquestion], quiz.currentquestion);
        }
        else {
            RadioQB(hDlg, quiz.answers[quiz.currentquestion], quiz.currentquestion);
        }
        return (INT_PTR)TRUE;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDEXIT:
        case IDCANCEL:
            {
                EndDialog(hDlg, LOWORD(wParam));
                return (INT_PTR)TRUE;
            }
        case IDRESULT:
             tmp = std::to_string(quiz.GetStat()) + "% You are amazing!\n";
             for (int i = 0; i < quiz.useranswer.size(); i++) {
                 if (quiz.useranswer[i] == 1) {
                     tmp +="   "+ std::to_string(i+1) + " + \n";
                 }
                 else {
                     tmp += "   " + std::to_string(i+1) + " - \n";
                 }
             }
             of.open("result.txt");
             if (!of.is_open())abort();
             of << tmp;
            MessageBox(hDlg, tmp.c_str(), "", MB_OK);
            break;
        case IDC_RADIO1:
        case IDC_RADIO2:
        case IDC_RADIO3:
        case IDC_RADIO4:
        case IDC_RADIO5:
            for (int i = 0; i < 5; i++) {
                current_answer[i]= IsDlgButtonChecked(hDlg, IDC_RADIO1+i);
            }
            break;
        case IDC_CHECK1:
        case IDC_CHECK2:
        case IDC_CHECK3:
        case IDC_CHECK4:
        case IDC_CHECK5:
            for (int i = 0; i < 5; i++) {
                current_answer[i] = IsDlgButtonChecked(hDlg, IDC_CHECK1 + i);
            }
            break;
        case IDC_LEFT:
            current_answer.resize(quiz.answers[quiz.currentquestion].size());
            quiz.CheckAnwser(current_answer);
            quiz.currentquestion--;
            SetDlgItemText(hDlg, IDC_STATIC, quiz.questions[quiz.currentquestion].first.c_str());
            ShowWindow(GetDlgItem(hDlg, IDC_RIGHT), 1);
            if (quiz.currentquestion == 0) {
                ShowWindow(GetDlgItem(hDlg, IDC_LEFT), SW_HIDE);
            }
            for (int i = 0; i < 5; i++) {
                SendDlgItemMessage(hDlg, IDC_RADIO1+i, BM_SETCHECK, BST_UNCHECKED, 0);
                SendDlgItemMessage(hDlg, IDC_CHECK1 + i, BM_SETCHECK, BST_UNCHECKED, 0);
            }
            
            if (quiz.questions[quiz.currentquestion].second) {
                CheckQB(hDlg, quiz.answers[quiz.currentquestion], quiz.currentquestion);
            }
            else {
                RadioQB(hDlg, quiz.answers[quiz.currentquestion], quiz.currentquestion);
            }
            current_answer.resize(5);
            break;
        case IDC_RIGHT:
            current_answer.resize(quiz.answers[quiz.currentquestion].size());
            quiz.CheckAnwser(current_answer);
            quiz.currentquestion++;
            SetDlgItemText(hDlg, IDC_STATIC, quiz.questions[quiz.currentquestion].first.c_str());
            ShowWindow(GetDlgItem(hDlg, IDC_LEFT), 1);
            if (quiz.currentquestion == quiz.questions.size() - 1) {
                ShowWindow(GetDlgItem(hDlg, IDC_RIGHT), SW_HIDE);
            }
            for (int i = 0; i < 5; i++) {
                SendDlgItemMessage(hDlg, IDC_RADIO1 + i, BM_SETCHECK, BST_UNCHECKED, 0);
                SendDlgItemMessage(hDlg, IDC_CHECK1 + i, BM_SETCHECK, BST_UNCHECKED, 0);

            }
            if (quiz.questions[quiz.currentquestion].second) {
                CheckQB(hDlg, quiz.answers[quiz.currentquestion], quiz.currentquestion);
            }
            else {
                RadioQB(hDlg, quiz.answers[quiz.currentquestion], quiz.currentquestion);
            }
            current_answer.resize(5);
            break;
        case IDC_SAVE:
            if (quiz.questions[quiz.currentquestion].second) {
                for (int i = 0; i < 5; i++) {
                    current_answer[i] = IsDlgButtonChecked(hDlg, IDC_CHECK1 + i);
                }
                
            }
            else {
                for (int i = 0; i < 5; i++) {
                    current_answer[i] = IsDlgButtonChecked(hDlg, IDC_RADIO1 + i);
                }

            }
            current_answer.resize(quiz.answers[quiz.currentquestion].size());
            quiz.CheckAnwser(current_answer);
            current_answer.resize(5);
            break;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

 