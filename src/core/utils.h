#pragma once

#include <cmath>
#include <cstdint>

// Reference:
//  https://dev.ti.com/gallery/view/mmwave/mmWave_Demo_Visualizer/ver/3.6.0/

inline double dspFftScalComp2(double fftMinSize, double fftSize)
{
    return fftMinSize / fftSize;
}

inline double toRelativeDb(uint16_t raw, double log2linScale,
                           double dspFftScaleCompAllLog)
{
    const double kToDB = 20.0 * std::log10(2);
    return raw * log2linScale * kToDB + dspFftScaleCompAllLog;
}

inline double log2MagnitudeToDb(uint16_t log2Value, int numVirtualAntennas,
                                int numRangeBins)
{
    const double log2linScale = (1.0 / 256.0) *
                                std::pow(2, std::ceil(std::log2(numVirtualAntennas))) /
                                numVirtualAntennas;

    const double dspFftScaleComp1DLog =
        20.0 * std::log10(dspFftScalComp2(32.0, numRangeBins));
    const double dspFftScaleCompAllLog = dspFftScaleComp1DLog;
    return toRelativeDb(log2Value, log2linScale, dspFftScaleCompAllLog);
}

/*
Reference: ti mmwave demo web visualizer

P.dataPath[idx].dopplerResolutionMps = 3e8 / (2 * P.profileCfg[profileCfgIdx].centerFreq_actual *1e9 *
                 (P.profileCfg[profileCfgIdx].idleTime + P.profileCfg[profileCfgIdx].rampEndTime)*1e-6 *
                  P.dataPath[idx].numDopplerBins * P.dataPath[idx].numTxAnt);
*/
inline double dopplerResolutionMps(double startFreqGhz, double idleTimeUs,
                                   double rampEndTimeUs, int numDopplerBins,
                                   int numTxAntennas)
{
    return 3e8 / (2.0 * startFreqGhz * 1e9 * (idleTimeUs + rampEndTimeUs) *
                  1e-6 * numDopplerBins * numTxAntennas);
}
