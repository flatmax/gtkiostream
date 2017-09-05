function yHat=IIRCascadeTest(A, B, x, y)
yTemp=x{1};
for i=1:size(A,2)
    xHat{i}=yTemp;
    yTemp=filter(B(:,i), A(:,i), yTemp);
    yHat{i}=yTemp;
end
yHat=yHat{end};
end