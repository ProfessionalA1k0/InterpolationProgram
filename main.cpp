#include <windows.h>
#include <vector>
#include <sstream>
#include <string>
#include <algorithm>
#include <iostream>
#include <cctype>
#include <commdlg.h>
#include "Point.h"
#include "FileOperations.h"
#include "Interpolation.h"

using namespace std;

#define IDC_INPUT_POINTS 101
#define IDC_INPUT_VALUES 102
#define IDC_BUTTON_SUBMIT 103
#define IDC_COMBO_METHOD 104
#define IDC_BUTTON_SHOW_GRAPH 105
#define IDC_BUTTON_SAVE_TO_FILE 106
#define IDC_BUTTON_RESET 107
#define IDC_BUTTON_EXIT 108

HWND hInputPointsEdit, hInputValuesEdit, hButtonSubmit, hComboBox, hListBox;
vector<Point> inputPoints;
vector<double> inputValues;
vector<Point> interpolated_points;
vector<Point> interpolated_graf;
vector<double> interpolated_results;
vector<double> x_values;
bool dataEntered = false;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE: {
            hInputPointsEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL,
                                              20, 20, 400, 100, hwnd, (HMENU)IDC_INPUT_POINTS, NULL, NULL);
            CreateWindow("STATIC", "Initial Points (x, y):", WS_CHILD | WS_VISIBLE, 20, 0, 200, 20, hwnd, NULL, NULL, NULL);
            hInputValuesEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL,
                                              20, 140, 400, 50, hwnd, (HMENU)IDC_INPUT_VALUES, NULL, NULL);
            hListBox = CreateWindowEx(0, "LISTBOX", NULL, WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_AUTOVSCROLL | LBS_NOTIFY,
                                      20, 250, 400, 150, hwnd, NULL, NULL, NULL);
            CreateWindow("STATIC", "Interpolation Results:", WS_CHILD | WS_VISIBLE, 20, 230, 200, 20, hwnd, NULL, NULL, NULL);

            CreateWindow("STATIC", "Rules for Data Entry:", WS_CHILD | WS_VISIBLE, 660, 20, 200, 20, hwnd, NULL, NULL, NULL);
            CreateWindow("STATIC", "1. Initial Points (x, y) should be entered in the format: x1 y1.", WS_CHILD | WS_VISIBLE, 660, 50, 600, 20, hwnd, NULL, NULL, NULL);
            CreateWindow("STATIC", "2. Interpolated Points (x) should be entered as numeric values separated by spaces.", WS_CHILD | WS_VISIBLE, 660, 80, 600, 20, hwnd, NULL, NULL, NULL);
            CreateWindow("STATIC", "3. Entered values must be in range: 0.001 < |x| < 100.", WS_CHILD | WS_VISIBLE, 660, 110, 600, 20, hwnd, NULL, NULL, NULL);
            CreateWindow("STATIC", "4. Enter values with no more than three signs after a coma. ", WS_CHILD | WS_VISIBLE, 660, 140, 600, 20, hwnd, NULL, NULL, NULL);
            CreateWindow("STATIC", "5. Number of Initial Points must be between 2 and 10. ", WS_CHILD | WS_VISIBLE, 660, 170, 600, 20, hwnd, NULL, NULL, NULL);
            CreateWindow("STATIC", "6. Number of Interpolation Points must be between 1 and 10. ", WS_CHILD | WS_VISIBLE, 660, 200, 600, 20, hwnd, NULL, NULL, NULL);
            CreateWindow("STATIC", "7. Do not enter identical points. ", WS_CHILD | WS_VISIBLE, 660, 230, 600, 20, hwnd, NULL, NULL, NULL);

            CreateWindow("STATIC", "Interpolation Points (x):", WS_CHILD | WS_VISIBLE, 20, 120, 200, 20, hwnd, NULL, NULL, NULL);
            hButtonSubmit = CreateWindow("BUTTON", "Submit", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
                                         20, 200, 80, 25, hwnd, (HMENU)IDC_BUTTON_SUBMIT, NULL, NULL);
            hComboBox = CreateWindow("COMBOBOX", NULL, WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
                                     110, 200, 100, 200, hwnd, (HMENU)IDC_COMBO_METHOD, NULL, NULL);
            SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)"Lagrange");
            SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)"Aitken");

            hListBox = CreateWindowEx(0, "LISTBOX", NULL, WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_AUTOVSCROLL | LBS_NOTIFY,
                                      20, 250, 800, 150, hwnd, NULL, NULL, NULL);

            CreateWindow("BUTTON", "Reset", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
                         460, 120, 100, 25, hwnd, (HMENU)IDC_BUTTON_RESET, NULL, NULL);

            CreateWindow("BUTTON", "Exit", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
                         460, 170, 100, 25, hwnd, (HMENU)IDC_BUTTON_EXIT, NULL, NULL);


            CreateWindow("BUTTON", "Save to File", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
                         460, 70, 100, 25, hwnd, (HMENU)IDC_BUTTON_SAVE_TO_FILE, NULL, NULL);

            CreateWindow("BUTTON", "Show Graph", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
                         460, 20, 100, 25, hwnd, (HMENU)IDC_BUTTON_SHOW_GRAPH, NULL, NULL);
            EnableWindow(GetDlgItem(hwnd, IDC_BUTTON_SAVE_TO_FILE), FALSE);
            EnableWindow(GetDlgItem(hwnd, IDC_BUTTON_SHOW_GRAPH), FALSE);
        }
        case WM_COMMAND: {
            if ((LOWORD(wParam) == IDC_BUTTON_SUBMIT && HIWORD(wParam) == BN_CLICKED)) {
                char buffer[1024];
                GetWindowText(hInputPointsEdit, buffer, sizeof(buffer));
                std::string pointsInput(buffer);
                GetWindowText(hInputValuesEdit, buffer, sizeof(buffer));
                std::string valuesInput(buffer);
                std::string inputText(buffer);

                for (char& c : valuesInput) {
                    if (!isdigit(c) && c != '.' && c != ' ' && c != '\b' && c != '-') {
                        MessageBox(hwnd, "Only numeric values are allowed!", "Error", MB_OK | MB_ICONERROR);
                        SendMessage(hInputValuesEdit, EM_SETSEL, valuesInput.length() - 1, valuesInput.length() - 1);
                        SendMessage(hInputValuesEdit, EM_REPLACESEL, FALSE, reinterpret_cast<LPARAM>(""));
                        return 0;
                    }
                }
                inputPoints.clear();
                double x, y;
                std::istringstream pointsStream(pointsInput);
                std::string line;
                while (std::getline(pointsStream, line)) {
                    std::stringstream lineStream(line);
                    char c;
                    if (lineStream >> x >> y && !(lineStream >>  c)) {
                        x = FileOperations::roundToThreeDecimalPlaces(std::stod(std::to_string(x)));
                        y = FileOperations::roundToThreeDecimalPlaces(std::stod(std::to_string(y)));

                        if (!FileOperations::isNumber(std::to_string(x)) || !FileOperations::isNumber(std::to_string(y))) {
                            MessageBox(hwnd, "Please enter valid numeric values for x and y.", "Error", MB_OK | MB_ICONERROR);
                            return 0;
                        }
                        if ((abs(x) < 0.001 && abs(x) > 0) || abs(x) > 100) {
                            MessageBox(hwnd, "Please enter valid number.", "Error", MB_OK | MB_ICONERROR);
                            return 0;
                        }
                        if ((abs(y) < 0.001 && abs(y) > 0) || abs(y) > 100) {
                            MessageBox(hwnd, "Please enter valid number.", "Error", MB_OK | MB_ICONERROR);
                            return 0;
                        }
                        Point newPoint(x, y);
                        if (FileOperations::pointAlreadyExists(inputPoints, newPoint)) {
                            MessageBox(hwnd, "Duplicate points are not allowed!", "Error", MB_OK | MB_ICONERROR);
                            return 0;
                        }
                        inputPoints.push_back(newPoint);
                    } else {
                        MessageBox(hwnd, "Invalid format for points. Please enter x and y separated by space.", "Error", MB_OK | MB_ICONERROR);
                        return 0;
                    }
                }

                inputValues.clear();
                std::stringstream valuesStream(valuesInput);
                double value;
                vector<Point> interpolationPoints;

                while (valuesStream >> value) {
                    value = FileOperations::roundToThreeDecimalPlaces(value);

                    if ((abs(value) < 0.001 && abs(value) > 0) || abs(value) > 100) {
                        MessageBox(hwnd, "Please enter valid number.", "Error", MB_OK | MB_ICONERROR);
                        return 0;
                    }

                    Point newPoint(value, 0);
                    if (FileOperations::pointAlreadyExists(interpolationPoints, newPoint)) {
                        MessageBox(hwnd, "Duplicate interpolation points are not allowed!", "Error", MB_OK | MB_ICONERROR);
                        return 0;
                    }
                    interpolationPoints.push_back(newPoint);
                    inputValues.push_back(value);

                }
                if (inputPoints.size() < 2 || inputPoints.size() > 10) {
                    MessageBox(hwnd, "Number of Initial Points must be between 2 and 10.", "Error", MB_OK | MB_ICONERROR);
                    break;
                }
                if (inputValues.size() < 1 || inputValues.size() > 10) {
                    MessageBox(hwnd, "Number of Interpolated Points be between 1 and 10", "Error", MB_OK | MB_ICONERROR);
                    break;
                }


                int selectedItem = SendMessage(hComboBox, CB_GETCURSEL, 0, 0);
                int interpolationMethod = (selectedItem == 0) ? 1 : 2;
                if (selectedItem == CB_ERR) {
                    MessageBox(hwnd, "Please select an interpolation method!", "Error", MB_OK | MB_ICONERROR);
                    break;
                }

                SendMessage(hListBox, LB_RESETCONTENT, 0, 0);

                interpolated_results.clear();
                interpolated_graf.clear();
                interpolated_points.clear();

                for (int i = 0; i < inputValues.size(); ++i) {
                    double interpolatedValue;
                    bool error;
                    int operationCount;
                    bool resultExceedsLimit = false;

                    for (const auto& point : inputPoints) {
                        interpolationPoints.push_back(Point(point.x, point.y));
                    }

                    if (interpolationMethod == 1) {
                        interpolatedValue = Interpolation::lagrangeInterpolation(inputPoints, inputValues[i], error, operationCount);
                    } else {
                        interpolatedValue = Interpolation::aitkenInterpolation(inputPoints, inputValues[i], error, operationCount);
                    }
                    interpolated_results.push_back(interpolatedValue);

                    if (abs(interpolatedValue) > 100) {
                        resultExceedsLimit = true;
                    }

                    interpolated_points.emplace_back(inputValues[i], interpolatedValue);
                    for (x = -100; x <= 100; x += 0.1) {
                        double interpolated_value;
                        if (interpolationMethod == 1) {
                            interpolated_value = Interpolation::lagrangeInterpolation(inputPoints, x, error, operationCount);
                        } else {
                            interpolated_value = Interpolation::aitkenInterpolation(inputPoints, x, error, operationCount);
                        }
                        interpolated_graf.emplace_back(x, interpolated_value);
                    }
                    if (resultExceedsLimit) {
                        EnableWindow(GetDlgItem(hwnd, IDC_BUTTON_SHOW_GRAPH), FALSE);
                    } else {
                        EnableWindow(GetDlgItem(hwnd, IDC_BUTTON_SHOW_GRAPH), TRUE);
                    }
                    if (error) {
                        MessageBox(hwnd, "Error occurred during interpolation.", "Error", MB_OK | MB_ICONERROR);
                        std::string DivisionbyZero  = "Division by zero detected during interpolation.";
                        SendMessage(hListBox, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(DivisionbyZero.c_str()));
                        break;
                    } else {
                        std::string resultText = "Interpolation result for x = " + std::to_string(inputValues[i]) + ": " + std::to_string(interpolatedValue)
                                                 + " (Operations: " + std::to_string(operationCount) + ")";
                        SendMessage(hListBox, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(resultText.c_str()));
                    }
                    dataEntered = true;
                    EnableWindow(GetDlgItem(hwnd, IDC_BUTTON_SAVE_TO_FILE), TRUE);
                    EnableWindow(GetDlgItem(hwnd, IDC_BUTTON_SHOW_GRAPH), TRUE);
                }
            } else if (LOWORD(wParam) == IDC_BUTTON_RESET && HIWORD(wParam) == BN_CLICKED) {
                SetWindowText(hInputPointsEdit, "");
                SetWindowText(hInputValuesEdit, "");
                SendMessage(hComboBox, CB_SETCURSEL, 0, 0);
                SendMessage(hListBox, LB_RESETCONTENT, 0, 0);
                EnableWindow(GetDlgItem(hwnd, IDC_BUTTON_SAVE_TO_FILE), FALSE);
                EnableWindow(GetDlgItem(hwnd, IDC_BUTTON_SHOW_GRAPH), FALSE);
                dataEntered = false;
                inputPoints.clear();
                inputValues.clear();
                interpolated_results.clear();
                x_values.clear();
                interpolated_points.clear();
                interpolated_graf.clear();
            } else if ((LOWORD(wParam) == IDC_BUTTON_SAVE_TO_FILE && HIWORD(wParam) == BN_CLICKED)) {
                if (!dataEntered) {
                    MessageBox(hwnd, "Please submit data before saving.", "Error", MB_OK | MB_ICONERROR);
                    break;
                }
                char filename[MAX_PATH] = { 0 };
                OPENFILENAME ofn = { sizeof(OPENFILENAME) };
                ofn.hwndOwner = hwnd;
                ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
                ofn.lpstrFile = filename;
                ofn.nMaxFile = MAX_PATH;
                ofn.lpstrDefExt = "txt";
                ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
                if (GetSaveFileName(&ofn)) {
                    string saveFilename = ofn.lpstrFile;
                    FileOperations::saveResultToFile(saveFilename, interpolated_results, inputValues);
                    break;
                }
                break;
            }else if ((LOWORD(wParam) == IDC_BUTTON_SHOW_GRAPH && HIWORD(wParam) == BN_CLICKED)) {
                bool resultExceedsLimit = false;
                for (const auto& result : interpolated_results) {
                    if (abs(result) > 100) {
                        resultExceedsLimit = true;
                        break;
                    }
                }
                if (resultExceedsLimit) {
                    MessageBox(hwnd, "Cannot generate graph. Interpolation result exceeds 100.", "Error", MB_OK | MB_ICONERROR);
                } else {
                    string scriptFilename = "gnuplot_script.plt";
                    FileOperations::generateGnuplotScript(scriptFilename, interpolated_points, interpolated_graf, inputPoints, -100, 100, -100, 100);
                    string gnuplotCommand = "gnuplot -p " + scriptFilename;
                    system(gnuplotCommand.c_str());
                }
            }
            else if ((LOWORD(wParam) == IDC_BUTTON_EXIT && HIWORD(wParam) == BN_CLICKED)) {
                PostQuitMessage(0);
            }
            break;
        }
        case WM_CLOSE: {
            DestroyWindow(hwnd);
            break;
        }
        case WM_DESTROY: {
            PostQuitMessage(0);
            break;
        }
        default: {
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const char* className = "InterpolationApp";
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = className;

    if (!RegisterClassEx(&wc)) {
        MessageBox(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    HWND hwnd = CreateWindowEx(0, className, "Interpolation Application",
                               WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 1400, 500,
                               NULL, NULL, hInstance, NULL);

    if (hwnd == NULL) {
        MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG Msg;
    while (GetMessage(&Msg, NULL, 0, 0) > 0) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    return Msg.wParam;
}