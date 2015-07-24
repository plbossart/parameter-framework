/*
 * Copyright (c) 2015, Intel Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#pragma once

#include "Config.hpp"
#include "ConfigFiles.hpp"

#include <ParameterMgrFullConnector.h>

namespace parameterFramework
{

/** Wrapper around the Parameter Framework to throw exceptions on errors and
 *  have more user friendly methods.
 */
class ParameterFramework : private parameterFramework::ConfigFiles,
                           private CParameterMgrFullConnector
{
private:
    using PF = CParameterMgrFullConnector;

public:
    ParameterFramework(const Config &config = Config()) :
        ConfigFiles(config),
        PF(getPath()) {}

    void start() {
        setForceNoRemoteInterface(true);
        mayFailCall(&PF::start);
    }

    /** @name Forwarded methods
     * Forward those methods without modification as there are ergonomic and
     * can not fail (no failure to throw).
     * @{ */
    using PF::applyConfigurations;
    using PF::getFailureOnMissingSubsystem;
    using PF::setFailureOnMissingSubsystem;
    using PF::getFailureOnFailedSettingsLoad;
    using PF::setFailureOnFailedSettingsLoad;
    using PF::getForceNoRemoteInterface;
    using PF::setForceNoRemoteInterface;
    using PF::getSchemaFolderLocation;
    using PF::setSchemaFolderLocation;
    using PF::getValidateSchemasOnStart;
    using PF::setValidateSchemasOnStart;
    using PF::isValueSpaceRaw;
    using PF::isOutputRawFormatHex;
    using PF::setOutputRawFormat;
    using PF::isTuningModeOn;
    using PF::isAutoSyncOn;
    using PF::setLogger;
    /** @} */

    /** Renaming for better readability (and coherency with PF::isValueSpaceRaw)
     *  of PF::setValueSpace. */
    void setRawValueSpace(bool enable) { setValueSpace(enable); }

    /** Wrap PF::setTuningMode to throw an exception on failure. */
    void setTuningMode(bool enable) { mayFailCall(&PF::setTuningMode, enable); }

    /** Wrap PF::setAutoSync to throw an exception on failure. */
    void setAutoSync(bool enable) { mayFailCall(&PF::setAutoSync, enable); }

private:
    /** Wrap a method that may fail to throw an Exception instead of retuning a boolean.
     * @param[in] method that return a boolean to indicate failure.
     * @param[in] args parameters to call method call with. */
    template <class... MArgs, class... Args>
    void mayFailCall(bool (PF::*method)(MArgs...), Args&&... args) {
        std::string error;
        if (not (this->*method)(std::forward<Args>(args)..., error)) {
            throw Exception(std::move(error));
        }
    }
};

} // parameterFramework
