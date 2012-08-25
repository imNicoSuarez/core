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
 *      AUTHORS: Miro Keller
 *
 *      COPYRIGHT: ©2011 - All Rights Reserved
 *
 *      LICENSE: see License.txt file
 *
 *      WEB: http://axr.vg
 *
 *      THIS CODE AND INFORMATION ARE PROVIDED "AS IS"
 *      WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED
 *      OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *      IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR
 *      FITNESS FOR A PARTICULAR PURPOSE.
 *
 ********************************************************************/

#include "AXRDebugging.h"

unsigned int axr_debug_level = 1;

void std_log_level(std::string message, int debugLevel)
{
    if (debugLevel <= axr_debug_level)
    {
        std::cout << message << std::endl;
    }
}

void std_log(std::string message) { std_log_level(message, 0); }
void std_log1(std::string message) { std_log_level(message, 1); }
void std_log2(std::string message) { std_log_level(message, 2); }
void std_log3(std::string message) { std_log_level(message, 3); }
void std_log4(std::string message) { std_log_level(message, 4); }

unsigned axr_debug_active_channels = 0;

void axr_log(uint32_t channels, std::string message)
{
    if ((axr_debug_active_channels & AXR_DEBUG_CH_ON) && (axr_debug_active_channels & channels))
        std_log(message);
}

void axr_log_inline(uint32_t channels, std::string message)
{
    if ((axr_debug_active_channels & AXR_DEBUG_CH_ON) && (axr_debug_active_channels & channels))
        std::cout << message;
}

void axr_debug_activate_channel(uint32_t channels)
{
    axr_debug_active_channels = (axr_debug_active_channels | channels);
}
