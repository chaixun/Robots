﻿#include <Platform.h>
#include <Robot_Server.h>

int main()
{
	auto rs = Robots::ROBOT_SERVER::GetInstance();

	//rs->CreateRobot<ROBOT_TYPE_I>();

#ifdef PLATFORM_IS_LINUX
	rs->LoadXml("/usr/Robots/resource/HexapodIII/HexapodIII.xml");
	rs->AddGait("wk", walk, parse);
	rs->Start();
#endif
#ifdef PLATFORM_IS_WINDOWS
	rs->LoadXml("C:\\Robots\\resource\\HexapodIII\\HexapodIII.xml");
	rs->AddGait("wk", Robots::walk, Robots::parseWalk);
#endif

	
	
	//
	/**/
	std::cout<<"finished"<<std::endl;


	Aris::Core::RunMsgLoop();

	return 0;
}
