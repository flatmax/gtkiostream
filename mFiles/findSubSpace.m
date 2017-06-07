%/* Copyright 2000-2014 Matt Flax <flatmax@flatmax.org>
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

function eigenvals=findSubSpace(signal, f, mask)
%load /tmp/signal.mat
%signal=s;
signal=signal(:); % ensure a column vector
%clear s
p=round(.25*length(signal)); % Estimate for a large number of sinusoids
% this is a correlation matrix
signal=signal(fliplr(repmat(1:2*p,length(signal)-2*p+1,1))+repmat((1:length(signal)-2*p+1)',1,2*p)-1)/sqrt(length(signal)-(2*p-1));
size(signal);
[U,S,eigenvects] = svd(signal,0);
eigenvals = diag(S).^2; % We need to square the singular values here

%UHat=U; SHat=S; eigenvectsHat=eigenvects; eigenvalsHat=eigenvals;
%load /tmp/results.mat
%stack=dbstack; dbstop(stack(1).name,stack(1).line+1) % stop octave & debug from the next line onwards

end
