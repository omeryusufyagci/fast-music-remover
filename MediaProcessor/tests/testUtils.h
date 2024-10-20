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
     * @tparam T Variadic template parameter pack representing the types of the arguments.
     * @param args The key-value pairs of configuration options and their corresponding values.
     */
    template <typename... T>
    void changeConfigOptions(T&&... args) {
        changeJsonContents(std::forward<T>(args)...);
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

    template <typename T>
    void changeJsonContent(const std::string& option, const T& value) {
        if (!jsonContent.contains(option)) {
            throw std::runtime_error(option + " not found");
        }
        jsonContent[option] = value;
    }

    void changeJsonContents() {}
    template <typename TKey, typename TValue, typename... TRest>
    void changeJsonContents(const TKey& option, const TValue& value, TRest&&... rest) {
        changeJsonContent(option, value);
        changeJsonContents(std::forward<TRest>(rest)...);
    }

    fs::path m_filePath;
    fs::path m_rootPath = fs::path(TEST_MEDIA_DIR).parent_path().parent_path();
    nlohmann::json jsonContent = {
        {"deep_filter_path",
         (m_rootPath / "res/deep-filter-0.5.6-x86_64-unknown-linux-musl").string()},
        {"deep_filter_tarball_path", (m_rootPath / "res/DeepFilterNet3_ll_onnx.tar.gz")},
        {"deep_filter_encoder_path",
         (m_rootPath / "res/DeepFilterNet3_ll_onnx/tmp/export/enc.onnx")},
        {"deep_filter_decoder_path",
         (m_rootPath / "res/DeepFilterNet3_ll_onnx/tmp/export/df_dec.onnx")},
        {"ffmpeg_path", "/usr/bin/ffmpeg"},
        {"downloads_path", "downloads"},
        {"uploads_path", "uploads"},
        {"use_thread_cap", false},
        {"max_threads_if_capped", 6}};
};

bool compareFilesByteByByte(const fs::path& file1, const fs::path& file2);
bool compareAudioFiles(const fs::path& file1, const fs::path& file2,
                       double tolerance = DEFAULT_TOLERANCE);
template <typename T>
bool compareBuffersWithTolerance(T begin1, T end1, T begin2, T end2, double tolerance);

}  // namespace MediaProcessor::testUtils

#endif  // TESTUTILS_H