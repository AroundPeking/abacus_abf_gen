#include "unk_overlap_pw.h"

unkOverlap_pw::unkOverlap_pw()
{
	//GlobalV::ofs_running << "this is unkOverlap_pw()" << std::endl;
}

unkOverlap_pw::~unkOverlap_pw()
{
	//GlobalV::ofs_running << "this is ~unkOverlap_pw()" << std::endl;
}



complex<double> unkOverlap_pw::unkdotp_G(const int ik_L, const int ik_R, const int iband_L, const int iband_R, const ComplexMatrix *evc)
{
	
	complex<double> result(0.0,0.0);
	//波函数的平面波基组总数
	const int number_pw = GlobalC::pw.ngmw;
	complex<double> *unk_L = new complex<double>[number_pw];
	complex<double> *unk_R = new complex<double>[number_pw];
	ZEROS(unk_L,number_pw);
	ZEROS(unk_R,number_pw);
	

	for (int ig = 0; ig < GlobalC::kv.ngk[ik_L]; ig++)
	{
		unk_L[GlobalC::wf.igk(ik_L,ig)] = evc[ik_L](iband_L, ig);
	}
	
	for (int ig = 0; ig < GlobalC::kv.ngk[ik_R]; ig++)
	{
		unk_R[GlobalC::wf.igk(ik_R,ig)] = evc[ik_R](iband_R, ig);
	}

	
	for (int iG = 0; iG < number_pw; iG++)
	{

		result = result + conj(unk_L[iG]) * unk_R[iG];

	}


#ifdef __MPI
    // note: the mpi uses MPI_COMMON_WORLD,so you must make the GlobalV::NPOOL = 1.
	double in_date_real = result.real();
	double in_date_imag = result.imag();
	double out_date_real = 0.0;
	double out_date_imag = 0.0;
	MPI_Allreduce(&in_date_real , &out_date_real , 1, MPI_DOUBLE , MPI_SUM , POOL_WORLD);
	MPI_Allreduce(&in_date_imag , &out_date_imag , 1, MPI_DOUBLE , MPI_SUM , POOL_WORLD);
	result = complex<double>(out_date_real,out_date_imag);
#endif

	delete[] unk_L;
	delete[] unk_R;
	return result;


}



complex<double> unkOverlap_pw::unkdotp_G0(const int ik_L, const int ik_R, const int iband_L, const int iband_R, const ComplexMatrix *evc, const Vector3<double> G)
{
	// (1) set value
	complex<double> result(0.0,0.0);
	complex<double> *phase = GlobalC::UFFT.porter;
	complex<double> *psi_r = new complex<double>[GlobalC::pw.nrxx]; // 实空间的波函数

	ZEROS( phase, GlobalC::pw.nrxx);
	ZEROS( psi_r, GlobalC::pw.nrxx );


	for (int ig = 0; ig < GlobalC::kv.ngk[ik_L]; ig++)
	{
		psi_r[ GlobalC::pw.ig2fftw[ GlobalC::wf.igk(ik_L,ig) ] ] = evc[ik_L](iband_L, ig);

	}

	
	// get the phase value in realspace
	for (int ig = 0; ig < GlobalC::pw.ngmw; ig++)
	{
		if (GlobalC::pw.gdirect[ig] == G)
		{
			phase[ GlobalC::pw.ig2fftw[ig] ] = complex<double>(1.0,0.0);
			break;
		}
	}
	
	// (2) fft and get value
    GlobalC::pw.FFT_wfc.FFT3D(psi_r, 1);
	GlobalC::pw.FFT_wfc.FFT3D(phase, 1);
	
	for (int ir = 0; ir < GlobalC::pw.nrxx; ir++)
	{
		psi_r[ir] = psi_r[ir] * phase[ir];
	}
	
	
	// (3) calculate the overlap in ik_L and ik_R
	GlobalC::pw.FFT_wfc.FFT3D( psi_r, -1);
	

	for(int ig = 0; ig < GlobalC::kv.ngk[ik_R]; ig++)
	{
		result = result + conj( psi_r[ GlobalC::pw.ig2fftw[GlobalC::wf.igk(ik_R,ig)] ] ) * evc[ik_R](iband_R, ig);
	}

#ifdef __MPI
    // note: the mpi uses MPI_COMMON_WORLD,so you must make the GlobalV::NPOOL = 1.
	double in_date_real = result.real();
	double in_date_imag = result.imag();
	double out_date_real = 0.0;
	double out_date_imag = 0.0;
	MPI_Allreduce(&in_date_real , &out_date_real , 1, MPI_DOUBLE , MPI_SUM , POOL_WORLD);
	MPI_Allreduce(&in_date_imag , &out_date_imag , 1, MPI_DOUBLE , MPI_SUM , POOL_WORLD);
	result = complex<double>(out_date_real,out_date_imag);
#endif
	
	delete[] psi_r;
    return result;
}

// if noncollinear = 1 or GlobalV::NSPIN = 4 , you need this routine to calculate overlap unk
complex<double> unkOverlap_pw::unkdotp_soc_G(const int ik_L, const int ik_R, const int iband_L, const int iband_R, const ComplexMatrix *evc)
{
	
	complex<double> result(0.0,0.0);
	//波函数的平面波基组总数
	const int number_pw = GlobalC::pw.ngmw;
	complex<double> *unk_L = new complex<double>[number_pw*GlobalV::NPOL];
	complex<double> *unk_R = new complex<double>[number_pw*GlobalV::NPOL];
	ZEROS(unk_L,number_pw*GlobalV::NPOL);
	ZEROS(unk_R,number_pw*GlobalV::NPOL);
	
	for(int i = 0; i < GlobalV::NPOL; i++)
	{
		for (int ig = 0; ig < GlobalC::kv.ngk[ik_L]; ig++)
		{
			unk_L[GlobalC::wf.igk(ik_L,ig)+i*number_pw] = evc[ik_L](iband_L, ig+i*GlobalC::wf.npwx);
		}
	
		for (int ig = 0; ig < GlobalC::kv.ngk[ik_R]; ig++)
		{
			unk_R[GlobalC::wf.igk(ik_R,ig)+i*number_pw] = evc[ik_R](iband_R, ig+i*GlobalC::wf.npwx);
		}

	}
	
	for (int iG = 0; iG < number_pw*GlobalV::NPOL; iG++)
	{

		result = result + conj(unk_L[iG]) * unk_R[iG];

	}
	
#ifdef __MPI
    // note: the mpi uses MPI_COMMON_WORLD,so you must make the GlobalV::NPOOL = 1.
	double in_date_real = result.real();
	double in_date_imag = result.imag();
	double out_date_real = 0.0;
	double out_date_imag = 0.0;
	MPI_Allreduce(&in_date_real , &out_date_real , 1, MPI_DOUBLE , MPI_SUM , POOL_WORLD);
	MPI_Allreduce(&in_date_imag , &out_date_imag , 1, MPI_DOUBLE , MPI_SUM , POOL_WORLD);
	result = complex<double>(out_date_real,out_date_imag);
#endif

	delete[] unk_L;
	delete[] unk_R;
	return result;


}


//这里G矢量是direct坐标
complex<double> unkOverlap_pw::unkdotp_soc_G0(const int ik_L, const int ik_R, const int iband_L, const int iband_R, const ComplexMatrix *evc, const Vector3<double> G)
{
	// (1) set value
	complex<double> result(0.0,0.0);
	complex<double> *phase = GlobalC::UFFT.porter;
	complex<double> *psi_up = new complex<double>[GlobalC::pw.nrxx];
	complex<double> *psi_down = new complex<double>[GlobalC::pw.nrxx];
	ZEROS( phase, GlobalC::pw.nrxx);
	ZEROS( psi_up, GlobalC::pw.nrxx );
	ZEROS( psi_down, GlobalC::pw.nrxx );

	for(int i = 0; i < GlobalV::NPOL; i++)
	{
		for (int ig = 0; ig < GlobalC::kv.ngk[ik_L]; ig++)
		{
			if( i == 0 ) psi_up[ GlobalC::pw.ig2fftw[ GlobalC::wf.igk(ik_L,ig) ] ] = evc[ik_L](iband_L, ig);
			if( i == 1 ) psi_down[ GlobalC::pw.ig2fftw[ GlobalC::wf.igk(ik_L,ig) ] ] = evc[ik_L](iband_L, ig+i*GlobalC::wf.npwx);
		}
	}
	
	// get the phase value in realspace
	for (int ig = 0; ig < GlobalC::pw.ngmw; ig++)
	{
		if (GlobalC::pw.gdirect[ig] == G)
		{
			phase[ GlobalC::pw.ig2fftw[ig] ] = complex<double>(1.0,0.0);
			break;
		}
	}
	
	// (2) fft and get value
    GlobalC::pw.FFT_wfc.FFT3D(psi_up, 1);
    GlobalC::pw.FFT_wfc.FFT3D(psi_down, 1);	
	GlobalC::pw.FFT_wfc.FFT3D(phase, 1);
	
	for (int ir = 0; ir < GlobalC::pw.nrxx; ir++)
	{
		psi_up[ir] = psi_up[ir] * phase[ir];
		psi_down[ir] = psi_down[ir] * phase[ir];
	}
	
	
	// (3) calculate the overlap in ik_L and ik_R
	GlobalC::pw.FFT_wfc.FFT3D( psi_up, -1);
	GlobalC::pw.FFT_wfc.FFT3D( psi_down, -1);
	
	for(int i = 0; i < GlobalV::NPOL; i++)
	{
		for(int ig = 0; ig < GlobalC::kv.ngk[ik_R]; ig++)
		{
			if( i == 0 ) result = result + conj( psi_up[ GlobalC::pw.ig2fftw[GlobalC::wf.igk(ik_R,ig)] ] ) * evc[ik_R](iband_R, ig);
			if( i == 1 ) result = result + conj( psi_down[ GlobalC::pw.ig2fftw[GlobalC::wf.igk(ik_R,ig)] ] ) * evc[ik_R](iband_R, ig+i*GlobalC::wf.npwx);
		}
	}
	
#ifdef __MPI
    // note: the mpi uses MPI_COMMON_WORLD,so you must make the GlobalV::NPOOL = 1.
	double in_date_real = result.real();
	double in_date_imag = result.imag();
	double out_date_real = 0.0;
	double out_date_imag = 0.0;
	MPI_Allreduce(&in_date_real , &out_date_real , 1, MPI_DOUBLE , MPI_SUM , POOL_WORLD);
	MPI_Allreduce(&in_date_imag , &out_date_imag , 1, MPI_DOUBLE , MPI_SUM , POOL_WORLD);
	result = complex<double>(out_date_real,out_date_imag);
#endif
	
	delete[] psi_up;
	delete[] psi_down;
    return result;
}

void unkOverlap_pw::test_for_unkOverlap_pw()
{
	
	const int number_pw = GlobalC::pw.ngmw;
	GlobalV::ofs_running << "the GlobalC::pw.ngmw is " << number_pw << std::endl;
	complex<double> *unk_L = new complex<double>[number_pw];
	for (int ig = 0; ig < GlobalC::kv.ngk[0]; ig++)
	{
		unk_L[GlobalC::wf.igk(0,ig)] = GlobalC::wf.evc[0](0, ig);
	}
	for (int ig = 0; ig < GlobalC::pw.ngmw; ig++)
	{
		GlobalV::ofs_running << GlobalC::pw.gdirect[ig].x << "," << GlobalC::pw.gdirect[ig].y << "," << GlobalC::pw.gdirect[ig].z << "  = " << unk_L[ig] << std::endl;
	}	
	
}

