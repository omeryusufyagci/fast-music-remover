#ifndef TESTUTILS_H
#define TESTUTILS_H

#include <filesystem>
#include <nlohmann/json.hpp>
#include <stdexcept>

namespace fs = std::filesystem;
namespace MediaProcessor::testUtils {

constexpr const char* DEFAULT_TEST_CONFIG_FILE_PATH = "testConfig.json";

class TestConfigFile {
    /*
    Create a test configuration file.
    */
   public:
    TestConfigFile() : m_filePath(DEFAULT_TEST_CONFIG_FILE_PATH) {
        writeJsonToFile(m_filePath, jsonContent);
    }
    explicit TestConfigFile(const fs::path& path) : m_filePath(path) {
        writeJsonToFile(path, jsonContent);
    }

    void createConfigFile(const fs::path& path, const nlohmann::json& jsonContent);

    template <typename... Args>
    void changeConfigOptions(Args&&... args) {
        changeJsonContents(std::forward<Args>(args)...);
        writeJsonToFile(m_filePath, jsonContent);
    }

    ~TestConfigFile() {
        deleteConfigFile();
    }

    fs::path getFilePath() const {
        return m_filePath;
    }

   private:
    void writeJsonToFile(const fs::path& path, const nlohmann::json& jsonContent) const;
    bool deleteConfigFile() const;

    template <typename ValueType>
    void changeJsonContent(const std::string& option, const ValueType& value) {
        if (!jsonContent.contains(option)) {
            throw std::runtime_error(option + " not found");
        }
        jsonContent[option] = value;
    }

    void changeJsonContents() {}
    template <typename Option, typename Value, typename... Rest>
    void changeJsonContents(const Option& option, const Value& value, Rest&&... rest) {
        changeJsonContent(option, value);
        changeJsonContents(std::forward<Rest>(rest)...);
    }

    fs::path m_filePath;
    fs::path rootPath = fs::path(TEST_MEDIA_DIR).parent_path().parent_path();
    nlohmann::json jsonContent = {
        {"deep_filter_path",
         (rootPath / "res/deep-filter-0.5.6-x86_64-unknown-linux-musl").string()},
        {"deep_filter_tarball_path", (rootPath / "res/DeepFilterNet3_ll_onnx.tar.gz")},
        {"deep_filter_encoder_path", (rootPath / "res/DeepFilterNet3_ll_onnx/tmp/export/enc.onnx")},
        {"deep_filter_decoder_path",
         (rootPath / "res/DeepFilterNet3_ll_onnx/tmp/export/df_dec.onnx")},
        {"ffmpeg_path", "/usr/bin/ffmpeg"},
        {"downloads_path", "downloads"},
        {"uploads_path", "uploads"},
        {"use_thread_cap", false},
        {"max_threads_if_capped", 6}};
};

bool areFilesIdentical(const fs::path& file1, const fs::path& file2);

}  // namespace MediaProcessor::testUtils

#endif  // TESTUTILS_H