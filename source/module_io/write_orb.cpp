#include <iostream>

#include "write_orb.h"

namespace ModuleIO
{

void write_orb(const std::vector<std::vector<Numerical_Orbital_Lm>> &orb, const std::string &file_name)
{
	assert(orb.size()>0);
	assert(orb[0].size()>0);

	std::ofstream ofs(file_name);
	ofs<<"---------------------------------------------------------------------------"<<std::endl;
	ofs<<"Element                     "<<orb[0][0].getLabel()<<std::endl;
	ofs<<"Energy Cutoff(Ry)           "<<100<<std::endl;
	ofs<<"Radius Cutoff(a.u.)         "<<orb[0][0].getRcut()<<std::endl;
	ofs<<"Lmax                        "<<orb.size()-1<<std::endl;
	auto get_il_name = [](const std::size_t il) -> char
	{
		switch(il)
		{
			case 0:		return 'S';
			case 1:		return 'P';
			case 2:		return 'D';
			default:	return 'C'+il;
		}
	};
	for(std::size_t il=0; il<orb.size(); ++il)
		ofs<<"Number of "<<get_il_name(il)<<"orbital-->       "<<orb[il].size()<<std::endl;
	ofs<<"---------------------------------------------------------------------------"<<std::endl;
	ofs<<"SUMMARY  END"<<std::endl<<std::endl;
	ofs<<"Mesh                        "<<orb[0][0].getNr()<<std::endl;
	ofs<<"dr                          "<<orb[0][0].get_rab()[0]<<std::endl;

	for(std::size_t il=0; il<orb.size(); ++il)
		for(std::size_t in=0; in<orb[il].size(); ++in)
		{
			ofs<<"                Type                   L                   N"<<std::endl;
			ofs<<"                   0                   "<<il<<"                   "<<in<<std::endl;
			ofs<<std::setprecision(20);
			for(int ir=0; ir<orb[il][in].getNr(); ++ir)
			{
				ofs<<orb[il][in].getPsi(ir);
				if(ir%4==3)	ofs<<std::endl;
				else		ofs<<"\t";
			}
			ofs<<std::defaultfloat;
			if(orb[il][in].getNr()%4!=3)	ofs<<std::endl;
		}
}

}