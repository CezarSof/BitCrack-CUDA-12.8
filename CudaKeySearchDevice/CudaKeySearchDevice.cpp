#include "CudaKeySearchDevice.h"
#include "Logger.h"
#include "util.h"
#include "cudabridge.h"
#include "AddressUtil.h"

void CudaKeySearchDevice::cudaCall(cudaError_t err)
{
    if (err != cudaSuccess) {
        throw KeySearchException(cudaGetErrorString(err));
    }
}

CudaKeySearchDevice::CudaKeySearchDevice(int device, int threads, int pointsPerThread, int blocks)
{
    cuda::CudaDeviceInfo info;
    try {
        info = cuda::getDeviceInfo(device);
        _deviceName = info.name;
    } catch (cuda::CudaException &ex) {
        throw KeySearchException(ex.msg);
    }

    if (threads <= 0 || threads % 32 != 0) {
        throw KeySearchException("The number of threads must be a multiple of 32");
    }

    if (pointsPerThread <= 0) {
        throw KeySearchException("At least 1 point per thread required");
    }

    // Optimize block/thread configuration for RTX 4050
    if (blocks == 0) {
        if (threads % info.mpCount != 0) {
            throw KeySearchException("Threads must be a multiple of " + util::format("%d", info.mpCount));
        }

        _threads = threads / info.mpCount;
        _blocks = info.mpCount;

        while (_threads > 1024) { // Increased max threads per SM for RTX GPUs
            _threads /= 2;
            _blocks *= 2;
        }
    } else {
        _threads = threads;
        _blocks = blocks;
    }

    _iterations = 0;
    _device = device;
    _pointsPerThread = pointsPerThread;
}

void CudaKeySearchDevice::init(const secp256k1::uint256 &start, int compression, const secp256k1::uint256 &stride)
{
    if (start.cmp(secp256k1::N) >= 0) {
        throw KeySearchException("Starting key is out of range");
    }

    _startExponent = start;
    _compression = compression;
    _stride = stride;

    cudaCall(cudaSetDevice(_device));
    cudaCall(cudaSetDeviceFlags(cudaDeviceScheduleBlockingSync));

    // Optimize cache settings for RTX 4050
    cudaCall(cudaDeviceSetCacheConfig(cudaFuncCachePreferShared));
    cudaCall(cudaDeviceSetSharedMemConfig(cudaSharedMemBankSizeEightByte));

    generateStartingPoints();
    cudaCall(allocateChainBuf(_threads * _blocks * _pointsPerThread));

    // Calculate incrementor for stride
    secp256k1::ecpoint g = secp256k1::G();
    secp256k1::ecpoint p = secp256k1::multiplyPoint(secp256k1::uint256((uint64_t)_threads * _blocks * _pointsPerThread) * _stride, g);

    cudaCall(_resultList.init(sizeof(CudaDeviceResult), 16));
    cudaCall(setIncrementorPoint(p.x, p.y));
}

void CudaKeySearchDevice::generateStartingPoints()
{
    uint64_t totalPoints = (uint64_t)_pointsPerThread * _threads * _blocks;
    uint64_t totalMemory = totalPoints * 40;

    Logger::log(LogLevel::Info, "Generating " + util::formatThousands(totalPoints) + " starting points (" + util::format("%.1f MB", (double)totalMemory / (1024 * 1024)) + ")");

    std::vector<secp256k1::uint256> exponents;
    secp256k1::uint256 privKey = _startExponent;
    exponents.push_back(privKey);

    for (uint64_t i = 1; i < totalPoints; i++) {
        privKey = privKey.add(_stride);
        exponents.push_back(privKey);
    }

    cudaCall(_deviceKeys.init(_blocks, _threads, _pointsPerThread, exponents));

    double pct = 10.0;
    for (int i = 1; i <= 256; i++) {
        cudaCall(_deviceKeys.doStep());

        if (((double)i / 256.0) * 100.0 >= pct) {
            Logger::log(LogLevel::Info, util::format("%.1f%%", pct));
            pct += 10.0;
        }
    }

    Logger::log(LogLevel::Info, "Done");
    _deviceKeys.clearPrivateKeys();
}

void CudaKeySearchDevice::doStep()
{
    uint64_t numKeys = (uint64_t)_blocks * _threads * _pointsPerThread;

    try {
        callKeyFinderKernel(_blocks, _threads, _pointsPerThread, _iterations < 2 && _startExponent.cmp(numKeys) <= 0, _compression);
    } catch (cuda::CudaException &ex) {
        throw KeySearchException(ex.msg);
    }

    getResultsInternal();
    _iterations++;
}

void CudaKeySearchDevice::getResultsInternal()
{
    int count = _resultList.size();
    if (count == 0) return;

    std::vector<CudaDeviceResult> results(count);
    _resultList.read(results.data(), count);

    for (const auto &r : results) {
        if (!isTargetInList(r.digest)) continue;

        KeySearchResult minerResult;
        secp256k1::uint256 offset = (secp256k1::uint256((uint64_t)_blocks * _threads * _pointsPerThread * _iterations) +
                                     secp256k1::uint256(getPrivateKeyOffset(r.thread, r.block, r.idx))) *
                                    _stride;
        secp256k1::uint256 privateKey = secp256k1::addModN(_startExponent, offset);

        minerResult.privateKey = privateKey;
        minerResult.compressed = r.compressed;
        memcpy(minerResult.hash, r.digest, 20);
        minerResult.publicKey = secp256k1::ecpoint(secp256k1::uint256(r.x, secp256k1::uint256::BigEndian),
                                                   secp256k1::uint256(r.y, secp256k1::uint256::BigEndian));

        removeTargetFromList(r.digest);
        _results.push_back(minerResult);
    }

    _resultList.clear();
    if (!_results.empty()) {
        cudaCall(_targetLookup.setTargets(_targets));
    }
}

bool CudaKeySearchDevice::verifyKey(const secp256k1::uint256 &privateKey, const secp256k1::ecpoint &publicKey, const unsigned int hash[5], bool compressed)
{
    secp256k1::ecpoint g = secp256k1::G();
    secp256k1::ecpoint p = secp256k1::multiplyPoint(privateKey, g);

    if (!(p == publicKey)) return false;

    unsigned int digest[5];
    if (compressed) {
        Hash::hashPublicKeyCompressed(p.x.exportWords<8>(secp256k1::uint256::BigEndian), p.y.exportWords<8>(secp256k1::uint256::BigEndian), digest);
    } else {
        Hash::hashPublicKey(p.x.exportWords<8>(secp256k1::uint256::BigEndian), p.y.exportWords<8>(secp256k1::uint256::BigEndian), digest);
    }

    return memcmp(digest, hash, 20) == 0;
}

size_t CudaKeySearchDevice::getResults(std::vector<KeySearchResult> &resultsOut)
{
    resultsOut.insert(resultsOut.end(), _results.begin(), _results.end());
    _results.clear();
    return resultsOut.size();
}

secp256k1::uint256 CudaKeySearchDevice::getNextKey()
{
    uint64_t totalPoints = (uint64_t)_pointsPerThread * _threads * _blocks;
    return _startExponent + secp256k1::uint256(totalPoints) * _iterations * _stride;
}
