/********************************************************************
 *             a  A
 *            AM\/MA
 *           (MA:MMD
 *            :: VD
 *           ::  º
 *          ::
 *         ::   **      .A$MMMMND   AMMMD     AMMM6    MMMM  MMMM6
 +       6::Z. TMMM    MMMMMMMMMDA   VMMMD   AMMM6     MMMMMMMMM6
 *      6M:AMMJMMOD     V     MMMA    VMMMD AMMM6      MMMMMMM6
 *      ::  TMMTMC         ___MMMM     VMMMMMMM6       MMMM
 *     MMM  TMMMTTM,     AMMMMMMMM      VMMMMM6        MMMM
 *    :: MM TMMTMMMD    MMMMMMMMMM       MMMMMM        MMMM
 *   ::   MMMTTMMM6    MMMMMMMMMMM      AMMMMMMD       MMMM
 *  :.     MMMMMM6    MMMM    MMMM     AMMMMMMMMD      MMMM
 *         TTMMT      MMMM    MMMM    AMMM6  MMMMD     MMMM
 *        TMMMM8       MMMMMMMMMMM   AMMM6    MMMMD    MMMM
 *       TMMMMMM$       MMMM6 MMMM  AMMM6      MMMMD   MMMM
 *      TMMM MMMM
 *     TMMM  .MMM
 *     TMM   .MMD       ARBITRARY·······XML········RENDERING
 *     TMM    MMA       ====================================
 *     TMN    MM
 *      MN    ZM
 *            MM,
 *
 *
 *      AUTHORS: see AUTHORS file
 *
 *      COPYRIGHT: ©2013 - All Rights Reserved
 *
 *      LICENSE: see LICENSE file
 *
 *      WEB: http://axrproject.org
 *
 *      THIS CODE AND INFORMATION ARE PROVIDED "AS IS"
 *      WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED
 *      OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *      IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR
 *      FITNESS FOR A PARTICULAR PURPOSE.
 *
 ********************************************************************/

#ifndef AXRABSTRACTLOGGER_H
#define AXRABSTRACTLOGGER_H

#include "AXRLoggerEnums.h"

namespace AXR
{
    class AXRAbstractLoggerPrivate;

    class AXR_API AXRAbstractLogger
    {
        Q_DISABLE_COPY(AXRAbstractLogger)

    public:
        virtual ~AXRAbstractLogger();

        AXRLoggerChannels activeChannels() const;
        void setActiveChannels(AXRLoggerChannels channels);

        bool areChannelsActive(AXRLoggerChannels channels);
        void activateChannels(AXRLoggerChannels channels);
        void deactivateChannels(AXRLoggerChannels channels);

        void log(AXRLoggerChannels channels, const AXRString &message);
        void logLine(AXRLoggerChannels channels, const AXRString &message);

    protected:
        AXRAbstractLogger();
        virtual void log(AXRLoggerChannel channel, const AXRString &message, bool newLine) = 0;

    private:
        AXRAbstractLoggerPrivate *d;
    };
}

#endif // AXRABSTRACTLOGGER_H
