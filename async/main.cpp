#include <chrono>
#include <future>
#include <iostream>
#include <vector>

struct File {

    File() = default;

    File(const std::string & fname, int numBytes): fname(fname), numBytes(numBytes) {

    }
    ~File() = default;

    std::string fname;
    int numBytes;
};

File loadBigFile(const std::string & fname, int targetBytes) {
    // simulator of load a big file
    int loadedBytes{0};
    while(loadedBytes < targetBytes) {
        std::this_thread::sleep_for(std::chrono::microseconds(500));
        loadedBytes += 200;
    }

    File file(fname, loadedBytes);

    return file;
}

int main() {

    const int NUM_BYTE{20000};
    const size_t numTask{10};
    std::vector<std::future<File>> vecFeatureFile;

    for (size_t i = 0; i < numTask; ++i) {
        const std::string fname{"file-" + std::to_string(i)};
        vecFeatureFile.emplace_back(std::async(std::launch::async, loadBigFile, fname, NUM_BYTE));
    }

   for (size_t i = 0; i < numTask; ++i) {
       std::cout << "check task-" << i << '\n';
       auto file = vecFeatureFile.at(i).get();
       std::cout << file.fname << " is ready" << std::endl;
   }
   
   return 0;
}
