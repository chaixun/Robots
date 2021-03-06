﻿#include <Platform.h>

#include <iostream>
#include <cstring>
#include <iomanip> 
#include <bitset>
#include <cstring>
#include <map>
#include <string>

using namespace std;


#ifdef PLATFORM_IS_WINDOWS
#define rt_printf printf
#endif



#include <Aris_Core.h>
#include <Aris_Message.h>
#include <Aris_IMU.h>
#include <Robot_Server.h>
#include <Robot_Gait.h>
#include <Robot_Type_I.h>

using namespace Aris::Core;

int main()
{
	auto rs = Robots::ROBOT_SERVER::GetInstance();
	rs->CreateRobot<Robots::ROBOT_TYPE_I>();
#ifdef PLATFORM_IS_LINUX
	rs->LoadXml("/usr/Robots/resource/Robot_Type_I/Robot_VIII/Robot_VIII.xml");
#endif
#ifdef PLATFORM_IS_WINDOWS
	rs->LoadXml("C:\\Robots\\resource\\Robot_Type_I\\Robot_III\\Robot_III.xml");
#endif
	rs->AddGait("wk", Robots::walk, Robots::parseWalk);
	rs->AddGait("ad", Robots::adjust, Robots::parseAdjust);
	rs->AddGait("fw", Robots::fastWalk, Robots::parseFastWalk);
	rs->AddGait("ro", Robots::resetOrigin, Robots::parseResetOrigin);
	rs->Start();
	std::cout<<"started"<<std::endl;

	
	
	Aris::Core::RunMsgLoop();

	return 0;
}
