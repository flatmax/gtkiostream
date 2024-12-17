function [xRet,fval,exitflag,output, residual]=optimiserRunner(dat)

% example setup :
% dat.HHatAbs = abs(HHat);
% dat.N = length(dat.HHatAbs );
% dat.fs = fs;
% dat.f = f;
% No=2;
% dat.x0 = linspace(0.7, 0.1, No);
% dat.lb = -1.5*ones(size(dat.x0));
% dat.ub = 1.5*ones(size(dat.x0));
% dat.imp = zeros(dat.N,1);
% dat.imp(1)=1;
% dat.getErrorLocal = @getErrorLocal
% dat.whichSolver.useFSolve=1;
% dat.whichSolver.useLSQNonlin=0;
% dat.whichSolver.useLSQCurvefit=0;
% dat.whichSolver.useFMincon=0;


whichSolver = dat.whichSolver;
[solver, optionsFs, efnFsLs, optionsLsqNL, optionsLsqCF, efnLsqCF, optionsFmin, efnFmin]=getSolverOptions(dat, whichSolver);

st=tic;
residual = [];
%     try
solver
switch solver
	case 'fsolve'
		[xRet,fval,exitflag,output]=fsolve(efnFsLs, dat.x0, optionsFs);
		residual=fval;
		er=sum(sum(abs(fval).*abs(fval)));
		er=rms(er);
	case 'lsqcurvefit'
		% [xRet,fval,exitflag,output] = lsqcurvefit(efnLsqCF,x0,dat.h, dat.ident, lb,ub,optionsLsqCF);
		[xRet,fval,exitflag,output] = lsqcurvefit(efnLsqCF, dat.x0, [], dat.ident, dat.lb, dat.ub,optionsLsqCF);
		er=fval;
		er=rms(er);
	case 'lsqnonlin'
		[xRet,fval,residual, exitflag,output]=lsqnonlin(efnFsLs, dat.x0, dat.lb, dat.ub, optionsLsqNL);
		er=fval;
		er=rms(er);
	case 'fmincon'
		[xRet,fval,exitflag,output]=fmincon(efnFmin, dat.x0, [], [], [], [], dat.lb, dat.ub, [], optionsFmin);
		er=getErrorXfMincon(xRet, dat, solver);
	otherwise
		error('unknown solver');
end
toc

% figure(1); clf
% errPlot(dat.h, xRet, output.bestfeasible.x);
% figure(2); clf
% id=filter(dat.h, 1, dat.hPadded);
% idHat=filter(1, xRet, dat.hPadded);
% id=id(1:N);
% idHat=idHat(1:N);
% errPlot(id, idHat);
end

function e=getErrorXfMincon(x, dat, solver)
e=getErrorX(x, dat, solver);
e=((sum(sum(abs(e)))));
title(num2str(e,'%.2e'))
end

function e=getErrorXLsqCF(x, xdata, N, dlhHat, solver)
e=dat.getErrorLocal(b, a, dlhHat, solver);
end

function e=getErrorX(x, dat, solver)
e=dat.getErrorLocal(x, dat, solver);
end

function [solver, optionsFs, efnFsLs, optionsLsqNL, optionsLsqCF, efnLsqCF, optionsFmin, efnFmin]=getSolverOptions(dat, whichSolver)
solver=[];
optionsFs=[]; efnFsLs=[]; optionsLsqNL=[]; efnFsLs=[]; optionsLsqCF=[]; efnLsqCF=[]; optionsFmin=[]; efnFmin=[];
if whichSolver.useFMincon
	solver='fmincon';
% 	optionsFmin = optimoptions('fmincon','SpecifyObjectiveGradient',true);
 	optionsFmin = optimset('fmincon');
	optionsFmin.MaxIterations = 2e3;
	optionsFmin.MaxIter = optionsFmin.MaxIterations;
	optionsFmin.MaxFunctionEvaluations = 20e5;
	optionsFmin.MaxFunEvals = optionsFmin.MaxFunctionEvaluations;
	optionsFmin.TolFun = 1e-24;
	optionsFmin.TolCon = 1e-24;
	optionsFmin.TolX = 1e-24;
	optionsFmin.FinDiffRelStep = 1e-16;
	% optionsFmin.SpecifyObjectiveGradient=true;

	optionsFmin.Display='final';
	%     optionsFmin.UseParallel=true;
	efnFmin=@(x)getErrorXfMincon(x, dat, solver);
end

if whichSolver.useLSQNonlin
	solver='lsqnonlin';
	%     optionsLsqNL = optimoptions('lsqnonlin');
	optionsLsqNL = optimset('lsqnonlin');
	optionsLsqNL.MaxIterations = 2e3;
	optionsLsqNL.MaxIter = optionsLsqNL.MaxIterations;
	optionsLsqNL.MaxFunctionEvaluations = 20e3;
	optionsLsqNL.MaxFunEvals = optionsLsqNL.MaxFunctionEvaluations;
	optionsLsqNL.Display='final';
	efnFsLs=@(x)getErrorX(x, dat, solver);
end

if whichSolver.useLSQCurvefit
	solver='lsqcurvefit'; % not the best
	%     optionsLsqCF = optimoptions('lsqcurvefit');
	optionsLsqCF = optimset('lsqcurvefit');
	optionsLsqCF.MaxIterations = 2e3;
	optionsLsqCF.MaxIter = optionsLsqCF.MaxIterations;
	optionsLsqCF.MaxFunctionEvaluations = 20e3;
	optionsLsqCF.MaxFunEvals = optionsLsqCF.MaxFunctionEvaluations;
	optionsLsqCF.Display='iter';
	efnLsqCF=@(x, xdata)getErrorXLsqCF(x, xdata, N, dlhHat, solver);
end

if whichSolver.useFSolve
	solver='fsolve';
%   	optionsFs = optimoptions('fsolve','SpecifyObjectiveGradient',true);
  	optionsFs = optimset('fsolve');
	optionsFs.Algorithm='levenberg-marquardt';
	optionsFs.MaxIterations = 2e3;
	optionsFs.MaxFunctionEvaluations = 20e3;
	optionsFs.MaxFunEvals = optionsFs.MaxFunctionEvaluations;
	optionsFs.StepTolerance = eps;
	optionsFs.TolFun = eps;
	optionsFs.TolX = eps;
	% optionsFs.FiniteDifferenceStepSize = 1e-13;

	optionsFs.Display='final';
	%     optionsFs.UseParallel=true;
	efnFsLs=@(x)getErrorX(x, dat, solver);
end
if isempty(solver) error('choose at least one solver'); end
%  optionsFs.Display='none';
optionsLsqNL.Display='none';
optionsLsqCF.Display='none';
optionsFmin.Display='iter';
end
