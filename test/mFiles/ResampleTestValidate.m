function ResampleTestValidate
  if ~exist('/tmp/x.txt')
    error('can''t find some of the test/ResampleTest output files, please run test/ResampleTest first.');
    return
  end

  load /tmp/x.txt
  load /tmp/y.txt
  if any(x-y)
      error('ResampleTestValidate: x-y != 0 : broken C++ implementation');
  end
  
  load /tmp/yp9.txt
  if DFTComp(x,yp9)>2e-5
      error('yp9 has a large error');
  end
  load /tmp/yp8.txt
  if DFTComp(x,yp8)>2e-5
      error('yp8 has a large error');
  end
  load /tmp/ym9.txt
  if DFTComp(x,ym9)>2e-5
      error('ym9 has a large error');
  end
  load /tmp/ym8.txt
  if DFTComp(x,ym8)>2e-5
      error('ym8 has a large error');
  end
end

function mE=DFTComp(x,yp9)
  X=(fft(x));
  YP9=(fft(yp9));
  N=floor(min(size(x,1)/2, size(yp9,1)/2));
  mE=0;
  for c=1:size(x,2)
      err=((X(1:N,c)-YP9(1:N,c))./X(1:N,c));
      [mETemp, idx]=max(abs(err));
      if mETemp>mE
          mE=mETemp;
      end
%       plot(abs(X(1:N,1))); hold on
%       plot(abs(YP9(1:N,1))); hold off
      fprintf('channel %d, idx=%d, max error = %.2e with X = %.2e, %.2e and Y = %.2e, %.2e \n', c, idx, mETemp, real(X(idx,c)), imag(X(idx,c)), real(YP9(idx,c)), imag(YP9(idx,c)))
  end
end