%/* Copyright 2000-2021 Matt Flax <flatmax@flatmax.org>
%   This file is part of GTK+ IOStream class set
%
%   GTK+ IOStream is free software; you can redistribute it and/or modify
%   it under the terms of the GNU General Public License as published by
%   the Free Software Foundation; either version 2 of the License, or
%   (at your option) any later version.
%
%   GTK+ IOStream is distributed in the hope that it will be useful,
%   but WITHOUT ANY WARRANTY; without even the implied warranty of
%   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
%   GNU General Public License for more details.
%
%   You have received a copy of the GNU General Public License
%   along with GTK+ IOStream
%*/

function x=ImpBandLim(s, fs, fi, fa)
% Generate a band limited impulse
% Inputs :
%         s : duration in s
%         fs: sample rate in Hz
%         fi: Minimum frequency in Hz
%         fa: Maximum frequency in Hz
% Output :
%         x : The signal

if nargin==0
    ImpBandLimTest;
    return;
end

N=round(s*fs);
x=zeros(N,1); % zeros of correct duration
x(1)=1.0; % the impulse
X=fft(x); % zero out of band data in the DFT domain
f=linspace(0,fs-fs/N,N);
idxOrig=find(f<fi | f>(fs-fi));
idx=round([1:fi/(fs/N) N-fi/(fs/N)+2:N]);
X(idx,1)=0;
idxOrig=find(f>fa & f<(fs-fa));
idx=round([fa/(fs/N)+2:(fs-fa)/(fs/N)]);
X(idx,1)=0;
% get the time domain signal
x=ifft(X,'symmetric');
% circular shift
% don't worry about circularly shifting for now ...
% xHat=circshift(x,N/2);
% phi=(linspace(0,N-1,N))*pi/2;
% X=X.*exp(-j*phi.');
% xHat=ifft(X);
%     subplot(211)
%     plot(real(xHat))
%     subplot(212)
%     plot(imag(xHat))
% if ~isreal(x)
%     clf
%     subplot(211)
%     plot(real(x))
%     subplot(212)
%     plot(imag(x))
%   error('x should be a real signal');
% end
% clf; plot(x)
end

function ImpBandLimTest

s=1; fs=1024; fi=0; fa=fs/2;
x=ImpBandLim(s, fs, fi, fa);

ax(1)=subplot(211);
semilogx(x);
ax(2)=subplot(212);
semilogx(abs(fft(x)));
end