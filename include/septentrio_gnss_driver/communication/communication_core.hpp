// *****************************************************************************
//
// © Copyright 2020, Septentrio NV/SA.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//    1. Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//    2. Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//    3. Neither the name of the copyright holder nor the names of its
//       contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// *****************************************************************************

// *****************************************************************************
//
// Boost Software License - Version 1.0 - August 17th, 2003
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:

// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
// *****************************************************************************

#pragma once

// Boost includes
#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>
// C++ library includes
#include <fstream>
#include <memory>
#include <sstream>
// ROSaic includes
#include <septentrio_gnss_driver/communication/async_manager.hpp>
#include <septentrio_gnss_driver/communication/telegram_handler.hpp>

/**
 * @file communication_core.hpp
 * @date 22/08/20
 * @brief Highest-Level view on communication services
 */

/**
 * @namespace io
 * This namespace is for the communication interface, handling all aspects related to
 * serial and TCP/IP communication..
 */
namespace io {

    //! Possible baudrates for the Rx
    const static uint32_t BAUDRATES[] = {
        1200,    2400,    4800,    9600,    19200,   38400,   57600,
        115200,  230400,  460800,  500000,  576000,  921600,  1000000,
        1152000, 1500000, 2000000, 2500000, 3000000, 3500000, 4000000};

    /**
     * @class CommunicationCore
     * @brief Handles communication with and configuration of the mosaic (and beyond)
     * receiver(s)
     */
    class CommunicationCore
    {
    public:
        /**
         * @brief Constructor of the class CommunicationCore
         * @param[in] node Pointer to node
         */
        CommunicationCore(ROSaicNodeBase* node);
        /**
         * @brief Default destructor of the class CommunicationCore
         */
        ~CommunicationCore();

        void close();

        /**
         * @brief Connects the data stream
         */
        void connect();

        /**
         * @brief Configures Rx: Which SBF/NMEA messages it should output and later
         * correction settings
         * */
        void configureRx();

        /**
         * @brief Hands over NMEA velocity message over to the send() method of
         * manager_
         * @param cmd The command to hand over
         */
        void sendVelocity(const std::string& velNmea);

    private:
        /**
         * @brief Resets Rx settings
         */
        void resetSettings();

        /**
         * @brief Initializes the I/O handling
         * * @return Wether connection was successful
         */
        [[nodiscard]] bool initializeIo();

        /**
         * @brief Reset main connection so it can receive commands
         * @return Main connection descriptor
         */
        std::string resetMainConnection();

        void processTelegrams();

        /**
         * @brief Hands over to the send() method of manager_
         * @param cmd The command to hand over
         */
        void send(const std::string& cmd);

        //! Pointer to Node
        ROSaicNodeBase* node_;
        //! Settings
        const Settings* settings_;
        //! TelegramQueue
        TelegramQueue telegramQueue_;
        //! TelegramHandler
        TelegramHandler telegramHandler_;
        //! Processing thread
        std::thread processingThread_;
        //! Whether connecting was successful
        bool initializedIo_ = false;
        //! Processes I/O stream data
        //! This declaration is deliberately stream-independent (Serial or TCP).
        std::unique_ptr<AsyncManagerBase> manager_;

        std::unique_ptr<AsyncManager<TcpIo>> tcpClient_;
        std::unique_ptr<UdpClient> udpClient_;

        std::unique_ptr<AsyncManager<TcpIo>> tcpVsm_;

        bool nmeaActivated_ = false;

        //! Indicator for threads to run
        std::atomic<bool> running_;

        //! Main communication port
        std::string mainConnectionPort_;
        // Port for receiving data streams
        std::string streamPort_;
    };
} // namespace io
