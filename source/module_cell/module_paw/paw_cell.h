// The Paw_Cell class stores PAW-related information in unitcell
// including number of atoms, number of element types, and so on
// it basically serves as an interface between the unitcell class
// and the libpaw library from ABINIT

#ifndef PAW_CELL
#define PAW_CELL

#include <vector>
#include <complex>
#include <string>

#include "paw_element.h"
#include "paw_atom.h"
#include "module_base/matrix3.h"

class Paw_Cell
{
    public:

    Paw_Cell(){};
    ~Paw_Cell(){};

// PART I. Operations in ABACUS

    void init_paw_cell(
        const double ecutwfc_in, const double cell_factor_in,
        const double omega_in,
        const int nat_in, const int ntyp_in,
        const int * atom_type_in, const double ** atom_coord_in,
        const std::vector<std::string> & filename_list_in,
        const int nx_in, const int ny_in, const int nz_in,
        const std::complex<double> * eigts1_in, const std::complex<double> * eigts2_in, const std::complex<double> * eigts3_in);

    // Given a list of k points, calculate the structure factors
    // exp(-i(k+G)R_I) = exp(-ikR_I) exp(-iG_xR_Ix) exp(-iG_yR_Iy) exp(-iG_zR_Iz)
    // as well as the spherical harmonics Ylm(k+G), and gnorm
    void set_paw_k(
        const int npw_in, const double * kpt,
        const int * ig_to_ix, const int * ig_to_iy, const int * ig_to_iz,
        const double ** kpg, const double tpiba);

    // This is one of the core functionalities of this class, which reads a wavefunction
    // psi(G), calculates its overlap with all projectors <psi(G)|ptilde(G)>,
    // then accumulates the contribution of this wavefunction to rhoij
    // Note k-point information is not passed here, but prepared in set_paw_k
    void accumulate_rhoij(const std::complex<double> * psi, const double weight);

    // returns rhoij for each atom
    std::vector<std::vector<double>> get_rhoij();
    // returns rhoijp and related info for each atom
    void get_rhoijp(std::vector<std::vector<double>> & rhoijp,
        std::vector<std::vector<int>> & rhoijselect, std::vector<int> & nrhoijsel);

    // calculates vkb, which stores the reciprocal space projectors
    // it is of size (nproj_tot,npw), and named vkb according to
    // its counterpart in nonlocal PP
    void get_vkb();

    //returns vkb
    std::vector<std::vector<std::complex<double>>> output_vkb(){return vkb;}

    int get_nproj_tot(){return nproj_tot;}
    // map projector to atom
    std::vector<int> get_iprj_to_ia(){return iprj_to_ia;}
    // map projector to mstate of that element
    std::vector<int> get_iprj_to_im(){return iprj_to_im;}
    // map projector to lstate of that element
    std::vector<int> get_iprj_to_il(){return iprj_to_il;}
    // map projector to l quantum number of that element
    std::vector<int> get_iprj_to_l() {return iprj_to_l;}
    // map projector to m quantum number of that element
    std::vector<int> get_iprj_to_m() {return iprj_to_m;}
    // start index of projector on atom ia
    std::vector<int> get_start_iprj() {return start_iprj_ia;}
    // max l quantum number of all elements
    int get_lmax(){return lmax;}

    // ylm(r), adapted from initylmg of ABINIT
    static std::vector<double> calc_ylm(const int lmax, const double * r);

    // helper function for calc_ylm: Legendre polynomial
    static double ass_leg_pol(const int l, const int m, const double arg);

    private:

    // based on list of atom type, calculate total number of projectors
    // of this system, then map each projector to atom index, mstate and lstate
    // record in iproj_to_ia/im/il
    void map_paw_proj();

    // array of paw_element
    std::vector<Paw_Element> paw_element_list;

    // array of paw_atom
    std::vector<Paw_Atom> paw_atom_list;

    int nproj_tot; // total number of projectors
    std::vector<int> iprj_to_ia; // map projector to atom
    std::vector<int> iprj_to_im; // map projector to mstate of that element
    std::vector<int> iprj_to_il; // map projector to lstate of that element
    std::vector<int> iprj_to_l;  // map projector to l quantum number of that element
    std::vector<int> iprj_to_m;  // map projector to m quantum number of that element
    std::vector<int> start_iprj_ia; // start index of projector on atom ia

    // max l quantum number of all elements
    int lmax;

    // atomic positions and types
    int nat;
    int ntyp;
    std::vector<int> atom_type; // the element type of each atom
    std::vector<std::vector<double>> atom_coord; // Cartesian coordinate of each atom (in Bohr)
    std::vector<int> nat_type; // #. atoms of each type
    // This array arranges the indexes of atoms according to element type
    // For example, if atom_type equals 0,1,2,1,2, then
    // atom_map is 0,1,3,2,4
    std::vector<int> atom_map;

    // volume of cell
    double omega;

    // FFT grid
    int nx, ny, nz;
    int npw;

    // The reciprocal space projectors; it is called vkb
    // to be consistent with non-local PP
    std::vector<std::vector<std::complex<double>>> vkb;

    // structure factor ('eigts1-3' from structure_factor class)
    // stores exp(- i G R_I) where G = (Gx,0,0), (0,Gy,0) and (0,0,Gz)
    std::vector<std::vector<std::complex<double>>> eigts1;
    std::vector<std::vector<std::complex<double>>> eigts2;
    std::vector<std::vector<std::complex<double>>> eigts3;

    // structure factor of (k+G) for current k point
    // dimension : natom * npw
    std::vector<std::vector<std::complex<double>>> struc_fact;

    // spherical harmonics Y_lm (k+G) for current k point
    // dimension : npw * #. (l,m) channels
    // This thing is calculated on the fly in treatment of nonlocal PP in ABACUS
    // but is calculated and stored in ABINIT
    // I'm going to follow the latter
    std::vector<std::vector<double>> ylm_k;

    // dimension : npw
    // norm of (k+G), used for interpolation of ptilde(G)
    // this is also calculated on the fly in nonlocal PP
    // but since it has same dimension of dim 1 of ylm_k
    // I'd rather also calculate it once and save it
    std::vector<double> gnorm;

    void set_ylm(const int npw_in, const double ** kpg);

// Part II. Passing infor for the initialization of PAW

    public:
    // The following gathers information needed by LibPAW, they will be inserted
    // to proper places in the main program

    // Cutoff energies, sets ecut and ecutpaw
    void set_libpaw_ecut(const double ecut_in, const double ecutpaw_in);
    // Sets rprimd, gprimd, gmet and ucvol
    // Input is latvec and lat0, will calculate the required info
    void set_libpaw_cell(const ModuleBase::Matrix3 latvec, const double lat0);
    // FFT grid information, sets ngfft and ngfftdg
    void set_libpaw_fft(const int nx_in, const int ny_in, const int nz_in,
        const int nxdg_in, const int nydg_in, const int nzdg_in);
    // Sets natom, ntypat, typat and xred
    void set_libpaw_atom(const int natom_in, const int ntypat_in, const int * typat_in, const double * xred_in);
    // Sets filename_list
    void set_libpaw_files();
    
    // Extract the information
    double get_libpaw_ecut() {return ecut;}
    double get_libpaw_ecutpaw() {return ecutpaw;}
    std::vector<double> get_libpaw_rprimd() {return rprimd;}
    std::vector<double> get_libpaw_gprimd() {return gprimd;}
    std::vector<double> get_libpaw_gmet() {return gmet;}
    double get_libpaw_ucvol() {return ucvol;}
    std::vector<int> get_libpaw_ngfft() {return ngfft;}
    std::vector<int> get_libpaw_ngfftdg() {return ngfftdg;}
    int get_libpaw_natom() {return natom;}
    int get_libpaw_ntypat() {return ntypat;}
    std::vector<int> get_libpaw_typat() {return typat;}
    std::vector<double> get_libpaw_xred() {return xred;}
    char* get_libpaw_filename_list() {return filename_list;}

    private:
// Info to be passed to libpaw_interface:
// 1. ecut, ecutpaw : kinetic energy cutoff of the planewave basis set
// there will be one coarse grid for density/potential, and a fine grid for PAW
// the unit is in Hartree
// 2. rprimd, gprimd : real and reciprocal space lattice vectors, respectively
// unit for rprimd is in Bohr, and for gprimd is in Bohr^-1
// 3. gmet : reciprocal space metric (bohr^-2)
// 4. ucvol : volume of unit cell (Bohr^3)
// 5. ngfft, ngfftdg : dimension of FFT grids of the corase and fine grids
// 6. natom, ntypat, typat: #. atoms, #. element types
// and typat records the type of each atom
// 7. xred : coordinate of each atom, in terms of rprimd (namely, direct coordinate)
// 8. filename_list : filename of the PAW xml files for each element

    double ecut, ecutpaw;
    std::vector<double> rprimd, gprimd, gmet;
    double ucvol;
    std::vector<int> ngfft, ngfftdg;
    int natom, ntypat;
    std::vector<int> typat;
    std::vector<double> xred;
    char* filename_list;
};

#endif