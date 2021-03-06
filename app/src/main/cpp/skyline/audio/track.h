// SPDX-License-Identifier: MPL-2.0
// Copyright © 2020 Skyline Team and Contributors (https://github.com/skyline-emu/)

#pragma once

#include <queue>

#include <kernel/types/KEvent.h>
#include <common.h>
#include "common.h"

namespace skyline::audio {
    /**
     * @brief The AudioTrack class manages the buffers for an audio stream
     */
    class AudioTrack {
      private:
        std::function<void()> releaseCallback; //!< Callback called when a buffer has been played
        std::deque<BufferIdentifier> identifiers; //!< Queue of all appended buffer identifiers

        u8 channelCount; //!< The amount channels present in the track
        u32 sampleRate; //!< The sample rate of the track

      public:
        CircularBuffer<i16, constant::SampleRate * constant::ChannelCount * 10> samples; //!< A vector of all appended audio samples
        Mutex bufferLock; //!< This mutex ensures that appending to buffers doesn't overlap

        AudioOutState playbackState{AudioOutState::Stopped}; //!< The current state of playback
        u64 sampleCounter{}; //!< A counter used for tracking buffer status

        /**
         * @param channelCount The amount channels that will be present in the track
         * @param sampleRate The sample rate to use for the track
         * @param releaseCallback A callback to call when a buffer has been played
         */
        AudioTrack(u8 channelCount, u32 sampleRate, const std::function<void()> &releaseCallback);

        /**
         * @brief Starts audio playback using data from appended buffers
         */
        inline void Start() {
            playbackState = AudioOutState::Started;
        }

        /**
         * @brief Stops audio playback. This waits for audio playback to finish before returning.
         */
        void Stop();

        /**
         * @brief Checks if a buffer has been released
         * @param tag The tag of the buffer to check
         * @return True if the given buffer hasn't been released
         */
        bool ContainsBuffer(u64 tag);

        /**
         * @brief Gets the IDs of all newly released buffers
         * @param max The maximum amount of buffers to return
         * @return A vector containing the identifiers of the buffers
         */
        std::vector<u64> GetReleasedBuffers(u32 max);

        /**
         * @brief Appends audio samples to the output buffer
         * @param tag The tag of the buffer
         * @param address The address of the audio buffer
         * @param size The size of the audio buffer in i16 units
         */
        void AppendBuffer(u64 tag, const i16 *address, u64 size);

        /**
         * @brief Appends audio samples to the output buffer
         * @param tag The tag of the buffer
         * @param sampleData A reference to a vector containing I16 format PCM data
         */
        void AppendBuffer(u64 tag, const std::vector<i16> &sampleData = {}) {
            AppendBuffer(tag, sampleData.data(), sampleData.size());
        }

        /**
         * @brief Checks if any buffers have been released and calls the appropriate callback for them
         */
        void CheckReleasedBuffers();
    };
}
