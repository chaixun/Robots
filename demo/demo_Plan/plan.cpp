#include <Platform.h>
#include "plan.h"
#include <Aris_DynKer.h>

Robots::ROBOT_TYPE_I robot;
Aris::Plan::FAST_PATH tg;

#define NUM 900
#define ACC_NUM 900
#define DEC_NUM 900
const double stepH = 0.04;
const double stepD = 1.1;
const double constTime = NUM;
const double accTime = ACC_NUM;
const double decTime = DEC_NUM;
const double v = stepD /  constTime * 1000 / 2;
const double a = stepD / 2 / accTime / accTime * 1000 * 1000;
const double eePosIni[6][3]{ 
 { -0.3, -0.85, -0.65 }
,{ -0.45, -0.85, 0 }
,{ -0.3, -0.85, 0.65 }
,{ 0.3, -0.85, -0.65 }
,{ 0.45, -0.85, 0 }
,{ 0.3, -0.85, 0.65 } };

int leg_index = 0;


const double ratio = (v-a*accTime/1000)/(1.0 / (constTime / 1000) * 2 * PI);
void b_const(double s_in, double *b_out)
{
	b_out[0] = eePosIni[leg_index][0];
	b_out[1] = stepH * sin(s_in) + eePosIni[leg_index][1];
	b_out[2] = -stepD / 2 * cos(s_in) + eePosIni[leg_index][2] + stepD / 4;
}
void g_const(double s_in, double *g_out)
{
	g_out[0] = 0;
	g_out[1] = stepH*cos(s_in);
	g_out[2] = stepD / 2 * sin(s_in);
}
void h_const(double s_in, double *h_out)
{
	h_out[0] = 0;
	h_out[1] = -stepH*sin(s_in);
	h_out[2] = stepD / 2 * cos(s_in);
}
void pe_const(double time, double *bodyPe, double *bodyVel = nullptr, double *bodyAcc = nullptr)
{
	double s = time / ( constTime / 1000) * 2 * PI;
	double ds = 1.0 / ( constTime / 1000) * 2 * PI;

	if (bodyPe)
	{
		std::fill_n(bodyPe, 6, 0);
		bodyPe[2] = v*time - ratio * std::sin(s);
	}

	if (bodyVel)
	{
		std::fill_n(bodyVel, 6, 0);
		bodyVel[2] = v - ratio * std::cos(s) * ds;
	}

	if (bodyAcc)
	{
		std::fill_n(bodyAcc, 6, 0);
		bodyAcc[2] = ratio * std::sin(s) * ds * ds;
	}


}
void get_const(Aris::Plan::FAST_PATH::DATA & data)
{
	double bodyPe[6],bodyVel[6],bodyAcc[6];
	pe_const(data.time, bodyPe, bodyVel, bodyAcc);

	double bodyPm[16];
	Aris::DynKer::s_pe2pm(bodyPe, bodyPm);
	robot.Body().SetPm(bodyPm);
	robot.Body().SetVel(bodyVel);
	robot.Body().SetAcc(bodyAcc);

	double pEE[3]{ 0 }, vEE[3]{ 0 };
	
	b_const(data.s, pEE);
	g_const(data.s, data.g);
	h_const(data.s, data.h);
	
	Aris::DynKer::s_daxpy(3, data.ds, data.g, 1, vEE, 1);

	robot.pLegs[leg_index]->SetPee(pEE, "G");
	robot.pLegs[leg_index]->SetVee(vEE, "G");

	robot.pLegs[leg_index]->GetJvi(data.Ji, "G");
	robot.pLegs[leg_index]->GetDifJvi(data.dJi, "G");
	robot.pLegs[leg_index]->GetCvi(data.Cv, "G");
	robot.pLegs[leg_index]->GetCai(data.Ca, "G");
}


void b_acc(double s_in, double *b_out)
{
	b_out[0] = eePosIni[leg_index][0];
	b_out[1] = stepH * sin(s_in) + eePosIni[leg_index][1];
	b_out[2] = -stepD / 4 * cos(s_in) + eePosIni[leg_index][2] + stepD / 4;
}
void g_acc(double s_in, double *g_out)
{
	g_out[0] = 0;
	g_out[1] = stepH*cos(s_in);
	g_out[2] = stepD / 4 * sin(s_in);
}
void h_acc(double s_in, double *h_out)
{
	h_out[0] = 0;
	h_out[1] = -stepH*sin(s_in);
	h_out[2] = stepD / 4 * cos(s_in);
}
void get_acc(Aris::Plan::FAST_PATH::DATA & data)
{
	double bodyPe[6]{ 0,0,0.5*a*data.time*data.time,0,0,0 }, bodyVel[6]{ 0,0,a*data.time,0,0,0 }, bodyAcc[6]{ 0,0,a,0,0,0 };
	double bodyPm[16];
	Aris::DynKer::s_pe2pm(bodyPe, bodyPm);
	robot.Body().SetPm(bodyPm);
	robot.Body().SetVel(bodyVel);
	robot.Body().SetAcc(bodyAcc);

	double pEE[3]{ 0 }, vEE[3]{ 0 };

	b_acc(data.s, pEE);
	g_acc(data.s, data.g);
	h_acc(data.s, data.h);

	Aris::DynKer::s_daxpy(3, data.ds, data.g, 1, vEE, 1);

	robot.pLegs[leg_index]->SetPee(pEE, "G");
	robot.pLegs[leg_index]->SetVee(vEE, "G");

	robot.pLegs[leg_index]->GetJvi(data.Ji, "G");
	robot.pLegs[leg_index]->GetDifJvi(data.dJi, "G");
	robot.pLegs[leg_index]->GetCvi(data.Cv, "G");
	robot.pLegs[leg_index]->GetCai(data.Ca, "G");
}

void b_dec(double s_in, double *b_out)
{
	b_out[0] = eePosIni[leg_index][0];
	b_out[1] = stepH * sin(s_in) + eePosIni[leg_index][1];
	b_out[2] = -stepD / 4 * cos(s_in) + eePosIni[leg_index][2];
}
void g_dec(double s_in, double *g_out)
{
	g_out[0] = 0;
	g_out[1] = stepH*cos(s_in);
	g_out[2] = stepD / 4 * sin(s_in);
}
void h_dec(double s_in, double *h_out)
{
	h_out[0] = 0;
	h_out[1] = -stepH*sin(s_in);
	h_out[2] = stepD / 4 * cos(s_in);
}
void get_dec(Aris::Plan::FAST_PATH::DATA & data)
{
	double bodyPe[6]{ 0,0, 0.5*a*decTime / 1000 * decTime / 1000 - 0.5*a*data.time*data.time,0,0,0 };
	double bodyVel[6]{ 0,0,a*decTime / 1000 - a*data.time,0,0,0 };
	double bodyAcc[6]{ 0,0,-a,0,0,0 };

	double bodyPm[16];
	Aris::DynKer::s_pe2pm(bodyPe, bodyPm);
	robot.Body().SetPm(bodyPm);
	robot.Body().SetVel(bodyVel);
	robot.Body().SetAcc(bodyAcc);

	double pEE[3]{ 0 }, vEE[3]{ 0 };

	b_dec(data.s, pEE);
	g_dec(data.s, data.g);
	h_dec(data.s, data.h);

	Aris::DynKer::s_daxpy(3, data.ds, data.g, 1, vEE, 1);

	robot.pLegs[leg_index]->SetPee(pEE, "G");
	robot.pLegs[leg_index]->SetVee(vEE, "G");

	robot.pLegs[leg_index]->GetJvi(data.Ji, "G");
	robot.pLegs[leg_index]->GetDifJvi(data.dJi, "G");
	robot.pLegs[leg_index]->GetCvi(data.Cv, "G");
	robot.pLegs[leg_index]->GetCai(data.Ca, "G");
}



void plan_prepare()
{
#ifdef PLATFORM_IS_WINDOWS
	robot.LoadXml("C:\\Robots\\resource\\Robot_Type_I\\Robot_VIII\\Robot_VIII.xml");
#endif
#ifdef PLATFORM_IS_LINUX
	robot.LoadXml("/usr/Robots/resource/Robot_Type_I/Robot_III.xml");
#endif
}

void plan_const(const char *fileName)
{
	std::vector<std::array<double, 18> > result;
	result.resize(NUM * 2);
	
	int legId[3]{ 0,2,4 };
	for (int i : legId)
	{
		std::cout << "begin to plan leg " << i << std::endl;

		leg_index = i;
		tg.SetMotorLimit(std::vector<Aris::Plan::FAST_PATH::MOTOR_LIMIT> {3, { 0.9,-0.9,3.0,-3.0 } });
		tg.SetBeginNode({ 0.0, 0.0, 0.0, 0.0, true });
		tg.SetEndNode({  constTime / 1000.0, PI, 0.0, 0.0, true });
		tg.SetFunction(get_const);
		tg.Run();


		for (int j = 0; j < NUM; ++j)
		{
			double pEE[3], pIn[3], pm[16], pe[6]{ 0, 0, v * (j + 1)*0.001, 0, 0, 0 };
			
			pe_const((j + 1)*0.001, pe);

			Aris::DynKer::s_pe2pm(pe, pm);
			b_const(tg.Result().at(j), pEE);
			robot.Body().SetPm(pm);
			robot.pLegs[leg_index]->SetPee(pEE);
			robot.pLegs[leg_index]->GetPin(&result[j][i * 3]);
			robot.pLegs[leg_index]->GetPin(&result[j + NUM][(i * 3 + 9) % 18]);
			std::swap(result[j + NUM][(i * 3 + 9) % 18 + 1], result[j + NUM][(i * 3 + 9) % 18 + 2]);

			std::copy_n(eePosIni[(i + 3) % 6], 3, pEE);
			pEE[2] += stepD / 4;
			robot.pLegs[(leg_index + 3) % 6]->SetPee(pEE);
			robot.pLegs[(leg_index + 3) % 6]->GetPin(&result[j][(i * 3 + 9) % 18]);
			robot.pLegs[(leg_index + 3) % 6]->GetPin(&result[j + NUM][i * 3]);
			
			std::swap(result[j + NUM][i * 3 + 1], result[j + NUM][i * 3 + 2]);
		}
	}
	
	Aris::DynKer::dlmwrite(fileName, result);
	std::cout << "finished planing const period" << std::endl;
}
void plan_acc(const char *fileName)
{
	std::vector<std::array<double, 18> > result;
	result.resize(ACC_NUM);

	int legId[3]{ 1,3,5 };
	for (int i : legId)
	{
		std::cout << "begin to plan leg " << i << std::endl;

		leg_index = i;
		tg.SetMotorLimit(std::vector<Aris::Plan::FAST_PATH::MOTOR_LIMIT> {3, { 0.9,-0.9,3.2,-3.2 } });
		tg.SetBeginNode({ 0.0, 0.0, 0.0, 0.0, true });
		tg.SetEndNode({ accTime / 1000.0, PI, 0.0, 0.0, true });
		tg.SetFunction(get_acc);
		tg.Run();


		double pEE[3], pIn[3], pm[16], pe[6]{ 0, 0, 0, 0, 0, 0 };
		for (int j = 0; j < ACC_NUM; ++j)
		{
			pe[2] = 0.5*a * (j + 1)*(j + 1)*1e-6;
			
			Aris::DynKer::s_pe2pm(pe, pm);
			b_acc(tg.Result().at(j), pEE);
			robot.Body().SetPm(pm);
			robot.pLegs[leg_index]->SetPee(pEE);
			robot.pLegs[leg_index]->GetPin(&result[j][i * 3]);

			std::copy_n(eePosIni[(i + 3) % 6], 3, pEE);
			robot.pLegs[(leg_index + 3) % 6]->SetPee(pEE);
			robot.pLegs[(leg_index + 3) % 6]->GetPin(&result[j][(i * 3 + 9) % 18]);
		}

		Aris::DynKer::dsp(pEE, 1, 3);
		Aris::DynKer::dsp(pe, 1, 6);
	}

	Aris::DynKer::dlmwrite(fileName, result);
	std::cout << "finished planing acc period" << std::endl;
}
void plan_dec(const char *fileName)
{
	std::vector<std::array<double, 18> > result;
	result.resize(DEC_NUM);

	int legId[3]{ 0,2,4 };
	for (int i : legId)
	{
		std::cout << "begin to plan leg " << i << std::endl;

		leg_index = i;
		tg.SetMotorLimit(std::vector<Aris::Plan::FAST_PATH::MOTOR_LIMIT> {3, { 0.9,-0.9,3.2,-3.2 } });
		tg.SetBeginNode({ 0.0, 0.0, 0.0, 0.0, true });
		tg.SetEndNode({  decTime / 1000.0, PI, 0.0, 0.0, true });
		tg.SetFunction(get_dec);
		tg.Run();


		double pEE[3], pIn[3], pm[16], pe[6]{ 0, 0, 0, 0, 0, 0 };
		for (int j = 0; j < DEC_NUM; ++j)
		{
			pe[2] = a*decTime/1000*(j + 1)*1e-3 - 0.5*a * (j + 1)*(j + 1)*1e-6;

			Aris::DynKer::s_pe2pm(pe, pm);
			b_dec(tg.Result().at(j), pEE);
			robot.Body().SetPm(pm);
			robot.pLegs[leg_index]->SetPee(pEE);
			robot.pLegs[leg_index]->GetPin(&result[j][i * 3]);

			std::copy_n(eePosIni[(i + 3) % 6], 3, pEE);
			pEE[2] += stepD / 4;
			robot.pLegs[(leg_index + 3) % 6]->SetPee(pEE);
			robot.pLegs[(leg_index + 3) % 6]->GetPin(&result[j][(i * 3 + 9) % 18]);
		}

		Aris::DynKer::dsp(pEE, 1, 3);
		Aris::DynKer::dsp(pe, 1, 6);
	}

	Aris::DynKer::dlmwrite(fileName, result);
	std::cout << "finished planing dec period" << std::endl;
}


