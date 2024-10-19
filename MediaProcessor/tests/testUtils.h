#ifndef TESTUTILS_H
#define TESTUTILS_H

#include <filesystem>
#include <nlohmann/json.hpp>
#include <stdexcept>

namespace fs = std::filesystem;
namespace MediaProcessor::testUtils {

constexpr const char* DEFAULT_TEST_CONFIG_FILE_PATH = "testConfig.json";
constexpr double DEFAULT_TOLERANCE = 0.01;

/**
 * @brief Create a test configuration file.
 */
class TestConfigFile {
   public:
    TestConfigFile() : m_filePath(DEFAULT_TEST_CONFIG_FILE_PATH) {
        writeJsonToFile(m_filePath, jsonContent);
    }
    explicit TestConfigFile(const fs::path& path) : m_filePath(path) {
        writeJsonToFile(path, jsonContent);
    }

    /**
     * @brief Create a test configuration file with custom content.
     *
     * @param path Path of the test configuration file.
     * @param jsonContent Custom JSON content.
     */
    void createConfigFile(const fs::path& path, const nlohmann::json& jsonContent);

    /**
     * @brief Changes the configuration options and writes them to test configuration file.
     *
     * This function accepts a variable number of arguments in the form of
     * key-value pairs e.g., (option1, value1, option2, value2, ...).
     * The keys represent the configuration options to be changed,
     * and the values represent the new settings for those options.
     *
     * example : changeConfigOptions("option1", 1, "option2", true);
     *
     * @tparam Args Variadic template parameter pack representing the types of the arguments.
     * @param args The key-value pairs of configuration options and their corresponding values.
     */
    template <typename... Args>
    void changeConfigOptions(Args&&... args) {
        changeJsonContents(std::forward<Args>(args)...);
        writeJsonToFile(m_filePath, jsonContent);
    }

    ~TestConfigFile() {
        deleteConfigFile();
    }

    /**
     * @brief return the path of the test configuration file the object holds.
     *
     * @return Path of the test configuration file.
     */
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

bool compareFilesByteByByte(const fs::path& file1, const fs::path& file2);
bool compareAudioFiles(const fs::path& file1, const fs::path& file2,
                       double tolerance = DEFAULT_TOLERANCE);
template <typename Iter>
bool compareBuffersWithTolerance(Iter begin1, Iter end1, Iter begin2, Iter end2, double tolerance);

}  // namespace MediaProcessor::testUtils

#endif  // TESTUTILS_H