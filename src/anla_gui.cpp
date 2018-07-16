#include <boost/regex.hpp>
#include <winsock2.h>
#include <windows.h>
#include <stddef.h>

#include "resource.h"
#include "database.h"
#include "common.h"
#include "sxd_client.h"
#include "sxd_web.h"

const char g_szClassName[] = "myWindowClass";

const char version[] = "R180";
std::unique_ptr<sxd_client> sxd_client_town;

// Step 4: the Window Procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE: {
        HWND hEdit = CreateWindow ("Edit", "", WS_CHILDWINDOW | WS_VISIBLE | WS_EX_CLIENTEDGE | WS_VSCROLL | WS_TABSTOP | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL, 0, 0, 0, 0, hwnd, (HMENU) IDC_MAIN_EDIT, ((LPCREATESTRUCT) lParam)->hInstance, NULL);
        SendMessage(hEdit, WM_SETFONT, (WPARAM) GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(FALSE, 0));
        sxd_client_town = std::unique_ptr<sxd_client>(new sxd_client(version, (int) hEdit));
        break;
    }
    case WM_SIZE:
        MoveWindow(GetDlgItem(hwnd, IDC_MAIN_EDIT), 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_USER_EXIT:
            PostMessage(hwnd, WM_CLOSE, 0, 0);
            break;
        case ID_USER_LOGIN: {
            const char* filename = ".\\sxdweb\\sxdweb.tmp";
            std::remove(filename);
            if (common::read_file(filename).size()) {
                common::log("µÇÂ¼Ê§°Ü£¡");
                //MessageBox(NULL, "Remove File Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
                return 0;
            }

            // initiation
            STARTUPINFO si;
            PROCESS_INFORMATION pi;
            ZeroMemory( &si, sizeof(si) );
            si.cb = sizeof(si);
            ZeroMemory( &pi, sizeof(pi) );

            // Start the child process.
            char sxdweb[] = ".\\sxdweb\\sxdweb.exe ¶ÈÈÕÈçÄê V587";
            if (!CreateProcessA( NULL, sxdweb, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
                common::log("CreateProcess Failed!");
                //MessageBox(NULL, "CreateProcess Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
                return 0;
            }

            // Wait until child process exits.
            WaitForSingleObject(pi.hProcess, INFINITE);

            // Close process and thread handles.
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);

            // Open login file
            std::string user_ini = common::read_file(filename);
            if (!user_ini.size()) {
                common::log("Read File Failed!");
                //MessageBox(NULL, "", "Error!", MB_ICONEXCLAMATION | MB_OK);
                return 0;
            }
            //MessageBox(NULL, user_ini.c_str(), "Login", MB_ICONEXCLAMATION | MB_OK);

            boost::smatch match;
            if (!regex_search(user_ini, match, boost::regex("url=(?<url>.*?)\r\ncode=(?<code>.*?)\r\ntime=(?<time>.*?)\r\nhash=(?<hash>.*?)\r\ntime1=(?<time1>.*?)\r\nhash1=(?<hash1>.*?)\r\n"))) {
                common::log("Íæ¼ÒÊý¾ÝÆ¥Åä³ö´í");
                //MessageBox(NULL, "", "Error!", MB_ICONEXCLAMATION | MB_OK);
                return 0;
            }
            std::string url(match["url"]);
            std::string code(match["code"]);
            std::string time(match["time"]);
            std::string hash(match["hash"]);
            std::string time1(match["time1"]);
            std::string hash1(match["hash1"]);

            std::ostringstream oss;
            oss << "Cookie: user=" << code << ";";
            oss << "_time=" << time << ";_hash=" << hash << ";";
            oss << "login_time_sxd_xxxxxxxx=" << time1 << ";login_hash_sxd_xxxxxxxx=" << hash1 << "\r\n";
            std::string cookie = oss.str();

            // get web page from url and cookie
            std::string web_page;
            {
                if (!boost::regex_search(url, match, boost::regex("http://(.*?)(:\\d*)?/(.*)")))
                    throw std::runtime_error("[url] Æ¥Åä³ö´í");
                std::string host(match[1]);
                std::string port(match[2]);
                port = port.size() ? port.substr(1) : "80";
                sxd_web web;
                web.connect(host, port);

                web_page = web.get(url, cookie);

            }
            //MessageBox(NULL, web_page.c_str(), "Login", MB_ICONEXCLAMATION | MB_OK);

            // login town
            if (sxd_client_town->login_town(web_page)) {
                common::log("µÇÂ¼Ê§°Ü£¡");
                //MessageBox(NULL, "µÇÂ¼Ê§°Ü!", "Error!", MB_ICONEXCLAMATION | MB_OK);
                return 0;
            }

            break;
        }
        }
        break;
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASSEX wc;
    HWND hwnd;
    MSG Msg;

    //Step 1: Registering the Window Class
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    //wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_MYICON));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
    //wc.lpszMenuName = NULL;
    wc.lpszMenuName = MAKEINTRESOURCE(IDR_MYMENU);
    wc.lpszClassName = g_szClassName;
    //wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    wc.hIconSm = (HICON) LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_MYICON), IMAGE_ICON, 16, 16, 0);

    if (!RegisterClassEx(&wc)) {
        MessageBox(NULL, "Window Registration Failed!", "Error!",
        MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Step 2: Creating the Window
    hwnd = CreateWindowEx(
    WS_EX_CLIENTEDGE, g_szClassName, "°²À²",
    WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
    NULL, NULL, hInstance, NULL);

    if (hwnd == NULL) {
        MessageBox(NULL, "Window Creation Failed!", "Error!",
        MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Step 3: The Message Loop
    while (GetMessage(&Msg, NULL, 0, 0) > 0) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    return Msg.wParam;
}
