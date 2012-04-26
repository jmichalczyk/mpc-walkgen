#include "qp-solver.h"


using namespace MPCWalkgen;
using namespace Eigen;


//TODO: NB_VAR_MAX
const int QPSolver::DefaultNbVarMax_=100;
const int QPSolver::DefaultNbCtrMax_=100;

QPSolver::QPSolver(const int nbVarMin, const int nbCtrMin, const int nbVarMax, const int nbCtrMax)
	:matrixQ_(nbVarMin,nbVarMin,nbVarMax,nbVarMax)
	,matrixA_(nbCtrMin,nbVarMin,nbCtrMax,nbVarMax)
	,vectorP_(nbVarMin,1,nbVarMax,1)
	,vectorBU_(nbCtrMin,1,nbCtrMax,1)
	,vectorBL_(nbCtrMin,1,nbCtrMax,1)
	,vectorXU_(nbVarMin,1,nbVarMax,1)
	,vectorXL_(nbVarMin,1,nbVarMax,1)
	,nbVar_(nbVarMin)
	,nbCtr_(nbCtrMin)
	,nbVarMax_(nbVarMax)
	,nbCtrMax_(nbCtrMax)
	,varOrder_(nbVarMax)
	,ctrOrder_(nbVarMax+nbCtrMax)
{
	for(int i=0;i<nbVarMax;++i){
		varOrder_(i)=i;
	}
	for(int i=0;i<nbVarMax+nbCtrMax;++i){
		ctrOrder_(i)=i;
	}
}

QPSolver::~QPSolver(){}

QPMatrix & QPSolver::matrix(const QPMatrixType type){
	switch(type){
		case matrixA:
			return matrixA_;
		case matrixQ:
			return matrixQ_;
		default:
			throw ("Call of QPSolver::matrix with a wrong type attribute " + type);
	}
}


QPVector & QPSolver::vector(const QPMatrixType type){
	switch(type){
		case vectorP:
			return vectorP_;
		case vectorBU:
			return vectorBU_;
		case vectorBL:
			return vectorBL_;
		case vectorXU:
			return vectorXU_;
		case vectorXL:
			return vectorXL_;
		default:
			throw ("Call of QPSolver::vector with a wrong type attribute " + type);
	}
}

void QPSolver::reset(){
	matrixQ_.reset();
	matrixA_.reset();
	vectorP_.reset();
	vectorBU_.reset();
	vectorBL_.reset();
	vectorXU_.reset();
	vectorXL_.reset();
}

void QPSolver::nbVar(const int nbVar){
	if (nbVar!=nbVar_){
		nbVar_=nbVar;
		resizeAll();
	}
}

void QPSolver::nbCtr(const int nbCtr){
	if (nbCtr!=nbCtr_){
		nbCtr_=nbCtr;
		resizeAll();
	}
}

void QPSolver::addNbCtr(const int addCtr){
	if (addCtr>0){
		nbCtr_+=addCtr;
		resizeAll();
	}
}

void QPSolver::varOrder(const Eigen::VectorXi & order){
	varOrder_ = order;
	matrixQ_.rowOrder(order);
	matrixQ_.colOrder(order);
	matrixA_.colOrder(order);
	vectorP_.rowOrder(order);
	vectorXU_.rowOrder(order);
	vectorXL_.rowOrder(order);
}

void QPSolver::ctrOrder(const Eigen::VectorXi & order){
	ctrOrder_=order;
	matrixA_.rowOrder(order);
	vectorBU_.rowOrder(order);
	vectorBL_.rowOrder(order);
}


bool QPSolver::resizeAll(){
	matrixQ_.resize(nbVar_,nbVar_);
	matrixA_.resize(nbCtr_,nbVar_);
	vectorP_.resize(nbVar_);
	vectorBU_.resize(nbCtr_);
	vectorBL_.resize(nbCtr_);
	vectorXU_.resize(nbVar_);
	vectorXL_.resize(nbVar_);

	bool maxSizechanged=false;
	if (nbVar_>nbVarMax_){
		nbVarMax_=nbVar_;
		maxSizechanged=true;
	}
	if (nbCtr_>nbCtrMax_){
		nbCtrMax_=nbCtr_;
		maxSizechanged=true;
	}
	return maxSizechanged;
}

void QPSolver::reorderInitialSolution(VectorXd & initialSolution,
				      VectorXi & initialConstraints){
	assert(initialSolution.size() >= nbVar_);
	assert(initialConstraints.size() >= nbCtr_ + nbVar_);
	VectorXd initialSolutionTmp = initialSolution;
	VectorXi initialConstraintsTmp = initialConstraints;
	for(int i=0;i<nbVar_;++i){
		initialSolution(varOrder_(i))=initialSolutionTmp(i);
		initialConstraints(varOrder_(i))=initialConstraintsTmp(i);
	}
	for(int i=0;i<nbCtr_;++i){
		initialConstraints(ctrOrder_(i+nbVar_))=initialConstraintsTmp(i+nbVar_);
	}

}

void QPSolver::reorderSolution(VectorXd & qpSolution, VectorXi & constraints,
			       VectorXi & initialConstraints){
	VectorXd solutionTmp = qpSolution;
	VectorXi constraintsTmp = constraints;

	for (int i=0;i<nbVar_;++i) {
		qpSolution(i) = solutionTmp(varOrder_(i));
		constraints(i) = constraintsTmp(varOrder_(i));
	}
	for(int i=0;i<nbCtr_;++i) {
		constraints(i+nbVar_) = constraintsTmp(ctrOrder_(i+nbVar_));
	}

	initialConstraints= constraints;
}

void QPSolver::dump(){
	std::cout << "nbVar : " << nbVar_ << std::endl;
	std::cout << "nbCtr : " << nbCtr_ << std::endl;
	std::cout << std::endl;
	std::cout << "Q :" << std::endl;
	std::cout << matrixQ_() << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << "chol(Q) :" << std::endl;
	std::cout << matrixQ_.cholesky() << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << "P :" << std::endl;
	std::cout << vectorP_() << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << "A :" << std::endl;
	std::cout << matrixA_() << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << "Bl / Bu :" << std::endl;
	std::cout << vectorBL_() << std::endl;
	std::cout << vectorBU_() << std::endl;
	std::cout << std::endl;
	std::cout << "Xl / Xu :" << std::endl;
	std::cout << vectorXL_() << std::endl;
	std::cout << vectorXU_() << std::endl;
}