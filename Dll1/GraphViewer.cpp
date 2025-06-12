#include "pch.h"

#include <string>
#include <thread>
#include <iostream> 
#include <chrono> // std::chrono::milliseconds

#define PIPE_NAME L"\\\\.\\pipe\\Integrity_Checker_MEP_PIPE"
#define WINDOW_CLASS_NAME L"GraphViewer"
#define WINDOW_TITLE L"C++ Win32 IPC Window"

// 전역 변수
HWND g_hWnd = NULL;
HANDLE g_hPipe = NULL;
HANDLE g_hPipeServerStopEvent = NULL; // 파이프 서버 스레드 종료 신호 이벤트

// 파이프 서버 로직을 별도 함수로 분리
void RunPipeServer()
{
    // Debugging output
    // AllocConsole();
    // FILE* p_stdout;
    // freopen_s(&p_stdout, "CONOUT$", "w", stdout);
    // std::wcout << L"C++ Win32 Named Pipe Server thread started." << std::endl;
    // OutputDebugStringW(L"C++ Win32 Named Pipe Server thread started.\n");


    g_hPipe = CreateNamedPipe(
        PIPE_NAME,
        PIPE_ACCESS_DUPLEX,          // 양방향 (read/write)
        PIPE_TYPE_BYTE | PIPE_WAIT,  // 바이트 스트림, 블로킹 모드
        PIPE_UNLIMITED_INSTANCES,    // 여러 클라이언트 동시 연결 가능
        1024,                        // 출력 버퍼 크기
        1024,                        // 입력 버퍼 크기
        0,                           // 기본 타임아웃
        NULL);                       // 보안 속성

    if (g_hPipe == INVALID_HANDLE_VALUE)
    {
        // OutputDebugStringW((L"Failed to create named pipe in thread. Error: " + std::to_wstring(GetLastError()) + L"\n").c_str());
        // FreeConsole();
        return;
    }

    // WaitForSingleObject에 사용할 핸들 배열
    HANDLE handlesToWaitOn[] = { g_hPipeServerStopEvent, g_hPipe };

    while (true) // 클라이언트 연결을 계속 대기
    {
        // ConnectNamedPipe 호출 전, 종료 신호 확인
        if (WaitForSingleObject(g_hPipeServerStopEvent, 0) == WAIT_OBJECT_0) // 이벤트가 이미 시그널 상태라면
        {
            // OutputDebugStringW(L"Pipe server: Stop event signaled before ConnectNamedPipe. Exiting.\n");
            break; // 스레드 종료
        }

        // OutputDebugStringW(L"Pipe server: Waiting for client connection...\n");
        // ConnectNamedPipe는 블로킹 함수이므로, 이벤트 핸들을 통한 타임아웃/종료 신호 감지가 어려움.
        // 대신 연결 후 ReadFile/WriteFile 등에서 감지하는 방법 사용.
        // 또는 ConnectNamedPipe를 오버랩 방식으로 사용하여 WaitForMultipleObjects와 조합.
        // 이 예시에서는 간단히 블로킹 모드로 유지하고, 통신 완료 후 또는 시작 전 이벤트 확인.

        if (ConnectNamedPipe(g_hPipe, NULL) != FALSE) // 클라이언트 연결 대기 (블로킹)
        {
            // OutputDebugStringW(L"Pipe server: Client connected!\n");

            // 통신 과정 중에도 종료 신호를 주기적으로 확인하거나,
            // ReadFile/WriteFile을 오버랩 I/O로 사용하여 종료 이벤트를 함께 대기해야 합니다.
            // 이 예제에서는 간략하게 통신 완료 후 다시 종료 신호 확인

            wchar_t buffer[1024];
            DWORD bytesRead;
            // ReadFile 역시 블로킹이므로, 만약 클라이언트가 데이터를 안 보내면 여기서 무한정 대기할 수 있음
            // 실제 환경에서는 오버랩 I/O + WaitForMultipleObjects를 사용해야 안전함
            if (ReadFile(g_hPipe, buffer, sizeof(buffer) - sizeof(wchar_t), &bytesRead, NULL) != FALSE)
            {
                buffer[bytesRead / sizeof(wchar_t)] = L'\0';
                // OutputDebugStringW((L"Pipe server: Received from C#: " + std::wstring(buffer) + L"\n").c_str());

                std::wstring response = L"Hello from C++ Win32 GUI: " + std::wstring(buffer);
                DWORD bytesWritten;
                if (WriteFile(g_hPipe, response.c_str(), (response.length() + 1) * sizeof(wchar_t), &bytesWritten, NULL) == FALSE)
                {
                    // OutputDebugStringW((L"Pipe server: Failed to write to pipe. Error: " + std::to_wstring(GetLastError()) + L"\n").c_str());
                }
                else
                {
                    // OutputDebugStringW((L"Pipe server: Sent to C++: " + response + L"\n").c_str());
                }
            }
            else
            {
                // OutputDebugStringW((L"Pipe server: Failed to read from pipe. Error: " + std::to_wstring(GetLastError()) + L"\n").c_str());
            }

            DisconnectNamedPipe(g_hPipe);
            // OutputDebugStringW(L"Pipe server: Client disconnected.\n");
        }
        else
        {
            // ConnectNamedPipe 실패 시 (예: ERROR_PIPE_CONNECTED는 이미 연결됨)
            DWORD lastError = GetLastError();
            if (lastError != ERROR_PIPE_CONNECTED)
            {
                // OutputDebugStringW((L"Pipe server: ConnectNamedPipe failed. Error: " + std::to_wstring(lastError) + L"\n").c_str());
                break; // 치명적인 에러이므로 스레드 종료
            }
        }
    }
    // 루프 종료 시:
    CloseHandle(g_hPipe); // 파이프 핸들 닫기
    g_hPipe = NULL;
    // OutputDebugStringW(L"Pipe server thread terminated.\n");
    // FreeConsole();
}

// 윈도우 프로시저 (메시지 처리)
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
        // 파이프 서버 스레드는 wWinMain에서 시작
        break;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        std::wstring text = L"C++ Win32 API Window (IPC Ready)";
        TextOutW(hdc, 10, 10, text.c_str(), (int)text.length());
        EndPaint(hWnd, &ps);
    }
    break;

    case WM_CLOSE:
        // OutputDebugStringW(L"WM_CLOSE received. Calling DestroyWindow.\n");
        DestroyWindow(hWnd);
        return 0;

    case WM_DESTROY:
        // OutputDebugStringW(L"WM_DESTROY received. Posting Quit message.\n");
        PostQuitMessage(0);
        return 0;

    default:
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
    return 0;
}

// Win32 애플리케이션의 엔트리 포인트
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    // 종료 이벤트 생성
    g_hPipeServerStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL); // 수동 리셋 이벤트 (Non-signaled 초기 상태)
    if (g_hPipeServerStopEvent == NULL)
    {
        MessageBox(NULL, L"Failed to create stop event!", L"Error", MB_ICONERROR | MB_OK);
        return 1;
    }

    // 1. 윈도우 클래스 등록
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = WINDOW_CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    if (!RegisterClassEx(&wc))
    {
        MessageBox(NULL, L"Window Registration Failed!", L"Error", MB_ICONERROR | MB_OK);
        CloseHandle(g_hPipeServerStopEvent);
        return 1;
    }

    // 2. 윈도우 생성
    g_hWnd = CreateWindowEx(
        0, WINDOW_CLASS_NAME, WINDOW_TITLE,
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 400, 200,
        NULL, NULL, hInstance, NULL
    );

    if (g_hWnd == NULL)
    {
        MessageBox(NULL, L"Window Creation Failed!", L"Error", MB_ICONERROR | MB_OK);
        CloseHandle(g_hPipeServerStopEvent);
        return 1;
    }

    // 3. 윈도우 표시
    ShowWindow(g_hWnd, nCmdShow);
    UpdateWindow(g_hWnd);

    // 4. 파이프 서버 스레드 시작
    std::thread pipeServerThread(RunPipeServer);

    // 5. 메시지 루프
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // 메시지 루프가 종료되면 (WM_QUIT 수신)
    // 파이프 서버 스레드에 종료 신호를 보냄
    SetEvent(g_hPipeServerStopEvent); // 이벤트 시그널 상태로 변경

    // 파이프 서버 스레드가 종료될 때까지 대기 (선택 사항이지만 안전을 위해 권장)
    // 실제 애플리케이션에서는 WaitForSingleObject(pipeServerThread.native_handle(), TIMEOUT) 사용
    // 단, pipeServerThread는 detach되었으므로 native_handle()에 접근할 수 없습니다.
    // 스레드를 join하려면 detach() 대신 pipeServerThread.join()을 호출해야 합니다.
    // 하지만 join()을 하면 wWinMain이 블로킹되므로, detach() 상태에서 종료를 보장하려면
    // 파이프 스레드가 이벤트에 반응하여 자체적으로 종료하는 로직을 반드시 구현해야 합니다.
    // 여기서는 파이프 스레드가 이벤트 감지에 따라 자체 종료하는 것으로 가정.

    if (pipeServerThread.joinable()) { // detach()를 사용했으므로 joinable()은 false일 것
        // 이 코드는 detach()를 사용했기 때문에 사실상 호출되지 않습니다.
        // 스레드가 정상 종료될 때까지 기다리려면 detach() 대신 join()을 사용해야 합니다.
        // 하지만 wWinMain에서 join()을 사용하면 메시지 루프가 종료될 때까지 프로세스가 종료되지 않습니다.
        // 따라서 파이프 스레드는 자체적으로 종료되도록 설계해야 합니다.
        // pipeServerThread.join(); 
    }

    CloseHandle(g_hPipeServerStopEvent); // 이벤트 핸들 닫기
    return (int)msg.wParam;
}