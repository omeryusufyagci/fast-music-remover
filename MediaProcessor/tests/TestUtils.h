#ifndef TESTUTILS_H
#define TESTUTILS_H

#include <filesystem>
#include <nlohmann/json.hpp>
#include <stdexcept>

namespace fs = std::filesystem;
namespace MediaProcessor::TestUtils {

constexpr const char* DEFAULT_TEST_CONFIG_FILE_PATH = "testConfig.json";

/**
 * @brief Generate a test configuration file.
 */
class TestConfigFile {
   public:
    TestConfigFile() : m_filePath(DEFAULT_TEST_CONFIG_FILE_PATH) {
        writeJsonToFile(m_filePath, jsonObject);
    }
    explicit TestConfigFile(const fs::path& path) : m_filePath(path) {
        writeJsonToFile(path, jsonObject);
    }

    /**
     * @brief Generate a test configuration file with a custom object.
     */
    void generateConfigFile(const fs::path& path, const nlohmann::json& jsonObject);

    /**
     * @brief Updates configuration options and writes them to the file.
     *
     * Accepts key-value pairs as arguments (e.g., "key", 1, "key2", true).
     *
     * @tparam T Variadic template parameter pack for argument types.
     * @param args Key-value pairs of configuration options and their values.
     */
    template <typename... T>
    void changeConfigOptions(T&&... args) {
        changeJsonObjects(std::forward<T>(args)...);
        writeJsonToFile(m_filePath, jsonObject);
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
    void changeJsonObject(const std::string& option, const T& value) {
        if (!jsonObject.contains(option)) {
            throw std::runtime_error(option + " not found");
        }
        jsonObject[option] = value;
    }

    void changeJsonObjects() {}
    template <typename TKey, typename TValue, typename... TRest>
    void changeJsonObjects(const TKey& option, const TValue& value, TRest&&... rest) {
        changeJsonObject(option, value);
        changeJsonObjects(std::forward<TRest>(rest)...);
    }

    fs::path m_filePath;
    fs::path m_rootPath = fs::path(TEST_MEDIA_DIR).parent_path().parent_path();
    nlohmann::json jsonObject = {
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
        {"max_threads_if_capped", 6},
        {"filter_attenuation_limit", 100.0f}};
};

/**
 * @brief A utility class for comparing files.
 *
 */
class CompareFiles {
   public:
    /**
     * @brief Compare two files byte-by-byte.
     *
     * @param filePath1 The path to the first file.
     * @param filePath2 The path to the second file.
     * @param chunkSize The size of the chunks to read (default: 1024).
     * @return true if the files are identical, false otherwise.
     */
    static bool compareFilesByteByByte(const fs::path& filePath1, const fs::path& filePath2,
                                       size_t chunkSize = DEFAULT_CHUNK_SIZE);
    /**
     * @brief Compare two audio files with a given tolerance.
     *
     * @param filePath1 The path to the first audio file.
     * @param filePath2 The path to the second audio file.
     * @param tolerance The tolerance level for comparing audio samples (default: 0.01).
     * @param chunkSize The size of the chunks to read in terms of frames (default: 1024).
     * @return true if the audio files are similar within the tolerance, false otherwise.
     */
    static bool compareAudioFiles(const fs::path& filePath1, const fs::path& filePath2,
                                  double tolerance = DEFAULT_TOLERANCE,
                                  size_t chunkSize = DEFAULT_CHUNK_SIZE);

   private:
    static constexpr size_t DEFAULT_CHUNK_SIZE = 1024;
    static constexpr double DEFAULT_TOLERANCE = 1.0;

    template <typename T>
    static bool isWithinTolerance(const T& a, const T& b, double tolerance,
                                  double relativeTolerance = 1.0);
    template <typename T>
    static bool compareBuffersWithTolerance(T begin1, T end1, T begin2, T end2, double tolerance,
                                            double relativeTolerance = 1.0);
};

}  // namespace MediaProcessor::TestUtils

#endif  // TESTUTILS_H