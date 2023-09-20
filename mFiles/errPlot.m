function ddat=errPlot(varargin)
% varargin{1}
dat=[];
datStr={};
freqPlot=0;
for i=1:length(varargin)
    v=varargin{i};
	if inputname(i)=='f'
		freqPlot=1;
		f=v;
	else
		datStr{end+1}=inputname(i);
		dat=[dat v];
	end
end
ax(1)=subplot(211);
if freqPlot
	semilogx(f, dat); grid on;
	ylabel('dB');
else
	plot(dat); grid on;
	ylabel('Amp.');
end
legend(datStr, 'location', 'best');
ax(2)=subplot(212);
if size(dat,2)>1
	ddat=dat(:,1)-dat(:,2:end);
	if freqPlot
		semilogx(f, ddat); grid on;
	else
		plot(ddat); grid on;
	end
	title(['RMS err ' num2str(rms(ddat),'%.1e ') ' effective RMS err ' num2str(rms(ddat)/rms(dat(:,1)),'%.1e ')])
end
ylabel('error');
linkaxes(ax,'x');
end

function y=rms(x)
y=sqrt(mean(x.^2));
end
