/*\
 *
 * ESP32-S2 WUD-Ducky
 *
 * Copyright (c) 2021 tobozo
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
\*/
#pragma once

namespace NTP
{
  bool enabled = false;

  struct Server
  {
    const char* name;
    const char* addr;
  };

  // Timezone is using a float because Newfoundland, India, Iran, Afghanistan, Myanmar, Sri Lanka, the Marquesas,
  // as well as parts of Australia use half-hour deviations from standard time, also some nations such as Nepal
  // and some provinces such as the Chatham Islands of New Zealand, use quarter-hour deviations.
  float timezone = 0; // UTC
  uint8_t daysavetime = 1; // UTC + 1
  const char* defaultServer = "pool.ntp.org";
  uint8_t currentServer = 0;

  void setTimezone( float tz );
  void setDst( bool set );
  void setServer( uint8_t id );
  void loadPrefServer();
  void enable();

  const Server Servers[] =
  {
    { "Global",        "pool.ntp.org" },
    { "Africa",        "africa.pool.ntp.org" },
    { "Asia",          "asia.pool.ntp.org" },
    { "Europe",        "europe.pool.ntp.org" },
    { "North America", "north-america.pool.ntp.org" },
    { "Oceania",       "oceania.pool.ntp.org" },
    { "South America", "south-america.pool.ntp.org" },
  };

};

