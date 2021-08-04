#include"stress_func.h"

static double output_acc = 1.0e-8;

//print target stress term 
void Stress_Func::print_stress(const string &name, const matrix& f, const bool screen, bool ry)const
{
	GlobalV::ofs_running << " --------------------------- " << name << " ----------------------------" << std::endl;
	
	
	double fac = 1.0;
	
	if(!ry)
	{
	 //	fac = Ry_to_eV / 0.529177;
	}

	std::cout << setprecision(5);
	std::cout << setiosflags(ios::showpos);

	if(screen)
	{
		std::cout << " ------------------- " << name << " --------------------" << std::endl;
	
	}

	for (int i=0;i<3;i++)
	{
		GlobalV::ofs_running << setw(15)<< " ";
		if( abs(f(i,0)) >output_acc) GlobalV::ofs_running << setw(15) << f(i,0) * fac;
		else GlobalV::ofs_running << setw(15) << "0";
		if( abs(f(i,1)) >output_acc) GlobalV::ofs_running << setw(15) << f(i,1) * fac;
		else GlobalV::ofs_running << setw(15) << "0";
		if( abs(f(i,2)) >output_acc) GlobalV::ofs_running << setw(15) << f(i,2) * fac;
		else GlobalV::ofs_running << setw(15) << "0";
		GlobalV::ofs_running << std::endl;

		if(screen)
		{
			std::cout<<fixed;
			if( abs(f(i,0)) >output_acc) std::cout << setw(15) << f(i,0)*fac;
			else std::cout << setw(15) << "0";
			if( abs(f(i,1)) >output_acc) std::cout << setw(15) << f(i,1)*fac;
			else std::cout << setw(15) << "0";
			if( abs(f(i,2)) >output_acc) std::cout << setw(15) << f(i,2)*fac;
			else std::cout << setw(15) << "0";
			std::cout << std::endl;
		}	
	}


	std::cout << resetiosflags(ios::showpos);

    return;
}

//print total stress
void Stress_Func::printstress_total(const matrix& scs, bool ry)
{
// zhengdy update 2016-10-08
	double unit_transform = 1;

	if(!ry)
	{
		unit_transform = RYDBERG_SI / pow(BOHR_RADIUS_SI,3) * 1.0e-8;
	}
//	std::cout.setf(ios::fixed);


	//GlobalV::ofs_running << setiosflags(ios::right);
 	GlobalV::ofs_running << setprecision(6) << setiosflags(ios::showpos) << setiosflags(ios::fixed) << std::endl;
	NEW_PART("TOTAL-STRESS (KBAR)");//Ryd/(a.u.)^3
    std::cout << " ><><><><><><><><><><><><><><><><><><><><><><" << std::endl;
    std::cout << " TOTAL-STRESS (KBAR):" << std::endl;
    std::cout << " ><><><><><><><><><><><><><><><><><><><><><><" << std::endl;

//        if(INPUT.stress_set == 1)
//        int GlobalV::TEST_STRESS = 1;

 	if(GlobalV::TEST_STRESS) 
	{
		std::cout << setiosflags(ios::fixed) << setprecision(6);
		std::cout << setiosflags(ios::showpos);
		std::cout << " ------------------- TOTAL      STRESS --------------------" << std::endl;
    	std::cout << " " << setw(8) << "STRESS" << std::endl;
    	GlobalV::ofs_running << " " << setw(12) << "STRESS" << std::endl;
	}

    
	for (int i=0; i<3; i++)
	{

		//if(GlobalV::TEST_STRESS)
		std::cout << " " << setw(15) << scs(i,0)*unit_transform << setw(15)
			<< scs(i,1)*unit_transform << setw(15) << scs(i,2)*unit_transform << std::endl;

		GlobalV::ofs_running << " " << setw(15) << scs(i,0)*unit_transform << setw(15)
			<< scs(i,1)*unit_transform << setw(15) << scs(i,2)*unit_transform << std::endl;

	}
	GlobalV::ofs_running << setiosflags(ios::left);
	std::cout << resetiosflags(ios::showpos);

    return;
}
