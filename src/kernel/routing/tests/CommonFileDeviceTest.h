/** \file               -*- C++ -*-
********************************************************************
* Test for file device timer
*
* Copyright:   (c) 2012 TU Dresden
* \author  Tobias Schlemmer <keinstein@users.berlios.de>
* \license GPL
*
*    This program is free software; you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation; either version 2 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program; if not, write to the Free Software
*    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
*
********************************************************************
* \addtogroup tests
* \{
********************************************************************/
// availlable groups: GUI, route, kernel, muwx, debug, docview


#include "src/kernel/Defs.h"
#include "src/kernel/routing/CommonFileDevice-inlines.h"
#include "src/kernel/routing/Route-inlines.h"
#include "src/kernel/routing/timing.h"
#include <cstdlib>


class testCommonFileDeviceTimer: public mutabor::CommonFileInputDevice {
public:
	long i;
	long max;
	long min;
	mutint64 lasttime ;
	testCommonFileDeviceTimer():CommonFileInputDevice(),i(0),sw() {
	}
	virtual ~testCommonFileDeviceTimer() {}
	void Play() {
		mutabor::CurrentTime.UseRealtime(true);
		max = 0; min = 100000; i= 0;
		CommonFileInputDevice::Play(wxTHREAD_JOINABLE );
		lasttime = wxGetLocalTimeMillis().GetValue();
		sw.Start();
	}

	void Pause() {
		CommonFileInputDevice::Pause();
	}

	bool Open() {
		sw.Start();
		return CommonFileInputDevice::Open();
	}

	void Stop() {
		CommonFileInputDevice::Stop();
		// check the overall time
		CPPUNIT_ASSERT(sw.Time() <= (i*(i-1))/2+30);
		CPPUNIT_ASSERT(sw.Time() >= (i*(i-1))/2-30);
		sw.Pause();
	}

	mutint64 PrepareNextEvent() {
		mutint64 tl = wxGetLocalTimeMillis().GetValue();
//		tl = tl - (lasttime + (mutint64)(i*(i+1))/2);
		tl = tl - lasttime - (mutint64) i;
		mutint64 deviation = std::abs(tl);
		if (max < deviation)  max = deviation;
		if (min > deviation || min == 0) min = deviation;
		if (tl > 10) {
			std::cerr << "Too slow: (" << i << "^2 + " << i << ") / 2 = " << (i*(i+1))/2 
				  << " Runtime: " << tl << "ms" << std::endl;
			CPPUNIT_ASSERT(tl <= 10);
		}
		if (tl < -10) {
			std::cerr << "Too fast: (" << i << "^2 + " << i << ") / 2 = " << (i*(i+1))/2 
				  << " Runtime: " << tl << "ms" << std::endl;
			CPPUNIT_ASSERT(tl >= -10);
		}

		lasttime = wxGetLocalTimeMillis().GetValue();
		if (++i<100) {
			return (mutint64)(i * 1000);
		}
		return GetNO_DELTA();
	}

	void Save(mutabor::tree_storage&, const mutabor::RouteClass*){}
	void Load(mutabor::tree_storage&, mutabor::RouteClass*){}
protected:
	wxStopWatch sw;
};

class CommonFileDeviceTest : public CPPUNIT_NS::TestFixture {
	CPPUNIT_TEST_SUITE( CommonFileDeviceTest );
	CPPUNIT_TEST( testTimer );
	CPPUNIT_TEST_SUITE_END();
	
public:
	CommonFileDeviceTest()
	{
	}

	virtual ~CommonFileDeviceTest()
	{
	}

	int countTestCases () const
	{ 
		return 1; 
	}
  
	void setUp() 
	{ 
		// change DEBUGA to DEBUG in case you need the debug output
#ifdef DEBUGA
                // debugFlags::flags.timer = true;
		// debugFlags::flags.midifile = true;
#endif
	}

	void tearDown()
		{
		}

#if __WXMSW__
	void usleep(int waitTime) {
		__int64 time1 = 0, time2 = 0, freq = 0;

		QueryPerformanceCounter((LARGE_INTEGER *) &time1);
		QueryPerformanceFrequency((LARGE_INTEGER *)&freq);
		
		do {
			QueryPerformanceCounter((LARGE_INTEGER *) &time2);
		} while((time2-time1) < waitTime);
	}	
#endif
	
	void testTimer()
		{
			testCommonFileDeviceTimer * tim = new testCommonFileDeviceTimer();
			CPPUNIT_ASSERT(tim);
			mutabor::InputDevice prevent_from_deletion(tim);
			tim->Open();
			CPPUNIT_ASSERT(tim->GetMode()==mutabor::DeviceStop);
			tim->Play();
			CPPUNIT_ASSERT(tim->GetMode()==mutabor::DevicePlay);
			usleep(2000);
			CPPUNIT_ASSERT(tim->GetMode()==mutabor::DevicePlay);
			tim->Pause();
			CPPUNIT_ASSERT(tim->GetMode()==mutabor::DevicePause);
			usleep(2000);
			CPPUNIT_ASSERT(tim->GetMode()==mutabor::DevicePause);
			tim->Pause();
			CPPUNIT_ASSERT(tim->GetMode()==mutabor::DevicePlay);
			usleep(2000);
			CPPUNIT_ASSERT(tim->GetMode()==mutabor::DevicePlay);
			tim->Pause();
			CPPUNIT_ASSERT(tim->GetMode()==mutabor::DevicePause);
			usleep(2000);
			CPPUNIT_ASSERT(tim->GetMode()==mutabor::DevicePause);
			tim->Play();
			CPPUNIT_ASSERT(tim->GetMode()==mutabor::DevicePlay);
			wxThread::ExitCode e = tim->WaitForDeviceFinish();
//			std::clog << "Deviation min: " << tim->min << " max: " << tim->max << std::endl;
			CPPUNIT_ASSERT(e == 0); 
		}
	
};
///\}
