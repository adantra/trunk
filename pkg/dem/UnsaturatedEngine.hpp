/*************************************************************************
*  Copyright (C) 2012 by Chao Yuan <chao.yuan@3sr-grenoble.fr>           *
*  Copyright (C) 2012 by Bruno Chareyre <bruno.chareyre@hmg.inpg.fr>     *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#pragma once
#include<yade/core/PartialEngine.hpp>
#include<yade/pkg/dem/TriaxialCompressionEngine.hpp>
#include<yade/pkg/dem/TesselationWrapper.hpp>
#include<yade/lib/triangulation/FlowBoundingSphere.hpp>

class Flow;
class TesselationWrapper;
#define _FlowSolver CGT::FlowBoundingSphere<UnsatTesselation>
#define TPL template<class Solver>

class UnsaturatedEngine : public PartialEngine
{
	public :
	typedef _FlowSolver							FlowSolver;
	typedef FlowTesselation							Tesselation;
	typedef FlowSolver::RTriangulation					RTriangulation;
	typedef FlowSolver::Finite_vertices_iterator                    	Finite_vertices_iterator;
	typedef FlowSolver::Finite_cells_iterator				Finite_cells_iterator;
	typedef FlowSolver::Cell_handle						Cell_handle;
	typedef RTriangulation::Finite_edges_iterator				Finite_edges_iterator;
	typedef RTriangulation::Vertex_handle					Vertex_handle;
	typedef RTriangulation::Point						CGALSphere;
	typedef CGALSphere::Point						Point;
	
	typedef std::vector<Cell_handle>					Vector_Cell;
	typedef typename Vector_Cell::iterator					VCell_iterator;

	protected:
		shared_ptr<FlowSolver> solver;
		shared_ptr<FlowSolver> backgroundSolver;
		volatile bool backgroundCompleted;
		Cell cachedCell;
		struct posData {Body::id_t id; Vector3r pos; Real radius; bool isSphere; bool exists; posData(){exists=0;}};
		vector<posData> positionBufferCurrent;//reflect last known positions before we start computations
		vector<posData> positionBufferParallel;//keep the positions from a given step for multithread factorization
// 		//copy positions in a buffer for faster and/or parallel access
		void setPositionsBuffer(bool current);
		void testFunction();

	public :
		enum {wall_xmin, wall_xmax, wall_ymin, wall_ymax, wall_zmin, wall_zmax};		
		Vector3r normal [6];
		bool currentTes;
		int id_offset;
		TPL void initSolver (Solver& flow);
		TPL void Triangulate (Solver& flow);
		TPL void AddBoundary (Solver& flow);
		TPL void Build_Triangulation (double P_zero, Solver& flow);
		TPL void Build_Triangulation (Solver& flow);
		TPL void Initialize_volumes (Solver& flow);
		TPL void BoundaryConditions(Solver& flow);
		TPL void initializeCellIndex(Solver& flow);
		TPL void initializePoreRadius(Solver& flow);
		
		TPL void invade (Solver& flow );
		TPL Real getMinEntryValue (Solver& flow);
		TPL Real getSaturation(Solver& flow);
		
		TPL void invadeSingleCell1(Cell_handle cell, double pressure, Solver& flow);
		TPL void invade1 (Solver& flow );
		TPL void checkTrap(Solver& flow, double pressure);//check trapped phase, define trapCapP.	
		TPL Real getMinEntryValue1 (Solver& flow);
		TPL void updatePressure(Solver& flow);
		TPL void updateReservoir(Solver& flow);		
		TPL void initWaterReservoirBound(Solver& flow);
		TPL void updateWaterReservoir(Solver& flow);
		TPL void waterReservoirRecursion(Cell_handle cell, Solver& flow);
		TPL void initAirReservoirBound(Solver& flow);
		TPL void updateAirReservoir(Solver& flow);
		TPL void airReservoirRecursion(Cell_handle cell, Solver& flow);
		TPL Real getSaturation1(Solver& flow);

		TPL void invadeSingleCell2(Cell_handle cell, double pressure, Solver& flow);
		TPL void invade2 (Solver& flow );
		TPL void updatePressure2(Solver& flow);
		TPL Real getMinEntryValue2 (Solver& flow);
		TPL Real getSaturation2(Solver& flow);	
		
		TPL unsigned int imposePressure(Vector3r pos, Real p,Solver& flow);
		TPL void setImposedPressure(unsigned int cond, Real p,Solver& flow);
		TPL void clearImposedPressure(Solver& flow);
		TPL void saveListNodes(Solver& flow);
		TPL void saveListConnection(Solver& flow);
		TPL void saveLatticeNodeX(Solver& flow,double x); 
		TPL void saveLatticeNodeY(Solver& flow,double y); 
		TPL void saveLatticeNodeZ(Solver& flow,double z);
		TPL void saveListAdjCellsTopBound(Solver& flow);
		TPL void saveListAdjCellsBottomBound(Solver& flow);		
		TPL void savePoreBodyInfo(Solver& flow);
		TPL void savePoreThroatInfo(Solver& flow);
		TPL void debugTemp(Solver& flow);
		
		TPL void computeSolidLine(Solver& flow);
		TPL void computeFacetPoreForcesWithCache(Solver& flow, bool onlyCache=false);
		
		TPL Vector3r fluidForce(unsigned int id_sph, Solver& flow) {
			const CGT::Vecteur& f=flow->T[flow->currentTes].vertex(id_sph)->info().forces; return Vector3r(f[0],f[1],f[2]);}
		
		template<class Cellhandle >
		double getRadiusMin(Cellhandle cell, int j);
		template<class Cellhandle>
		Real Volume_cell_single_fictious (Cellhandle cell);
		template<class Cellhandle>
		Real Volume_cell_double_fictious (Cellhandle cell);
		template<class Cellhandle>
		Real Volume_cell_triple_fictious (Cellhandle cell);
		template<class Cellhandle>
		Real Volume_cell (Cellhandle cell);
		template<class Solver>
		void updateVolumeCapillaryCell ( Solver& flow);		
		template<class Cellhandle>
		double computeEffPoreRadius(Cellhandle cell, int j);
		template<class Cellhandle>
		double computeEffPoreRadiusNormal(Cellhandle cell, int j);
		template<class Cellhandle>
		double bisection(Cellhandle cell, int j, double a, double b);
		template<class Cellhandle>
		double computeDeltaForce(Cellhandle cell,int j, double rcap);
		template<class Cellhandle>
		Real computePoreArea(Cellhandle cell, int j);
		template<class Cellhandle>
		Real computePorePerimeter(Cellhandle cell, int j);		
		void saveVtk(const char* folder) {bool initT=solver->noCache; solver->noCache=false; solver->saveVtk(folder); solver->noCache=initT;}
		python::list getConstrictions() {
			vector<Real> csd=solver->getConstrictions(); python::list pycsd;
			for (unsigned int k=0;k<csd.size();k++) pycsd.append(csd[k]); return pycsd;}
		double getPorePressure(Vector3r pos){return solver->getPorePressure(pos[0], pos[1], pos[2]);}
		TPL int getCell(double posX, double posY, double posZ, Solver& flow){return flow->getCell(posX, posY, posZ);}

		void emulateAction(){
			scene = Omega::instance().getScene().get();
			action();}
		unsigned int 	_imposePressure(Vector3r pos, Real p) {return imposePressure(pos,p,solver);}
		void 		_setImposedPressure(unsigned int cond, Real p) {setImposedPressure(cond,p,solver);}
		void 		_clearImposedPressure() {clearImposedPressure(solver);}
		int		_getCell(Vector3r pos) {return getCell(pos[0],pos[1],pos[2],solver);}
		void 		_buildTriangulation() {setPositionsBuffer(true); Build_Triangulation(solver);}
		void		_invade() {invade(solver);}
		Real		_getMinEntryValue() {return getMinEntryValue(solver);}
		Real		_getSaturation() {return getSaturation(solver);}
		void		_saveListNodes() {saveListNodes(solver);}
		void		_saveListConnection() {saveListConnection(solver);}
 		void		_saveLatticeNodeX(double x) {saveLatticeNodeX(solver,x);}
 		void		_saveLatticeNodeY(double y) {saveLatticeNodeY(solver,y);}
 		void		_saveLatticeNodeZ(double z) {saveLatticeNodeZ(solver,z);}
 		void 		_saveListAdjCellsTopBound() {saveListAdjCellsTopBound(solver);}
 		void 		_saveListAdjCellsBottomBound() {saveListAdjCellsBottomBound(solver);}
 		void		_savePoreBodyInfo(){savePoreBodyInfo(solver);}
 		void		_savePoreThroatInfo(){savePoreThroatInfo(solver);}
 		void		_debugTemp(){debugTemp(solver);}
 		void		_computeFacetPoreForcesWithCache(){computeFacetPoreForcesWithCache(solver);}
		Vector3r 	_fluidForce(unsigned int id_sph) {return fluidForce(id_sph,solver);}
		
		virtual ~UnsaturatedEngine();

		virtual void action();

		YADE_CLASS_BASE_DOC_ATTRS_DEPREC_INIT_CTOR_PY(UnsaturatedEngine,PartialEngine,"Preliminary version engine of a model for unsaturated soils",
					((bool,isActivated,true,,"Activate UnsaturatedEngine."))
					((bool,first,true,,"Controls the initialization/update phases"))
					((bool, Debug, false,,"Activate debug messages"))
					((bool, isPhaseTrapped, true,,"Activate invade mode. If True, the wetting phase can be trapped, activate invade mode 1; if false, the wetting phase cann't be trapped, activate invade mode 2."))
					((double, wall_thickness,0.001,,"Walls thickness"))
					((double,P_zero,0,,"The value used for initializing pore pressure. It is useless for incompressible fluid, but important for compressible model."))
					((bool, updateTriangulation, 0,,"If true the medium is retriangulated."))
					((double,gasPressure,0,,"Invasion pressure"))
					((double,surfaceTension,0.0728,,"Water Surface Tension in contact with air at 20 Degrees Celsius is: 0.0728(N/m)"))
					((double, porosity, 0,,"Porosity computed at each retriangulation"))
					((int, xmin,0,(Attr::readonly),"Index of the boundary $x_{min}$. This index is not equal the the id of the corresponding body in general, it may be used to access the corresponding attributes (e.g. flow.bndCondValue[flow.xmin], flow.wallId[flow.xmin],...)."))
					((int, xmax,1,(Attr::readonly),"See :yref:`FlowEngine::xmin`."))
					((int, ymin,2,(Attr::readonly),"See :yref:`FlowEngine::xmin`."))
					((int, ymax,3,(Attr::readonly),"See :yref:`FlowEngine::xmin`."))
					((int, zmin,4,(Attr::readonly),"See :yref:`FlowEngine::xmin`."))
					((int, zmax,5,(Attr::readonly),"See :yref:`FlowEngine::xmin`."))

					((vector<bool>, bndCondIsPressure, vector<bool>(6,false),,"defines the type of boundary condition for each side. True if pressure is imposed, False for no-flux. Indexes can be retrieved with :yref:`FlowEngine::xmin` and friends."))
					((vector<double>, bndCondValue, vector<double>(6,0),,"Imposed value of a boundary condition. Only applies if the boundary condition is imposed pressure, else the imposed flux is always zero presently (may be generalized to non-zero imposed fluxes in the future)."))
					//FIXME: to be implemented:
					((vector<Vector3r>, boundaryVelocity, vector<Vector3r>(6,Vector3r::Zero()),, "velocity on top boundary, only change it using :yref:`FlowEngine::setBoundaryVel`"))
					((vector<int>, wallIds,vector<int>(6),,"body ids of the boundaries (default values are ok only if aabbWalls are appended before spheres, i.e. numbered 0,...,5)"))
					((vector<bool>, boundaryUseMaxMin, vector<bool>(6,true),,"If true (default value) bounding sphere is added as function of max/min sphere coord, if false as function of yade wall position"))					

					((bool, pressureForce, true,,"Compute the pressure field and associated fluid forces. WARNING: turning off means fluid flow is not computed at all."))
					((bool, computeForceActivated, true,,"Activate capillary force computation. WARNING: turning off means capillary force is not computed at all, but the drainage can still work."))
					((bool, invadeBoundary, false,,"Invade from boundaries."))
					,
					/*deprec*/
					,,
					for (int i=0; i<6; ++i){normal[i]=Vector3r::Zero(); wallIds[i]=i;}
					normal[wall_ymin].y()=normal[wall_xmin].x()=normal[wall_zmin].z()=1;
					normal[wall_ymax].y()=normal[wall_xmax].x()=normal[wall_zmax].z()=-1;
					solver = shared_ptr<FlowSolver> (new FlowSolver);
					first=true;
					,
					.def("imposePressure",&UnsaturatedEngine::_imposePressure,(python::arg("pos"),python::arg("p")),"Impose pressure in cell of location 'pos'. The index of the condition is returned (for multiple imposed pressures at different points).")
					.def("setImposedPressure",&UnsaturatedEngine::_setImposedPressure,(python::arg("cond"),python::arg("p")),"Set pressure value at the point indexed 'cond'.")
					.def("clearImposedPressure",&UnsaturatedEngine::_clearImposedPressure,"Clear the list of points with pressure imposed.")
					.def("getConstrictions",&UnsaturatedEngine::getConstrictions,"Get the list of constrictions (inscribed circle) for all finite facets.")
					.def("saveVtk",&UnsaturatedEngine::saveVtk,(python::arg("folder")="./VTK"),"Save pressure field in vtk format. Specify a folder name for output.")
					.def("getPorePressure",&UnsaturatedEngine::getPorePressure,(python::arg("pos")),"Measure pore pressure in position pos[0],pos[1],pos[2]")
					.def("emulateAction",&UnsaturatedEngine::emulateAction,"get scene and run action (may be used to manipulate engine outside the main loop).")
					.def("getCell",&UnsaturatedEngine::_getCell,(python::arg("pos")),"get id of the cell containing (X,Y,Z).")
					.def("testFunction",&UnsaturatedEngine::testFunction,"The playground for Chao's experiments.")
					.def("buildTriangulation",&UnsaturatedEngine::_buildTriangulation,"Triangulate spheres of the current scene.")
					.def("getSaturation",&UnsaturatedEngine::_getSaturation,"get saturation.")					
					.def("getMinEntryValue",&UnsaturatedEngine::_getMinEntryValue,"get the minimum air entry pressure for the next invade step.")
					.def("saveListNodes",&UnsaturatedEngine::_saveListNodes,"Save the list of nodes.")
					.def("saveListConnection",&UnsaturatedEngine::_saveListConnection,"Save the connections between cells.")
					.def("saveLatticeNodeX",&UnsaturatedEngine::_saveLatticeNodeX,(python::arg("x")),"Save the slice of lattice nodes for x_normal(x). 0: out of sphere; 1: inside of sphere.")
					.def("saveLatticeNodeY",&UnsaturatedEngine::_saveLatticeNodeY,(python::arg("y")),"Save the slice of lattice nodes for y_normal(y). 0: out of sphere; 1: inside of sphere.")
					.def("saveLatticeNodeZ",&UnsaturatedEngine::_saveLatticeNodeZ,(python::arg("z")),"Save the slice of lattice nodes for z_normal(z). 0: out of sphere; 1: inside of sphere.")
					.def("saveListAdjCellsTopBound",&UnsaturatedEngine::_saveListAdjCellsTopBound,"Save the cells IDs adjacent top boundary(connecting water reservoir).")
					.def("saveListAdjCellsBottomBound",&UnsaturatedEngine::_saveListAdjCellsBottomBound,"Save the cells IDs adjacent bottom boundary(connecting air reservoir).")
					.def("savePoreBodyInfo",&UnsaturatedEngine::_savePoreBodyInfo,"Save pore bodies positions/Voronoi centers and size/volume.")
					.def("savePoreThroatInfo",&UnsaturatedEngine::_savePoreThroatInfo,"Save pore throat area, inscribed radius and perimeter.")
					.def("debugTemp",&UnsaturatedEngine::_debugTemp,"debug temp file.")
					.def("invade",&UnsaturatedEngine::_invade,"Run the drainage invasion.")
					.def("computeForce",&UnsaturatedEngine::_computeFacetPoreForcesWithCache,"Compute capillary force. ")
					.def("fluidForce",&UnsaturatedEngine::_fluidForce,(python::arg("Id_sph")),"Return the fluid force on sphere Id_sph.")
					)
		DECLARE_LOGGER;
};

REGISTER_SERIALIZABLE(UnsaturatedEngine);
