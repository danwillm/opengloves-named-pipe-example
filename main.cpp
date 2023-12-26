#include <iostream>

#include <array>
#include <thread>

#include <windows.h>

template <typename T>
class NamedPipe {
public:
    NamedPipe(const std::string& pipeName) {
        while (1) {
            pipeHandle_ = CreateFileA(pipeName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

            if (pipeHandle_ != INVALID_HANDLE_VALUE) break;

            if (GetLastError() != ERROR_PIPE_BUSY) {
                std::cout << "bad error" << std::endl;
            }

            if (!WaitNamedPipeA(pipeName.c_str(), 1000)) {
                std::cout << "timed out waiting for pipe" << std::endl;
            }

            std::cout << "Failed to setup named pipe.. Waiting 1 second..." << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }

        std::cout << "Named pipe created successfully." << std::endl;
    }

    bool Write(const T& data) {
        DWORD dwWritten;

        return WriteFile(pipeHandle_, (LPCVOID)&data, sizeof(data), &dwWritten, NULL);
    }

    ~NamedPipe() {
        CloseHandle(pipeHandle_);
    }

    HANDLE pipeHandle_;
};

struct v2PipeData {
    std::array<std::array<float, 4>, 5> flexion;
    std::array<float, 5> splay;
    float joy_x;
    float joy_y;
    bool joy_button;
    bool trigger_button;
    bool a_button;
    bool b_button;
    bool grab;
    bool pinch;
    bool menu;
    bool calibrate;

    // new
    float trigger_value;
};

int main() {
   std::string leftPipeName = R"(\\.\pipe\vrapplication\input\glove\v2\left)";
    std::string rightPipeName = R"(\\.\pipe\vrapplication\input\glove\v2\right)";
    
   // NamedPipe<v2PipeData> leftPipe(leftPipeName);
   NamedPipe<v2PipeData> rightPipe(rightPipeName);

    v2PipeData currentData{};

    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    std::cout << "attempting to send data" << std::endl;

    while (true) {
        for (int i = 0; i < currentData.flexion.size(); i++) {
            for (int k = 0; k < currentData.flexion[i].size(); k++) {
                currentData.flexion[i][k] = std::fmod(currentData.flexion[i][k] + 0.01f, 1.f);
            }
        }

    //    leftPipe.Write(currentData);
     rightPipe.Write(currentData);
        
        std::cout << "sent data" << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}