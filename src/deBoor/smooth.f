      double precision function smooth ( x, y, dy, npoint, s, v, a )

c*********************************************************************72
c
cc SMOOTH constructs the cubic smoothing spline to given data.
c
c  from  * a practical guide to splines *  by c. de boor    
calls  setupq, chol1d
c
c  constructs the cubic smoothing spline  f  to given data  (x(i),y(i)),
c  i=1,...,npoint, which has as small a second derivative as possible
c  while
c  s(f) = sum( ((y(i)-f(x(i)))/dy(i))**2 , i=1,...,npoint ) .le. s .
c
c******  i n p u t  ******
c  x(1),...,x(npoint)   data abscissae,  a s s u m e d  to be strictly
c        increasing .
c  y(1),...,y(npoint)     corresponding data ordinates .
c  dy(1),...,dy(npoint)     estimate of uncertainty in data,  a s s u m-
c        e d  to be positive .
c  npoint.....number of data points,  a s s u m e d  .gt. 1
c  s.....upper bound on the discrete weighted mean square distance of
c        the approximation  f  from the data .
c
c******  w o r k  a r r a y s  *****
c  v.....of size (npoint,7)
c  a.....of size (npoint,4)
c
c*****  o u t p u t  *****
c  a(.,1).....contains the sequence of smoothed ordinates .
c  a(i,j) = f^(j-1)(x(i)), j=2,3,4, i=1,...,npoint-1 ,  i.e., the
c        first three derivatives of the smoothing spline  f  at the
c        left end of each of the data intervals .
c     w a r n i n g . . .   a  would have to be transposed before it
c        could be used in  ppvalu .
c
c******  m e t h o d  ******
c     The matrices  Q-transp*d  and  Q-transp*D**2*Q  are constructed in
c   s e t u p q  from  x  and  dy , as is the vector  qty = Q-transp*y .
c  Then, for given  p , the vector  u  is determined in  c h o l 1 d  as
c  the solution of the linear system
c               (6(1-p)Q-transp*D**2*Q + p*Q)u  = qty  .
c  From  u , the smoothing spline  f  (for this choice of smoothing par-
c  ameter  p ) is obtained in the sense that
c                        f(x(.))  =  y - 6(1-p)D**2*Q*u        and
c                      f''(x(.))  =  6*p*u                      .
c     The smoothing parameter  p  is found (if possible) so that
c                sf(p)  =  s ,
c  with  sf(p) = s(f) , where  f  is the smoothing spline as it depends
c  on  p .  if  s = 0, then p = 1 . if  sf(0) .le. s , then p = 0 .
c  Otherwise, the secant method is used to locate an appropriate  p  in
c  the open interval  (0,1) . However, straightforward application of
c  the secant method, as done in the original version of this program,
c  can be very slow and is influenced by the units in which  x  and  y 
c  are measured, as C. Reinsch has pointed out. Instead, on recommend-
c  ation from C. Reinsch, the secant method is applied to the function
c           g:q |--> 1/sqrt{sfq(q)} - 1/sqrt{s} ,
c  with  sfq(q) := sf(q/(1+q)), since  1/sqrt{sfq}  is monotone increasing
c  and close to linear for larger  q . One starts at  q = 0  with a
c  Newton step, i.e., 
c                q_0 = 0,  q_1 = -g(0)/g'(0)
c  with  g'(0) = -(1/2) sfq(0)^{-3/2} dsfq, where dsfq = -12*u-transp*r*u ,
c  and  u  as obtained for  p = 0 . Iteration terminates as soon as 
c   abs(sf - s) .le. .01*s .
c
      implicit none
c
c     logical test 
c     parameter (test = .true.)
c     integer itercnt
      integer npoint,   i,npm1
      double precision a(npoint,4),dy(npoint),s,v(npoint,7),
     &  x(npoint),y(npoint)
     &     ,change,ooss,oosf,p,prevsf,prevq,q,sfq,sixp,six1mp,utru
      call setupq(x,dy,y,npoint,v,a(1,4))
      if (s .gt. 0.0D+00)                    go to 20
   10 p = 1.0D+00
      call chol1d(p,v,a(1,4),npoint,1,a(1,3),a(1,1))
      sfq = 0.0D+00
                                        go to 60
   20 p = 0.0D+00
      call chol1d(p,v,a(1,4),npoint,1,a(1,3),a(1,1))
      sfq = 0.0D+00
      do 21 i=1,npoint
   21    sfq = sfq + (a(i,1)*dy(i))**2
      sfq = sfq*36.0D+00
      if (sfq .le. s)                   go to 60
      utru = 0.0D+00
      do 25 i=2,npoint
   25    utru = utru + v(i-1,4)*(a(i-1,3)*(a(i-1,3)+a(i,3))+a(i,3)**2)
      ooss = 1.0D+00/dsqrt(s)
      oosf = 1.0D+00/dsqrt(sfq)
      q = -(oosf-ooss)*sfq/(6.0D+00*utru*oosf)
c  secant iteration for the determination of p starts here.
c     itercnt = 0
      prevq = 0.0D+00
      prevsf = oosf
   30 call chol1d(q/(1.0D+00+q),v,a(1,4),npoint,1,a(1,3),a(1,1))
      sfq = 0.0D+00
      do 35 i=1,npoint
   35    sfq = sfq + (a(i,1)*dy(i))**2
      sfq = sfq*36.0D+00/(1.0D+00+q)**2
      if (dabs(sfq-s) .le. 0.01D+00*s)        go to 59
      oosf = 1.0D+00/dsqrt(sfq)
      change = (q-prevq)/(oosf-prevsf)*(oosf-ooss)
      prevq = q
      q = q - change
      prevsf = oosf
c     itercnt = itercnt + 1
                                        go to 30
   59 p = q/(1.0D+00+q)
correct value of p has been found.
compute pol.coefficients from  Q*u (in a(.,1)).
   60 smooth = sfq
c     if (test) then
c        print *, 'number of iterations = ', itercnt
c     end if
c     six1mp = 6./(1.0D+00+q)
      do 61 i=1,npoint
c  61    a(i,1) = y(i) - six1mp*dy(i)**2*a(i,1)
   61    a(i,1) = y(i) - 6.0D+00 * ( 1.0D+00 - p ) *dy(i)**2*a(i,1)
      sixp = 6.0D+00*p
      do 62 i=1,npoint
   62    a(i,3) = a(i,3)*sixp
      npm1 = npoint - 1
      do 63 i=1,npm1
         a(i,4) = (a(i+1,3)-a(i,3))/v(i,4)
   63    a(i,2) = (a(i+1,1)-a(i,1))/v(i,4)
     &    - (a(i,3)+a(i,4)/3.0D+00*v(i,4))/2.0D+00*v(i,4)
                                        return
      end
